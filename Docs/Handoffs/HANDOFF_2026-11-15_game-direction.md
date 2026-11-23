# Aquila matches, Combat Simulator and Windows playtest

2026-11-22 local, round 118. The owner confirmed a separate Combat Simulator in chat, requested a planning commit and push, and asked what to work on next. Match and observer work was already on `origin/main` in `75939af`, with its round 117 records in `c07f284`. [Previous publication](../Testing/native-observer-publication.json).

## Combat Simulator

Alongside Play Match, enter a small arena directly. Try any ship and its compatible equipment, compare builds, see where gear is earned, or fight for fun. Developers can repeat a duel after a balance change without starting a universe. Use match combat code, equipment rules and damage settings. Keep arena state outside all match saves. [Agreed design and proposed first duel](../../knowledge/game-direction.md#combat-simulator).

Recommended next build on Windows is Resolute against Sentinel with standard and piercing ammunition. Reset the same duel, compare shields and hull, then leave and resume an unchanged test match. Existing practice shares expedition state and has different damage scaling. Round 118 reran the eleven existing native test groups with a separate scratch save path. All passed. This checks the existing mechanics. Direct arena launch, matching damage settings and save isolation still need implementation and their own tests. Local result: `.runtime/combat-simulator-r118/tests/index.json`.

## Playable on Windows

Use Epic's launcher Unreal 5.8.4 CL 58902052 at `C:/Program Files/Epic Games/UE_5.8`. The game opens near Resolute with the UI hidden. WASD pans, R/F changes height and Shift speeds movement. Drag either mouse button to rotate, middle-drag to pan, and use the wheel to zoom. Click a visible ship to follow it. Pan or click empty space to detach. Home finds Resolute, C visits its bridge, and H toggles the command panel. Two fingers reveal touch controls. Purple impact discs mark shield, hull or armor contact. [Play controls](../../native/Aquila/README.md#play) and [camera decision](../../knowledge/interface-direction.md#native-observer-decision-2026-11-22).

At wrap the seven native source hashes and the Windows executable matched the [observer evidence](../Testing/native-rts-observer.json). Six packaged input runs and eleven simulation/save groups had passed. Six local captures and the Android arm64 build log also checked out. Earlier owner confirmation covers exterior mouse orbit. Owner comfort with the new view, desktop sizing, a full human expedition and physical Galaxy inputs remain open. Windows is primary. Galaxy checks happen at milestones. Sustained phone performance, cloud saves and Apple also remain open.

The packaged game was responsive and autosaving in `.runtime/expedition-windows-r101/20261119T020630Z/Campaign`. It restored revision 151102 on launch. A read-only wrap check at 01:36 UTC on November 23 verified revision 151499 and its parent. The first expedition was in **Failed** with the helm on **Hold**. The game remained open, so check the current revision before acting. The earlier held E input caused a defeat, retry and excessive saves. Launch owner sessions without injected input. Keep the full journal. [Incident](../Testing/native-command-panel.json) and [restart evidence](../Testing/native-windows-restart.json).

## Match and save direction

Each run is one standalone, real-time match. The player begins with a homeworld and Resolute against one expanding computer rival. Choose where to explore, settle, build or fight. Ships and worlds keep working while attention moves elsewhere. Either side can lose ships and worlds. Settling more worlds makes a civilization harder to eliminate. The precise defeat rule remains open.

Ships can be replaced, including Resolute. Supplies and upgrades extend the fleet's reach. Terraforming and new civilizations can make once-empty regions worth revisiting. Local supplies, technology and ships are possible rewards. New Run starts from scratch. Save points belong to their run and restore both sides. A different generated universe per run is proposed. Windows currently restores one expedition with a fixed default seed. Fleet construction, strategic opponents, world development and run selection remain design work. [Match and run notes](../../knowledge/game-direction.md#standalone-matches), [decisions](../DECISIONS.md) and [backlog](../BACKLOG.md).

## Paused publishing inquiry

Texas sole owner, US-only planning. Keep the home address private. The owner chose an LLC with a dedicated business address and resolved the employment/IP question. Favor dependable handling of important letters over unlimited scans. Quality and stability come before price. Northwest is the leading candidate. Regus Plaza 7000 is a conditional fallback. No provider, package or filing date was chosen. [Publishing decisions and app examples](../../knowledge/publishing-business.md).

Four developers display the same Austin suite on Google Play. Zensoul names Northwest in Wyoming. The examples do not identify the plans or private verification documents. The [focused research](../Research/texas-business-address-reliability-review-v0001.json) was reviewed once. Actual mail performance and this LLC’s address eligibility remain open.

D&B’s chat agent could not send the proposed referral. There was no live representative, case number, appointment or reliable staffing schedule. The owner deferred the retry. On a new request, use the [saved D&B message](PROMPT_2026-11-18_dnb-follow-up.txt) to ask a representative whether a home-based LLC can claim this virtual office as its primary address and which documents qualify. The [redacted chat receipt](../Research/dnb-google-play-address-chat-v1.json) points to the local exchange. Raw research, chat and personal contact details stay outside Git.

## Session state

The [round 116 checkpoint](../History/game-direction/20261123T013848Z.md) and [hash manifest](../History/game-direction/20261123T013848Z.json) preserve the exact prior record. The [earlier publication receipt](../Testing/native-playtest-publication.json) covers the previous Windows controls and paused inquiry. [Project lessons](../../LESSONS.md) cover held input, framing restored ships and keeping run histories separate. The Windows package, game journal and media remain local.

The [round 117 handoff](../History/game-direction/20261123T021928Z.md) and [hash record](../History/game-direction/20261123T021928Z.json) preserve the prior checkpoint. At that wrap, the Windows game, report 8953 and Research Console 8776 were running. Report 8952 and preview 4200 were inactive. In round 118 a read-only check of the running game's save found revision 151583, Failed with the helm on Hold. It can advance on its own. Check again before acting. Keep the journal and launch owner sessions without injected input.

Last real inbox: `reports/game-direction/inbox/20261115T015927Z.json`. Later decisions came through chat. The current push request covers these game planning records. D&B stays paused. Contacts, purchases, filing, signup, generation, other publication, asset transfer and deployment require fresh approval.

## Restart prompt

```text
Aquila Code Dark at D:/OtherProjects/AquilaCodeDark. Continue the game direction task.
Read AGENTS.md, Docs/TaskRoutes.md and Docs/Handoffs/HANDOFF_2026-11-15_game-direction.md in full.
Report http://127.0.0.1:8953/?agentreport=1&paseoAgent=09c42e01-b201-4b15-8ad9-8a9f4ed3c740 (reports/game-direction/, round 118).
Last real inbox read: reports/game-direction/inbox/20261115T015927Z.json. Later decisions came through chat.
1. Match design: one homeworld, Resolute and one expanding computer rival. New runs start fresh. Save points belong to their run. Different universes per run are proposed.
Combat Simulator is a separate arena for trying all compatible builds and testing the same combat rules without loading a universe. Read knowledge/game-direction.md#combat-simulator. The first recommended Windows duel uses Resolute, Sentinel and two ammo types. The standalone mode remains to build.
2. Windows playtest: launcher Unreal 5.8.4. Free RTS observer and hidden controls work. Six packaged input checks and eleven simulation/save groups passed. Camera feel, sizing and human pacing remain open.
Galaxy checks at milestones. Sustained mobile performance, cloud saves and Apple remain open.
The running expedition recorded defeat with the helm on Hold. Check the current save before acting. Preserve the journal and launch without injected input.
The Windows game was still running in round 118. Service states from the previous wrap are recorded above.
3. D&B waits for owner direction. Read knowledge/publishing-business.md and Docs/Handoffs/PROMPT_2026-11-18_dnb-follow-up.txt before reopening. Keep the home address private.
Preserve the positive app examples and their limits. No specialist referral or provider selection.
The planning push is one-time. New contact, purchase, filing, signup, generation, further publication, transfer or deployment needs fresh approval.
Do not start any of this. Read, say where things stand, then wait.
```
