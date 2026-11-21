# Aquila native expedition and publishing

2026-11-20, round 104. The Windows camera fix, desktop controls and paused D&B inquiry reached `origin/main` in `80145671829e4bee53d6f0e107cda31f9f66da8d`. The [publication record](../Testing/native-playtest-publication.json) identifies the exact content and checks. This checkpoint and the [restart prompt](PROMPT_2026-11-20_native-playtest.txt) follow in the records commit.

## Windows playtest

The owner confirmed the exterior orbit with a physical mouse. The game closed normally on revision 37 of the isolated first expedition. The ship is on Hold during travel to the signal. On owner direction, open the separate campaign and choose **Resume course**. The [acceptance record](../Testing/native-windows-orbit-acceptance.json) identifies the journal, its hash and the owner's exact answer. The saved campaign lives locally at `.runtime/expedition-windows-r101/20261119T020630Z/Campaign`.

Left-click commands. Hold either mouse button over the ship or open space, then drag to look around. After release, the exterior camera recenters after 0.65 seconds. The bridge keeps its heading. At 1280×720, the desktop command strip narrowed from 1,204 to 854 pixels. A Windows touch preview kept the same large button bounds. [Desktop controls and build evidence](../Testing/native-windows-desktop-controls.json).

The Development Windows package and 11 simulation/save groups passed. Automated mouse checks passed in the exterior and bridge views. This wrap verified the five changed source files, the packaged binary, saved test outputs and journal against their recorded hashes where available. Desktop size approval and full human expedition pacing remain open. Earlier Galaxy foundation menus were owner-accepted. Sustained phone performance, linked cloud saves and Apple builds remain untested. [Earlier expedition verification](../Testing/native-expedition-verification.json) and [Galaxy acceptance evidence](../Testing/native-mobile-ui-verification.json).

Use Epic's launcher Unreal 5.8.4 CL 58902052 at `C:/Program Files/Epic Games/UE_5.8`. DuskForge has a separate engine. Windows stays primary. Check Galaxy at milestones or after mobile changes. Single-player expedition, automatic helm and weapons, and offline revisioned saves remain the direction. Steam, Android and Apple share the release target. The browser game remains a reference for the future public site. [Native guide](../../native/Aquila/README.md) and [foundation plan](../Plans/NATIVE_FOUNDATION.md).

## Paused publishing inquiry

Texas sole owner, US-only planning. Keep the home address private. The owner chose an LLC with a dedicated business address and resolved the employment/IP question. Favor dependable handling of important letters over unlimited scans. Quality and stability come before price. Northwest is the leading candidate. Regus Plaza 7000 is a conditional fallback. No provider, package or filing date was chosen. [Publishing decisions and app examples](../../knowledge/publishing-business.md).

Four developers display the same Austin suite on Google Play. Zensoul names Northwest in Wyoming. The examples do not identify the plans or private verification documents. The [focused research](../Research/texas-business-address-reliability-review-v0001.json) was reviewed once. Actual mail performance and this LLC’s address eligibility remain open.

D&B’s chat agent could not send the proposed referral. There was no live representative, case number, appointment or reliable staffing schedule. The owner deferred the retry. On a new request, use the [saved D&B message](PROMPT_2026-11-18_dnb-follow-up.txt) to ask a representative whether a home-based LLC can claim this virtual office as its primary address and which documents qualify. The [redacted chat receipt](../Research/dnb-google-play-address-chat-v1.json) points to the local exchange. Raw research, chat and personal contact details stay outside Git.

## Handoff

The content commit included 41 reviewed paths. The shared index had no staged changes afterward. The previous [round 103 checkpoint](../History/game-direction/20261121T034508Z.md) and [hash manifest](../History/game-direction/20261121T034508Z.json) preserve its exact bytes, earlier history and approval boundaries.

The local report runs on 8953. Research Console runs on 8776. Report 8952, preview 4200 and the game were inactive at the wrap. No game/build job, provider follow-up or asset transfer was running for this session. The only real inbox item remains `reports/game-direction/inbox/20261115T015927Z.json`. Physical orbit confirmation came through chat.

Wait for the owner to resume the Windows playtest or reopen D&B. This wrap used the one-time commit and push authorization. Provider contact, purchases, filing, store signup, generation, further publication, asset transfer and website deployment require new direction. Preserve peer files and shared staging.

## Restart prompt

```text
Repo D:/OtherProjects/AquilaCodeDark. Windows expedition and paused D&B address inquiry.
Read AGENTS.md, Docs/TaskRoutes.md and Docs/Handoffs/HANDOFF_2026-11-15_game-direction.md in full.
Publication verified on origin/main. See Docs/Testing/native-playtest-publication.json.
Report http://127.0.0.1:8953/?agentreport=1&paseoAgent=1797de6d-c62b-478b-b154-82859c3ad278 (reports/game-direction/, round 104).
Last real inbox read: reports/game-direction/inbox/20261115T015927Z.json.
Use launcher Unreal 5.8.4. Windows first. Galaxy checks at milestones.
Owner confirmed exterior mouse orbit. Windows package, 11 simulation/save groups and both automated camera checks passed.
Desktop sizing, full human expedition pacing, sustained phone performance, cloud saves and Apple remain open.
Separate save at revision 37, first signal, helm on Hold. On owner direction, choose Resume course.
D&B follow-up waits for the owner. Read knowledge/publishing-business.md and Docs/Handoffs/PROMPT_2026-11-18_dnb-follow-up.txt before reopening.
Keep the home address private. Preserve positive app examples and their limits. No specialist referral or provider selection.
Report 8953 and Research Console 8776 running. Report 8952, preview 4200 and game inactive. No session job or transfer.
Wrap authority consumed. New contact, purchase, filing, signup, generation, publication, transfer or deployment needs fresh approval.
Do not start any of this. Read, say where things stand, then wait.
```
