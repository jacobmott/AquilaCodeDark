---
name: session-wrap
description: Save Aquila Code Dark knowledge and its current task checkpoint, then provide a restart prompt. Use for a wrap, handoff or end-of-session save. Does not itself commit, push, upload or deploy.
---
# Save the Aquila session

Read `AGENTS.md`, `Docs/TaskRoutes.md` and `Docs/SessionSave.md`; reuse complete supplied text.
Select this task's checkpoint through `HANDOFF.md`, then search the knowledge index for matching topics.
Inspect this session's changes, actual report inbox answers, tests, approvals and running work.
Follow the save policy: harvest facts and lessons, update changed decisions/backlog, archive exact prior checkpoint bytes,
replace current state, and update only this task's root row and report.
Preserve peers and the shared index. Do not write global personal memory without explicit authorization.

Verify local links and checkpoint size with `py -B scripts/check-startup-context.py --handoff <path>`.
Record untested behavior, owned dirty paths, failures and any still-running process.
Supply a copyable restart prompt naming this repo and its current checkpoint, report and last-read inbox receipt.
Present open choices without starting them. End the prompt exactly:
`Do not start any of this. Read, say where things stand, then wait.`

Commit, push, sync and deployment require their own authorization. If explicit newsession was requested, follow its full skill.
