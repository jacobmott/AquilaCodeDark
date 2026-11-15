# Storage and recovery

Code, authored SVG templates, documentation and sanitized manifests belong in Git.
Media, raw generation receipts, secrets, scratch, caches and asset source files stay outside Git.
Asset Ledger records and report inboxes are ignored. No Git LFS or encoded-media workaround.
The existing favicon, embedded-image SVG and HTML snippet are grandfathered by exact hash in `config/git-artifact-exceptions.json`.
New or changed embedded media is refused; source-native SVG remains allowed.

## Dedicated private store

Bucket: `jacobmott-aquila-code-dark`, region `us-east-1`.
All public-access blocks are enabled, ownership is BucketOwnerEnforced, encryption is AES256, policy requires TLS.
Versioning is not enabled and must remain off, matching Production Tools.
The legacy `jacobmottgithub/AquilaCodeDark/` backup prefix was made private without changing other project policies.
Old objects remain preserved. The separate `aquilacode.io` website bucket and CloudFront deployment are unchanged.

Folders retain their working paths: `src/assets`, `AllAssets`, `Tiled`, and future `Assets`, `Screenshots`, `Videos`.
`config/asset-storage.json` is the explicit backup allowlist. Secrets, diagnostics, credentials, linked directories and caches are excluded.
Source files in these asset folders include maps and authoring data; they stay with the corresponding asset backup.
Report media requires its own scoped backup configuration; default asset backup never scans the repository indiscriminately.

## Commands

Install the isolated helper environment once, without changing global Python:

```powershell
py -3.14 -m venv .venv-tools
.\.venv-tools\Scripts\python.exe -m pip install -r scripts/requirements-storage.txt
.\.venv-tools\Scripts\python.exe scripts/asset_store.py backup
```

The first call previews count, bytes and conflicts. Only under transfer authority, add `--apply --receipt .runtime/storage/<unique-name>.json`.
The helper hashes inputs, checks remote metadata, uses conditional creates and server-validated SHA-256, and reads back an immutable manifest.
It never overwrites or deletes an existing object. Changed assets stop before writes; an authorized new `--namespace revisions/<label>`
preserves both versions. An interrupted upload can resume: identical completed objects are skipped.
An upload receipt is not restore proof.

Restore preview, then add `--apply` when authorized:

```powershell
.\.venv-tools\Scripts\python.exe scripts/asset_store.py restore --manifest-key _manifests/<receipt-key>.json --target D:\Recovery\AquilaCodeDark
```

Target must be new or empty and isolated. Every restored file is checked against manifest SHA-256 and size.
Never restore directly over the working tree. Compare first, then select replacements under explicit scope.
Manifests can be listed with `aws s3 ls s3://jacobmott-aquila-code-dark/_manifests/`.
The helper uses existing AWS credentials; never copy credentials into config or receipts.

`scripts/setup_storage.py` is the guarded provisioning receipt generator, not the normal backup command.
Its default is a preview. Do not rerun apply without provisioning authority.
S3 conditional-create semantics: [AWS PutObject reference](https://docs.aws.amazon.com/boto3/latest/reference/services/s3/client/put_object.html).
