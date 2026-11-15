"""Check the full selected Git index without changing it. Exceptions are pinned, never inferred."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
MEDIA = set('png jpg jpeg gif webp bmp tif tiff heic ico mp4 mov webm mkv avi m4v mp3 wav m4a ogg flac woff woff2 ttf otf blend fbx glb gltf obj stl usd usda usdc usdz uasset umap psd psb zip 7z rar tar gz exe dll pyd pdb bin safetensors pt pth onnx ckpt sqlite sqlite3 db pdf docx xlsx pptx'.split())
FORBIDDEN = {'.runtime', 'secrets', 'allassets', 'tiled', 'assets', '.venv-tools', 'node_modules', '.angular', 'dist', 'out', '__pycache__'}


def rejected(path, data, exceptions):
    p = PurePosixPath(path)
    parts = [x.lower() for x in p.parts]
    n = parts[-1]
    if any(x in FORBIDDEN for x in parts):
        return 'local-only folder'
    if n in {'.env', 'credentials', 'settings.local.json'} or (n.startswith('.env.') and n != '.env.example'):
        return 'private configuration'
    if n.endswith(('.pem', '.key', '.pfx', '.p12', '.ledger-record.json', '.ledger-draft.json')) or ('reports' in parts and 'inbox' in parts):
        return 'private evidence or credentials'
    if re.search(rb'(?:AKIA|ASIA)[A-Z0-9]{16}', data) or re.search(rb'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----', data):
        return 'credential pattern; contents withheld'
    exception = exceptions.get(path)
    if exception and hashlib.sha256(data).hexdigest() == exception['sha256']:
        return None
    if p.suffix.lower().lstrip('.') in MEDIA:
        return 'binary/media asset outside Git (or changed pinned legacy exception)'
    try:
        value = data.decode('utf-8-sig')
    except UnicodeDecodeError:
        return 'not UTF-8 text'
    if any(ord(c) < 32 and c not in '\t\r\n' for c in value):
        return 'binary/control bytes'
    if re.search(r'data:(?:image|audio|video|application/(?:pdf|zip|octet-stream))[^,\s]*;base64,[A-Za-z0-9+/=\s]{128,}', value, re.I):
        return 'embedded media (or changed pinned legacy exception)'
    if len(data) > 5 * 1024**2:
        return 'text exceeds 5 MiB; review source versus encoded asset'
    return None


def main():
    result = subprocess.run(['git', 'show', ':config/git-artifact-exceptions.json'], cwd=ROOT, capture_output=True)
    exceptions = json.loads(result.stdout)['exceptions'] if result.returncode == 0 else {}
    rows = subprocess.check_output(['git', 'ls-files', '--stage', '-z'], cwd=ROOT)
    failures = []
    count = 0
    for row in rows.split(b'\0'):
        if not row:
            continue
        meta, name = row.split(b'\t', 1)
        mode, oid, stage = meta.split()
        path = name.decode('utf-8')
        count += 1
        if stage != b'0' or mode not in {b'100644', b'100755'}:
            reason = 'unmerged or non-regular entry'
        else:
            data = subprocess.check_output(['git', 'cat-file', 'blob', oid.decode()], cwd=ROOT)
            reason = rejected(path, data, exceptions)
        if reason:
            failures.append((path, reason))
    for path, reason in failures:
        print(f'REFUSED {path}: {reason}', file=sys.stderr)
    print(json.dumps({'indexed_files': count, 'failures': len(failures)}))
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
