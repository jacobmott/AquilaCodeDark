---
name: newsession
description: Wrap the current Aquila Code Dark session, commit and push only its owned work, and provide a restart prompt. Use for explicit newsession or wrap/commit/push/handoff requests. Honor prompt-only or no-push requests. Installing or editing this skill does not invoke it.
---
# Aquila new-session workflow

An explicit `newsession`, `/newsession` or `$newsession` request authorizes this session's wrap, commit and push.
Do not ask again for that same scope. Honor narrower wording, including prompt-only and no-push.
Copying, reviewing or installing the workflow does not authorize publication or run these steps.

1. Read `AGENTS.md`, `Docs/TaskRoutes.md`, `.claude/skills/session-wrap/SKILL.md`, `Docs/SessionSave.md`
   and `.claude/rules/commits.md`; reuse complete supplied text. Select only this task through `HANDOFF.md`.
2. Inspect owned diffs, upstream state, recent commits, actual report inbox answers, checks and active processes.
   Preserve all other sessions' files, staging, notes, reports and running work. Do not infer ownership from dirty status.
3. Follow session-wrap and the save policy. Harvest durable knowledge, update this checkpoint and its root row,
   and keep decisions, consumed authority, unresolved choices and verification limits explicit.
4. Save a dated `Docs/Handoffs/PROMPT_<date>_<task>.txt` and identical boot block in the checkpoint.
   Include repo path, core/index, selected checkpoint, report link with all parameters, last-read inbox filename,
   checked state, active processes, two or three costly traps, and next choices with their approval requirements.
   End: `Do not start any of this. Read, say where things stand, then wait.`
5. For a full invocation, commit through the ownership procedure in the commit rule, run appropriate checks,
   and push the configured upstream. Never broad-stage, amend peer commits, force-push or merge live branches as cleanup.
   If publication is blocked, preserve the work and state the exact blocker. Prompt-only requests skip this step.
6. Verify the committed diff and remote presence. Reconcile the saved prompt with actual results without an endless receipt-commit loop.
   Report skipped steps and owned/peer leftovers. Do not imply backup, deployment, provider spend or resumed game work.
7. Reply with the report link and publication evidence, then the saved prompt as a copyable text block.
