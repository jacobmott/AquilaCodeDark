# Shared context and account setup

The common core is `AGENTS.md`; `CLAUDE.md` imports only that core. `Docs/TaskRoutes.md` routes detail on demand.
Project knowledge, lessons, decisions and current checkpoints remain readable by every client through the repository.

## Claude accounts

All three existing sign-ins keep their independent configuration and session history:

| Account | Native project memory |
| --- | --- |
| Default | `~/.claude/projects/D--OtherProjects-AquilaCodeDark/memory` |
| Content | `~/.claude-content/projects/D--OtherProjects-AquilaCodeDark/memory` |
| Third | `~/.claude-duskforge3/projects/D--OtherProjects-AquilaCodeDark/memory` |

The setup links these directories to `~/ClaudeMemory/aquila-code-dark/memory` and adds narrow read/edit permissions.
It preserves prior empty directories and account settings backups. Existing nonempty notes or unexpected links stop setup for review.
Preview with `pwsh -NoProfile -File scripts/setup_shared_memory.ps1`; apply only under account-setup authority.
The shared folder does not serialize writers: re-read before editing and preserve peer changes.
Do not copy private account details, tokens or whole project histories into this memory.

## Skills and Codex

Canonical workflows: `.claude/skills/newsession/` and `.claude/skills/session-wrap/`.
Codex/shared adapters: matching `.agents/skills/` folders pointing to those files.
The global session-wrap skill can still be discovered; this repo's save policy governs its project-specific behavior.
An installation is not a skill invocation and does not commit anything.

Codex already trusts this project. Repo config turns off automatic global-memory use and the unrelated trading MCP here,
matching Production Tools. Stored personal memories and global tool registrations remain intact.
This running conversation retains already supplied context. A new session must confirm its effective loading.
[Official skill discovery](https://learn.chatgpt.com/docs/build-skills) and [configuration layers](https://learn.chatgpt.com/docs/config-file/config-basic).

Existing global Agent Report hooks are configured for all three Claude accounts and Codex.
The root report block covers clients where hooks are unavailable. No hook trust bypass is installed.
The Git guard runs independently through local `core.hooksPath=.githooks`.
Filesystem checks prove routing and identical bytes; they do not prove fresh model startup, skill selection or compliance.
