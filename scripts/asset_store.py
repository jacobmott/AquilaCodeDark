"""Preview-first private asset backup and hash-verified isolated restore. Never deletes or overwrites."""
import argparse
import base64
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import sys
from datetime import datetime, timezone
from uuid import uuid4

ROOT = Path(__file__).resolve().parents[1]
BLOCKED = {"secrets", ".git", ".aws", ".runtime", "diagnostics", "localonly", "node_modules", "__pycache__"}


def safe_relative(value):
    p = PurePosixPath(value)
    if not value or p.is_absolute() or "\\" in value or any(x in {".", "..", ""} for x in value.split("/")):
        raise ValueError("Unsafe relative path")
    for part in p.parts:
        if re.search(r'[<>:"|?*\x00-\x1f]', part) or part.endswith((".", " ")):
            raise ValueError("Unsafe Windows path")
        if re.fullmatch(r'(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\..*)?', part, re.I):
            raise ValueError("Reserved Windows name")
    return p


def excluded(relative):
    p = safe_relative(relative)
    names = [x.lower() for x in p.parts]
    n = names[-1]
    return (any(x in BLOCKED for x in names) or n == ".env" or n.startswith(".env.")
            or n.endswith((".pem", ".key", ".pfx", ".p12", ".tmp", ".bak"))
            or n in {"credentials", "thumbs.db", "desktop.ini", ".ds_store"}
            or "secret" in n or "credential" in n)


def digest(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def inventory(root, folders, namespace):
    safe_relative(namespace)
    found = {}
    for folder in folders:
        safe_relative(folder)
        base = root / folder
        if not base.exists():
            continue
        if base.is_symlink() or (hasattr(base, "is_junction") and base.is_junction()):
            raise ValueError(f"Linked asset root refused: {folder}")
        for directory, dirs, files in os.walk(base, followlinks=False):
            for name in list(dirs):
                child = Path(directory) / name
                if child.is_symlink() or (hasattr(child, "is_junction") and child.is_junction()):
                    raise ValueError("Linked asset directory refused")
                if name.lower() in BLOCKED:
                    dirs.remove(name)
            for name in files:
                path = Path(directory) / name
                relative = path.relative_to(root).as_posix()
                if excluded(relative):
                    continue
                if path.is_symlink() or not path.resolve().is_relative_to(root.resolve()):
                    raise ValueError("Asset escaped repository")
                stat = path.stat()
                if stat.st_size > 5 * 1024**3:
                    raise ValueError("Single file exceeds 5 GiB; use a separately reviewed multipart workflow")
                row = {"path": relative, "key": namespace + "/" + relative, "bytes": stat.st_size,
                       "sha256": digest(path), "mtime_ns": stat.st_mtime_ns}
                if path.stat().st_mtime_ns != stat.st_mtime_ns:
                    raise ValueError("Asset changed during inventory")
                if relative.casefold() in found:
                    raise ValueError("Duplicate or case-colliding asset path")
                found[relative.casefold()] = row
    return sorted(found.values(), key=lambda x: x["path"])


def head(client, bucket, key):
    from botocore.exceptions import ClientError
    try:
        return client.head_object(Bucket=bucket, Key=key)
    except ClientError as exc:
        if exc.response["Error"]["Code"] in {"404", "NoSuchKey", "NotFound"}:
            return None
        raise


def matching(info, row):
    return info is not None and info["ContentLength"] == row["bytes"] and info.get("Metadata", {}).get("sha256") == row["sha256"]


def backup(client, config, args):
    rows = inventory(ROOT, config["folders"], args.namespace or config["default_namespace"])
    if not rows:
        raise ValueError("No eligible assets found")
    bucket = config["bucket"]
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
        info = list(pool.map(lambda row: head(client, bucket, row["key"]), rows))
    conflicts = [row["path"] for row, old in zip(rows, info) if old is not None and not matching(old, row)]
    missing = [row for row, old in zip(rows, info) if old is None]
    summary = {"mode": "backup", "applied": args.apply, "files": len(rows), "bytes": sum(x["bytes"] for x in rows),
               "new_files": len(missing), "new_bytes": sum(x["bytes"] for x in missing), "conflicts": conflicts}
    print(json.dumps(summary), flush=True)
    if conflicts:
        raise ValueError("Conflicts held; choose a new --namespace after reviewing the changed assets")
    if not args.apply:
        return summary

    def upload(row):
        path = ROOT / row["path"]
        stat = path.stat()
        if stat.st_size != row["bytes"] or stat.st_mtime_ns != row["mtime_ns"]:
            raise ValueError("Source changed after preview inventory")
        with path.open("rb") as stream:
            client.put_object(Bucket=bucket, Key=row["key"], Body=stream, ContentLength=row["bytes"],
                              IfNoneMatch="*", Metadata={"sha256": row["sha256"]},
                              ChecksumSHA256=base64.b64encode(bytes.fromhex(row["sha256"])).decode(), ServerSideEncryption="AES256")
        if not matching(head(client, bucket, row["key"]), row):
            raise ValueError("Remote metadata verification failed")
        return row["bytes"]

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
        for count, _ in enumerate(pool.map(upload, missing), 1):
            if count % 100 == 0:
                print(json.dumps({"uploaded_files": count, "of": len(missing)}), flush=True)
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    manifest = {"schema": "aquila.asset-manifest/1", "created_utc": stamp, "bucket": bucket,
                "files": [{k: v for k, v in row.items() if k != "mtime_ns"} for row in rows]}
    body = (json.dumps(manifest, indent=2) + "\n").encode()
    key = f"_manifests/{stamp}-{uuid4().hex[:8]}.json"
    client.put_object(Bucket=bucket, Key=key, Body=body, IfNoneMatch="*", ContentType="application/json",
                      ChecksumSHA256=base64.b64encode(hashlib.sha256(body).digest()).decode(), ServerSideEncryption="AES256")
    returned = client.get_object(Bucket=bucket, Key=key)["Body"].read()
    if returned != body:
        raise ValueError("Manifest read-back mismatch")
    summary.update(manifest_key=key, manifest_sha256=hashlib.sha256(body).hexdigest(), verified="upload checksum and remote metadata; restore pending")
    return summary


def validate_manifest(manifest, bucket):
    if manifest.get("schema") != "aquila.asset-manifest/1" or manifest.get("bucket") != bucket or not manifest.get("files"):
        raise ValueError("Wrong or empty manifest")
    seen = set()
    for row in manifest["files"]:
        safe_relative(row["path"])
        safe_relative(row["key"])
        if excluded(row["path"]) or not row["key"].endswith("/" + row["path"]):
            raise ValueError("Unexpected asset manifest path")
        if row["path"].casefold() in seen or not re.fullmatch(r"[0-9a-f]{64}", row["sha256"]):
            raise ValueError("Duplicate path or invalid hash")
        if not isinstance(row["bytes"], int) or row["bytes"] < 0:
            raise ValueError("Invalid size")
        seen.add(row["path"].casefold())


def restore(client, config, args):
    safe_relative(args.manifest_key)
    if not args.manifest_key.startswith("_manifests/"):
        raise ValueError("Expected an immutable manifest key")
    raw = client.get_object(Bucket=config["bucket"], Key=args.manifest_key)["Body"].read()
    manifest = json.loads(raw)
    validate_manifest(manifest, config["bucket"])
    target = Path(args.target).absolute()
    if target.is_symlink() or (hasattr(target, "is_junction") and target.is_junction()):
        raise ValueError("Linked restore target refused")
    if target.exists() and any(target.iterdir()):
        raise ValueError("Restore target must be new or empty")
    resolved = target.resolve()
    if resolved == ROOT or (resolved.is_relative_to(ROOT) and not resolved.is_relative_to(ROOT / ".runtime" / "restores")):
        raise ValueError("Restore must be isolated from working files")
    rows = manifest["files"]
    summary = {"mode": "restore", "applied": args.apply, "files": len(rows), "bytes": sum(r["bytes"] for r in rows),
               "manifest_key": args.manifest_key, "target": str(target)}
    print(json.dumps(summary), flush=True)
    if not args.apply:
        return summary
    target.mkdir(parents=True, exist_ok=True)
    resolved = target.resolve(strict=True)
    # Prepare the directory tree serially. Resolving missing Windows parents
    # while other workers create them can return inconsistent transient paths.
    for row in rows:
        parent = (target / row["path"]).parent
        if not parent.resolve().is_relative_to(resolved):
            raise ValueError("Restore parent escaped target")
        parent.mkdir(parents=True, exist_ok=True)
        if not parent.resolve(strict=True).is_relative_to(resolved):
            raise ValueError("Restore parent escaped target after creation")

    def download(row):
        path = target / row["path"]
        if not path.parent.resolve(strict=True).is_relative_to(resolved):
            raise ValueError("Restore path escaped target")
        body = client.get_object(Bucket=config["bucket"], Key=row["key"])["Body"]
        value = hashlib.sha256()
        size = 0
        with path.open("xb") as out:
            for chunk in iter(lambda: body.read(1024 * 1024), b""):
                out.write(chunk)
                value.update(chunk)
                size += len(chunk)
        body.close()
        if value.hexdigest() != row["sha256"] or size != row["bytes"]:
            raise ValueError("Restored asset hash or size mismatch; partial evidence retained")

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
        for count, _ in enumerate(pool.map(download, rows), 1):
            if count % 100 == 0:
                print(json.dumps({"verified_files": count, "of": len(rows)}), flush=True)
    summary["verified"] = "all restored file bytes and SHA-256 hashes match manifest"
    return summary


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("operation", choices=("backup", "restore"))
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--namespace")
    ap.add_argument("--manifest-key")
    ap.add_argument("--target")
    ap.add_argument("--receipt")
    ap.add_argument("--workers", type=int, choices=range(1, 17), default=8)
    args = ap.parse_args()
    if args.operation == "restore" and (not args.manifest_key or not args.target):
        ap.error("restore needs --manifest-key and --target")
    import boto3
    from botocore.config import Config
    config = json.loads((ROOT / "config" / "asset-storage.json").read_text())
    client = boto3.client("s3", region_name=config["region"], config=Config(max_pool_connections=20, retries={"max_attempts": 3}))
    security = client.get_public_access_block(Bucket=config["bucket"])["PublicAccessBlockConfiguration"]
    if not all(security.values()) or client.get_bucket_versioning(Bucket=config["bucket"]).get("Status") == "Enabled":
        raise ValueError("Bucket security/versioning differs from approved storage contract")
    result = backup(client, config, args) if args.operation == "backup" else restore(client, config, args)
    if args.receipt:
        path = Path(args.receipt)
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("x", encoding="utf-8", newline="\n") as out:
            json.dump(result, out, indent=2)
            out.write("\n")
    print(json.dumps(result), flush=True)


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(f"Stopped: {type(exc).__name__}: {exc}", file=sys.stderr)
        raise SystemExit(1)
