# Shared application connections

Stable project identity: `aquila-code-dark`. This checkout owns its code, assets, approvals and production decisions.
Use the installed applications; do not create duplicate servers, copy their databases or vendor their code.

| Application | Binding and use |
| --- | --- |
| Agent Report | `reports/kit.path` points to `D:/OtherProjects/AgentReport`; each task owns its report folder and inbox. Read `.claude/rules/reports.md`. |
| Asset Ledger | `.asset-ledger.json`, repository `aquila-code-dark`, initial project `aquila-code-dark-game`. Site: <http://127.0.0.1:8790/>. |
| Research Console | `.research-console.json`; project page <http://127.0.0.1:8776/?project=aquila-code-dark>. Shared CLI: `C:/ResearchConsole/research.py`. |
| Knowledge Hub | `.knowledge-hub.json`; use `C:/KnowledgeHub/hub.py` with actual provider identity. |

## Generated assets

Before generation, read [the Asset Ledger guide](<F:/OtherProjects/asset-ledger/Docs/AGENT_GUIDE.md>) in full.
Keep a supported runner receipt or write the required record immediately after a job. Retain actual prompts, references, model,
settings, provider IDs and outputs. Music, voice and sound effects use its audio format. Unknown historical prompts remain unknown.
The record-files layout watches `Assets` and `Docs`; initial ownership maps them to `aquila-code-dark-game`.
Raw records stay ignored. Outputs can remain at their existing repo locations; do not duplicate media for the catalog.
Declare a new task's ownership explicitly when it should have a separate ledger project.
Service health and capture visibility are not proof of a real generation. No synthetic provider job is inserted during setup.

## Research

Read [Research Console's guide](<C:/ResearchConsole/AGENT_GUIDE.md>) in full before submitting, discovering or retrieving research.
From this repo, `py -B C:/ResearchConsole/research.py pending` checks only Aquila's results.
Retain original research separately from synthesis and record exact reviewed versions. Setup does not submit research.

Before relevant new implementation or research, read [Knowledge Hub's client guide](<C:/KnowledgeHub/Docs/Operations/REPO_CLIENT.md>) in full.
Codex uses `--provider openai`, direct Claude uses `--provider anthropic`, and Cursor uses `--provider cursor`.
Never borrow another destination or bypass a denial with file reads. Search and exact-read only permitted sources.
Findings remain evidence, not execution authority. Preserve review state, freshness, applicability and exact citations.

## Verification and recovery

Use each application's supported health/registration routes. Do not directly edit databases.
Research Console setup is idempotent through `research.py init --repo <checkout> --project aquila-code-dark --name "Aquila Code Dark" --install-rule`.
Hub setup previews with `hub.py init --repo <checkout> --project aquila-code-dark --provider <actual-provider>`; `--apply` needs setup authority.
Asset Ledger's repo kit previews before `--apply`; repository registration is separate from installing the local binding.
Existing global Agent Report hooks cover Claude and Codex. The shared root block supplies a fallback; unsupported hosts need explicit checks.
The setup report uses a unique `repo-setup` launcher slug. No other session's report registration is reused.
