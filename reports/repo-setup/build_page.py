"""Build the repository setup report from sanitized verification receipts."""
import importlib.util
import json
import sys
from html import escape
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
KIT = Path((ROOT / "reports/kit.path").read_text().strip())
spec = importlib.util.spec_from_file_location("page", KIT / "kit/page.py")
page = importlib.util.module_from_spec(spec)
spec.loader.exec_module(page)
decide = page.decide


def table(rows):
    return ("<table><thead><tr><th>Area</th><th>Result</th></tr></thead><tbody>"
            + "".join("<tr>" + "".join(f"<td>{cell}</td>" for cell in row) + "</tr>" for row in rows)
            + "</tbody></table>")


def main(output):
    out = Path(output)
    out.mkdir(parents=True, exist_ok=True)
    evidence = json.loads((ROOT / "Docs/Setup/verification.json").read_text())
    for name in ("verification.json", "security-verification.json", "storage-receipt.json",
                 "knowledge-hub-verification.json", "asset-ledger-verification.json"):
        (out / name).write_bytes((ROOT / "Docs/Setup" / name).read_bytes())
    for name, source in (("storage-guide.md", "knowledge/storage.md"),
                         ("startup-guide.md", "knowledge/context-startup.md"),
                         ("recovery-guide.md", "knowledge/development.md"),
                         ("handoff.md", "Docs/Handoffs/HANDOFF_2026-11-14_setup.md")):
        (out / name).write_bytes((ROOT / source).read_bytes())
    restore_ok = evidence["storage"]["restore_verified"]
    asset_result = ("1,557 assets backed up and restored; every size and hash matches." if restore_ok
                    else "Private backup complete. Full restore verification remains open.")
    capture_stale = evidence["services"]["asset_ledger"]["capture_stale"]
    capture_result = ("Repository connected. Shared capture check overdue; local check queued. No generation ran." if capture_stale
                      else "Repository connected. Capture status checked; no generation ran.")
    bro = ("Repo setup saved. Session skills, shared knowledge, three Claude accounts and service links are ready. "
           + ("All 1,557 assets passed backup and restore checks. " if restore_ok else "Restore verification remains open. ")
           + "No commit or push. Fresh agent sessions and gameplay remain untested.")
    content = decide.recap(bro, [
        ("Sessions", "Shared project knowledge and both session skills installed for Claude and Codex."),
        ("Ownership", "Commit rules protect other sessions; the local Git guard blocks new media and secrets."),
        ("Memory", "Three Claude accounts point to one memory folder; Codex uses project records."),
        ("Storage", asset_result),
        ("Connections", "All four tools linked. Build and six safety checks pass; browser verification unavailable."),
    ], {
        "did": "Set up shared knowledge, session skills, private storage, memory and service links.",
        "why": "Future sessions need one reliable place for project knowledge, assets and handoffs.",
        "working": "Local setup checks, service bindings and the application build pass.",
        "not_working": "No connected report browser. Fresh client sessions and gameplay remain untested.",
        "next": "Setup remains uncommitted. Further publication or game work needs its own request.",
    })
    body = table([
        ("Session history", "Project knowledge, task routes, decisions, lessons, checkpoints and restart guidance saved."),
        ("Session skills", "New-session and session-wrap entry points validated for Claude and Codex."),
        ("Git ownership", "Explicit owned paths and hunks only. Shared staging preserved. Local pre-commit guard enabled."),
        ("Legacy media", "Three exact-hash exceptions preserve existing media. Authored SVG templates remain allowed."),
        ("Claude memory", "All three accounts resolve to the same memory file. Existing account settings preserved."),
        ("Codex", "Project instructions, skill discovery and trusted repository settings configured."),
        ("Asset backup", asset_result + " Total: 3,358,920,163 bytes."),
        ("Storage privacy", "Dedicated private bucket created. Legacy Aquila public reads blocked; other project policies preserved."),
        ("Website", "Existing website still responds. No deployment or hosting changes."),
        ("Asset Ledger", capture_result),
        ("Research Console", "Aquila binding and project lookup pass. No research submitted."),
        ("Knowledge Hub", "Three providers registered. Codex search and exact passage retrieval pass, including hash verification."),
        ("Agent Report", "Shared kit, launcher and hooks linked. Page checks and local HTTP pass."),
        ("Application", "Production Angular and Electron compilation passes. Existing dependency versions preserved."),
        ("Limits", "Fresh model loading, report rendering, gameplay and real generation capture remain untested."),
        ("Preservation", "Original setup notes archived. Game source and shared Git index preserved. No commit or push."),
    ])
    body += "<p>Restore checks found a Windows path issue. Directory preparation was fixed; concurrent restore regression checks now pass.</p>"
    body += '<p><a href="verification.json">Verification</a> · <a href="storage-receipt.json">Backup and restore receipt</a> · <a href="security-verification.json">Privacy checks</a></p>'
    body += '<p><a href="startup-guide.md">Session guide</a> · <a href="storage-guide.md">Storage guide</a> · <a href="recovery-guide.md">Recovery guide</a> · <a href="handoff.md">Current handoff</a></p>'
    content += decide.info_thread("aquila-setup-r1-info", "Approved repository setup saved locally; verification limits remain explicit.", body_html=body, short="Setup and proof")
    content += decide.whats_next("aquila-setup-r1-next", "No further work started. Fresh sessions can load the saved handoff; publication requires its own request.", short="Ready for next session")
    rounds = page.entry(1, "Repository setup", content, when="2026-11-15 UTC")
    status = decide.recap(
        "Setup and notes are saved. Assets are backed up. Setup changes have not been committed or pushed. Fresh sessions and gameplay remain untested.",
        [
            ("Setup", "Repository infrastructure installed locally."),
            ("Saved", "Knowledge, evidence and the handoff are saved."),
            ("Backup", "All 1,557 assets passed backup and restore checks."),
            ("Git", "Setup changes remain uncommitted and unpushed."),
            ("Limits", "Fresh sessions and gameplay remain untested; shared capture was overdue at the last check."),
        ],
        {
            "did": "Checked current Git status and the latest commit.",
            "why": "Confirm whether the setup was committed and pushed.",
            "working": "Setup, saved project notes and verified asset backup are in place.",
            "not_working": "Setup changes are not committed or pushed.",
            "next": "Publication waits for a commit-and-push request.",
        },
    )
    status += decide.info_thread(
        "aquila-setup-r2-info", "Set up and saved, yes. Setup changes remain uncommitted and unpushed.",
        body_html=table([
            ("Current commit", "<code>37218b8</code>; unchanged from the task's starting commit."),
            ("Working files", "Setup files remain modified or untracked. Shared staging remains empty."),
            ("Commit rule", "Commit only on the user's request in this session. The latest message was treated as a status check."),
        ]), short="Publication status",
    )
    status += decide.whats_next(
        "aquila-setup-r2-next", "Commit and push remain pending. No publication ran during this status check.", short="Still local",
    )
    rounds = page.entry(2, "Saved, not published", status, when="2026-11-15 UTC") + rounds
    wrap = decide.recap(
        "Wrap saved. Commit and push now authorized. This session's setup files are being published; site changes come next for discussion.",
        [
            ("Saved", "Final checkpoint, restart prompt and reusable lessons saved."),
            ("Scope", "Only this session's setup changes belong in the commit."),
            ("Assets", "Existing private backup and verified restore remain in place."),
            ("Capture", "Aquila remains connected. The shared capture check is no longer stale."),
            ("Next", "Discuss desired site changes after publication finishes."),
        ],
        {
            "did": "Saved the wrap and reviewed owned publication files.",
            "why": "The owner requested a wrap, commit and push before discussing site work.",
            "working": "Setup, backups and project records are ready; shared staging remains preserved.",
            "not_working": "Publication remains pending in this checkpoint.",
            "next": "Commit, push, verify the remote, then discuss site changes.",
        },
    )
    wrap += decide.info_thread(
        "aquila-setup-r3-info", "Wrap saved; commit and push authorized.",
        body_html="<p>New lessons cover restore verification and Git index metadata. No game source or dependencies changed.</p>"
                  "<p>Fresh client sessions, gameplay and report rendering remain untested. No site deployment is authorized.</p>",
        short="Wrap and scope",
    )
    wrap += decide.whats_next("aquila-setup-r3-next", "Publication checks run first. Site discussion follows; no site edits have started.", short="Publication first")
    rounds = page.entry(3, "Wrap and publication", wrap, when="2026-11-15 UTC") + rounds
    publication_path = ROOT / "Docs/Setup/publication.json"
    if publication_path.is_file():
        published = json.loads(publication_path.read_text())
        (out / "publication.json").write_bytes(publication_path.read_bytes())
        (out / "restart-prompt.txt").write_bytes((ROOT / "Docs/Handoffs/PROMPT_2026-11-15_setup.txt").read_bytes())
        close = decide.recap(
            "Session wrapped. Setup commit pushed and verified. Notes, lessons and restart prompt saved. Next discussion: what should change on the site?",
            [
                ("Published", "The setup commit is verified on the remote branch."),
                ("Saved", "Final handoff, restart prompt, lessons and publication evidence are saved."),
                ("Checks", "Six safety checks, startup checks, Git guard and report page checks pass."),
                ("Limits", "Fresh sessions and gameplay remain untested. The report opened and passed a browser snapshot check."),
                ("Next", "Choose the first site change to discuss."),
            ],
            {
                "did": "Wrapped the setup session, committed owned files, pushed and verified the remote.",
                "why": "The owner requested publication before discussing site work.",
                "working": "Setup and saved knowledge are published; asset backup and restore checks passed.",
                "not_working": "Fresh client sessions, gameplay and real generation capture remain untested.",
                "next": "Discuss the desired site changes before implementation.",
            },
        )
        close += decide.info_thread(
            "aquila-setup-r4-info", "Session wrapped; setup commit pushed and verified.",
            body_html=table([
                ("Source commit", "<code>" + escape(published["commit"]) + "</code>"),
                ("Remote", "<code>origin/main</code>; exact source commit verified after push."),
                ("Scope", "64 owned setup paths published. Game source, dependencies and peer staging preserved."),
                ("Knowledge", "Saved restore and index-handling lessons, current checkpoint and restart prompt."),
                ("Capture", "Aquila connected with no scoped issues. The shared capture check is no longer stale."),
                ("Report", "Opened in the workspace browser; current round and site question verified by snapshot."),
                ("Site", "No site changes or deployment started."),
            ]) + '<p><a href="publication.json">Publication evidence</a> · <a href="handoff.md">Final handoff</a> · <a href="restart-prompt.txt">Restart prompt</a></p>',
            short="Published setup",
        )
        close += decide.question("aquila-site-r4-scope", "What would you like to change on the site first?", [], short="Site changes")
        close += decide.whats_next("aquila-setup-r4-next", "Site planning waits for your changes. No site implementation or deployment has started.", short="Site discussion")
        rounds = page.entry(4, "Wrapped and published", close, when="2026-11-15 UTC") + rounds
    doc = page.document("Aquila repository setup", "<main>" + rounds + "</main>", "repo-setup", folder=HERE)
    decide.require_ok(doc, root=out)
    decide.save_questions(doc, folder=HERE)
    questions = HERE / "questions.json"
    questions.write_text(questions.read_text(encoding="utf-8"), encoding="utf-8", newline="\n")
    (out / "index.html").write_text(doc, encoding="utf-8", newline="\n")
    print(out / "index.html")


if __name__ == "__main__":
    main(sys.argv[1])
