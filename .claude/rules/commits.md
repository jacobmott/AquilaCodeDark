---
paths:
  - ".claude/skills/newsession/**"
  - ".agents/skills/newsession/**"
---
# Commit only this session's work

Commit only on the user's request in this session; push when committing unless explicitly excluded.
A normal wrap or installing a skill is not permission. An explicit newsession invocation includes commit and push.
Even "commit everything" means this session's work only.

1. Inspect status, recent commits, the upstream and every owned diff. A peer may have already committed the work.
2. Never use `git add .`, `git add -A`, broad `git add -u`, `git commit -a`, or an unscoped commit through the shared index.
3. For tracked files owned entirely by this session, `git commit --only -F <message-file> -- <explicit paths>` is sufficient.
4. For new files, shared notes or partially owned files, create a unique session index using `GIT_INDEX_FILE`.
   Seed it with `git read-tree HEAD`. Stage explicitly owned files. For shared files, construct HEAD plus only this session's hunks,
   create its blob using `git hash-object -w --path=<path> <owned-copy>`, and update only that private index.
   Inspect its complete staged diff and run the Git guard and whitespace check against that index before committing.
5. Snapshot the real index's staged changes before the private-index commit. Never reset a path holding peer staging.
   After committing, reconcile only owned paths whose prior index entry was identical to old HEAD and has not changed concurrently.
   Preserve existing peer staging exactly. Report any remaining index reconciliation rather than overwriting it.
6. Remove only this session's temporary index and restore the prior `GIT_INDEX_FILE` environment value.
   Verify the commit's paths and hunks. Recheck upstream before pushing; never force-push or merge live branches as cleanup.
7. A push can carry earlier peer commits already on the branch. Inspect and report that; never amend or remove their commits.
8. Verify remote presence and report owned unpushed work and peer leftovers accurately. If nothing is ours, commit nothing.

Store multiline messages in a UTF-8 file and use `-F`; never interpolate shell-sensitive prose.
