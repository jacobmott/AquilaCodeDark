# Aquila Code Dark

Existing Angular and Phaser game with Rapier physics and an Electron desktop shell.

Start agents at [AGENTS.md](AGENTS.md) and [the task index](Docs/TaskRoutes.md).
Current work: [HANDOFF.md](HANDOFF.md). Shared knowledge: [knowledge/README.md](knowledge/README.md).

## Run and build

```powershell
npm run serve:dev
npm run build:prod
npm run electron:dev
```

A fresh clone needs locked dependencies (`npm ci`) and restored game assets.
Read [development and recovery](knowledge/development.md) before installation, restore or deployment.
Builds do not deploy. The existing production website is https://aquilacode.io.

## Storage

Code and project knowledge stay in Git. Runtime art, audio, maps and source assets use the dedicated private S3 bucket.
Read [storage and recovery](knowledge/storage.md) for preview, conflict handling and verified isolated restore.
Never upload Secrets, credentials, diagnostics or scratch. Backup never deletes or overwrites conflicting data.
The old shared-bucket backup prefix has been made private; the website bucket remains separate.

## Sessions and shared applications

- `session-wrap`: save shared knowledge, decisions, checkpoint and restart prompt.
- `newsession`: save, commit and push only the invoking session's work, then supply the prompt.
- Normal work and installing skills do not authorize commits. Shared files and the Git index may contain other sessions' work.
- All three Claude accounts share one Aquila memory directory; Codex uses the same repository knowledge.
- [Shared integrations](knowledge/integrations.md): Agent Report, Asset Ledger, Research Console and Knowledge Hub.

Local setup checks and evidence: [setup checkpoint](Docs/Handoffs/HANDOFF_2026-11-14_setup.md).
The [old README](Docs/History/setup/README.before-20261114.md) remains historical evidence, not current operating instructions.
