---
paths:
  - "scripts/asset_store.py"
  - "knowledge/storage.md"
  - "config/asset-storage.json"
---
# Private asset storage
Read `knowledge/storage.md`. Preview before apply. Never delete, overwrite a conflicting asset, upload secrets or enable versioning.
Use the dedicated private bucket. Website deployment is a separate workflow requiring explicit authority.
Restore into a new isolated directory first and verify hashes before selecting any replacement for working files.
