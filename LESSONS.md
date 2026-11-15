# Lessons

## 2026-11-14 [M]: Check real storage permissions
The legacy shared-bucket policy allowed anonymous reads of the entire Aquila backup prefix, including its Secrets folder.
Folder naming and a private Git remote do not establish S3 privacy. Verify bucket policy, public-access blocks and anonymous access.
Evidence and remediation: `knowledge/storage.md` and the setup receipt.

## 2026-11-14 [M]: Preserve client-specific loading
Claude imports the shared core through `CLAUDE.md`; Codex discovers repo skills through `.agents/skills/`.
Three Claude accounts need links to the same project-memory folder. Configured files do not prove fresh model loading.

## 2026-11-14 [M]: A text extension can contain media
An existing SVG template and HTML snippet contain embedded raster images. Check content as well as extensions.
Keep grandfathered exceptions hash-pinned; do not ban authored SVG templates or silently expand exceptions.

## 2026-11-15 [M]: Verify an actual isolated restore
The first concurrent restore stopped during missing-parent path resolution on Windows; the upload had passed.
Preparing directories serially before parallel downloads passed the nested-directory regression and all 1,557 real asset hashes.
Evidence: `Docs/Setup/storage-receipt.json` and `scripts/tests/test_repo_safety.py`.
Treat upload receipts as upload proof; verify a separate restore before claiming recoverability.

## 2026-11-15 [M]: Separate index bytes from staged content
The Git index byte hash changed after checks while staged content still exactly matched the starting commit.
A byte-hash mismatch alone does not prove a peer staged changes. Inspect exact mode/blob/stage entries before any reconciliation.
Evidence: `Docs/Setup/verification.json`. Preserve unknown changes and use an index lock for authorized owned-entry reconciliation.
