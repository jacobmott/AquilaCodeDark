# Save current state; link history

This policy saves context. It grants no commit, push, upload, deployment, spend or task-resumption authority.
An explicit newsession invocation has the publication scope stated in its skill.

1. Inspect this session's changes, validation, decisions, unanswered report submissions and active processes.
   Re-read shared notes before editing; preserve peer edits and the shared index.
2. Update the selected task's current checkpoint under `Docs/Handoffs/`.
   Include verified state, evidence, allowed scope, consumed approvals, paused/running work, open choices,
   owned dirty/unpushed paths, failures and the next permitted step or required approval.
3. Merge durable facts into the matching `knowledge/` note. Add only new lessons to `LESSONS.md`, dated and tagged
   `[J]` for owner decisions, `[M]` for measured facts or `[R]` for researched claims. Update decisions/backlog only when changed.
4. Before shortening an existing owned checkpoint, retain its exact bytes under `Docs/History/<task>/<UTC timestamp>.md`.
   Record original path, time and SHA-256. Link history rather than copying it into startup.
   Keep unpublished peer captures in ignored `.runtime/`; never rewrite another session's checkpoint.
5. Update only this task's row in `HANDOFF.md`. Root index stays below 9,000 bytes.
   Current checkpoints target at most 1,200 words and 10,000 UTF-8 bytes; split supporting detail without losing authority or stop boundaries.
6. Update this task's report and preserve earlier rounds and unanswered questions. Never fabricate inbox submissions.
7. A restart prompt names the repository, task, shared core, task index, checkpoint, report and last-read inbox filename.
   State tested versus untested work, active processes and remaining choices. Link other evidence for conditional use.
   End exactly: `Do not start any of this. Read, say where things stand, then wait.`
8. Run `py -B scripts/check-startup-context.py --handoff <path>` after changing startup or a checkpoint.
   Validate links and compare preserved boundaries. File checks do not prove fresh client loading or model compliance.

Keep personal memory supplementary. Do not write global Codex memory unless explicitly requested through its permitted mechanism.
