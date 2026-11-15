"""Approved Aquila storage provisioning. Preview unless --apply; keeps local policy receipts."""
import argparse
import hashlib
import json
import pathlib
import subprocess
from datetime import datetime, timezone

ROOT = pathlib.Path(__file__).resolve().parents[1]
LEGACY = "jacobmottgithub"
PREFIX = "AquilaCodeDark/"
BUCKET = "jacobmott-aquila-code-dark"


def aws(*args, optional=False):
    p = subprocess.run(["aws", *args, "--output", "json", "--no-cli-pager",
                        "--cli-connect-timeout", "10", "--cli-read-timeout", "60"], capture_output=True, text=True)
    if p.returncode:
        if optional and any(x in p.stderr for x in ("NoSuchBucket", "404", "NoSuchKey")):
            return None
        raise RuntimeError(f"AWS {args[0]} {args[1]} failed; no response or secret printed")
    return json.loads(p.stdout) if p.stdout.strip() else {}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    folder = ROOT / ".runtime" / "setup" / stamp
    account = aws("sts", "get-caller-identity")["Account"]
    raw = aws("s3api", "get-bucket-policy", "--bucket", LEGACY)["Policy"]
    old = json.loads(raw)
    new = json.loads(raw)
    resource = f"arn:aws:s3:::{LEGACY}/{PREFIX}*"
    removed = []
    kept = []
    for statement in old["Statement"]:
        if statement.get("Effect") == "Allow" and statement.get("Principal") == "*" and statement.get("Resource") == resource:
            removed.append(statement)
        else:
            kept.append(statement)
    if not removed and not any(s.get("Sid") == "AquilaPrivateBackupOnly" for s in kept):
        raise RuntimeError("Legacy policy changed; inspect before applying")
    new["Statement"] = kept
    if not any(s.get("Sid") == "AquilaPrivateBackupOnly" for s in kept):
        new["Statement"].append({"Sid": "AquilaPrivateBackupOnly", "Effect": "Deny", "Principal": "*",
                                 "Action": "s3:GetObject", "Resource": resource,
                                 "Condition": {"StringNotEquals": {"aws:PrincipalAccount": account}}})
    summary = {"legacy_public_statements_removed": len(removed), "other_statements_preserved": len(kept),
               "dedicated_bucket": BUCKET, "applied": args.apply}
    print(json.dumps(summary))
    if not args.apply:
        return
    folder.mkdir(parents=True, exist_ok=False)
    (folder / "legacy-policy.before.json").write_text(json.dumps(old, indent=2)+"\n", encoding="utf-8")
    (folder / "legacy-policy.after.json").write_text(json.dumps(new, indent=2)+"\n", encoding="utf-8")
    if aws("s3api", "get-bucket-policy", "--bucket", LEGACY)["Policy"] != raw:
        raise RuntimeError("Concurrent bucket policy edit; stopped")
    aws("s3api", "put-bucket-policy", "--bucket", LEGACY, "--policy", json.dumps(new))
    actual = json.loads(aws("s3api", "get-bucket-policy", "--bucket", LEGACY)["Policy"])
    if actual != new:
        raise RuntimeError("Legacy policy read-back mismatch")
    names = {x["Name"] for x in aws("s3api", "list-buckets")["Buckets"]}
    if BUCKET not in names:
        aws("s3api", "create-bucket", "--bucket", BUCKET, "--region", "us-east-1", "--object-ownership", "BucketOwnerEnforced")
    aws("s3api", "put-public-access-block", "--bucket", BUCKET, "--public-access-block-configuration",
        json.dumps({k: True for k in ("BlockPublicAcls", "IgnorePublicAcls", "BlockPublicPolicy", "RestrictPublicBuckets")}))
    aws("s3api", "put-bucket-encryption", "--bucket", BUCKET, "--server-side-encryption-configuration",
        json.dumps({"Rules": [{"ApplyServerSideEncryptionByDefault": {"SSEAlgorithm": "AES256"}}]}))
    aws("s3api", "put-bucket-ownership-controls", "--bucket", BUCKET, "--ownership-controls",
        json.dumps({"Rules": [{"ObjectOwnership": "BucketOwnerEnforced"}]}))
    policy = {"Version": "2012-10-17", "Statement": [{"Sid": "DenyNonTLS", "Effect": "Deny", "Principal": "*",
               "Action": "s3:*", "Resource": [f"arn:aws:s3:::{BUCKET}", f"arn:aws:s3:::{BUCKET}/*"],
               "Condition": {"Bool": {"aws:SecureTransport": "false"}}}]}
    aws("s3api", "put-bucket-policy", "--bucket", BUCKET, "--policy", json.dumps(policy))
    aws("s3api", "put-bucket-tagging", "--bucket", BUCKET, "--tagging",
        json.dumps({"TagSet": [{"Key": "Project", "Value": "aquila-code-dark"}, {"Key": "Purpose", "Value": "private-assets"}]}))
    verification = {"public_access": aws("s3api", "get-public-access-block", "--bucket", BUCKET),
                    "encryption": aws("s3api", "get-bucket-encryption", "--bucket", BUCKET),
                    "ownership": aws("s3api", "get-bucket-ownership-controls", "--bucket", BUCKET),
                    "versioning": aws("s3api", "get-bucket-versioning", "--bucket", BUCKET),
                    "policy_status": aws("s3api", "get-bucket-policy-status", "--bucket", BUCKET)}
    assert all(verification["public_access"]["PublicAccessBlockConfiguration"].values())
    assert not verification["policy_status"]["PolicyStatus"]["IsPublic"]
    assert verification["versioning"].get("Status") != "Enabled"
    summary.update(verification=verification, legacy_policy_sha256=hashlib.sha256(json.dumps(actual, sort_keys=True).encode()).hexdigest())
    (folder / "receipt.json").write_text(json.dumps(summary, indent=2)+"\n", encoding="utf-8")
    print(json.dumps({"verified": True, "receipt": str(folder / "receipt.json")}))


if __name__ == "__main__":
    main()
