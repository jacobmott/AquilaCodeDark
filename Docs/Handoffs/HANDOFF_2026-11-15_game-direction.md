# Aquila remake: wrapped for discussion

2026-11-15 UTC / local. Gameplay implementation paused at the owner's request.
The owner explicitly requested session wrap, commit and push, then discussion of new information they will provide.
Wrap saved; publication verification is pending. The final receipt will be `Docs/Testing/game-direction-publication.json`.

## Authority and stop boundary

- Commit and push only this continuing remake session's work. Preserve peers and the shared Git index.
  This authority is consumed by the wrap publication; it does not resume gameplay work.
- Wait for the owner's discussion topic. Do not select or implement another milestone during this pause.
- Website deployment, asset transfers, paid generation and research submission require separate explicit authority.
  Assets, secrets, generated media and scratch stay outside Git. Historical approvals grant no new authority.
- Owner accepts the preceding projectile encounter: "projectiles work well" and the current prototype works well.
  That playtest does not independently verify the later automatic helm.
- Full 3D flight above/below ships and follow-behind camera are settled. Original camera choice:
  `reports/game-direction/inbox/20261115T015927Z.json`.
- The website is a persistent Three.js universe with nearly invisible glass HTML controls; Robinhood was inspected read-only.
  Battleships remain in orbit; smaller shuttles descend. Separate interior rooms need not exactly fit the exterior hull.
- Owner accepts doubled battleship height, camera orbit, the bridge/passage/hangar loop and the walking-camera fix.
  Weapons acquire, aim and fire themselves. Capital navigation follows decisions; manual battleship controls remain hidden and gated.

## Current playable state

Angular 18 with Three.js/types 0.186.0; one renderer across Home, Fleet, Atlas, Play and Settings.
Bounded procedural sectors, relative rendering, larger compressed planets, shared terrain, quality budgets, local saves and controlled descent work locally.
Resolute measures 13,600 × 4,800 × 1,280 m. Peregrine, Osprey and Aegis are shuttle classes; names and finish remain reviewable.
Bridge, passage and hangar support first-person walking, live windows, stations and guided shuttle launch/return.
Walking uses WASD, Shift, mouse/arrows, F for stations and C outside; Escape releases and pauses.
Left/right exterior battleship drags orbit the camera and return behind on release. Shuttles retain manual flight.

Approach orbit/Atlas plots a course and holds at arrival. Hold position cancels movement.
Keep combat range seeks 35 km; Withdraw seeks 85 km and never approaches if already farther away. Tolerance is 1 km.
These are provisional practice distances. Orders continue while walking or changing stations/views.
Pause, settings, focus/visibility loss and graphics recovery freeze progress; restoration remains paused. Return to Play resumes.
Hangar launch stops the capital before transferring control. Reload restores position/orientation with no stale order.
Ship changes, reset and terminal combat outcomes cancel movement. New destinations end temporary practice.
Routes use six-second eased motion and swept planetary clearance. Heading changes are bounded to 0.45 radians/second.
This is kinematic navigation, not physical thrust, general tactical planning or projectile-aware evasion.

Two automatic turrets fire visible kinetic bursts with recoil, flashes and contact effects. Enemy rounds also travel before dealing damage.
Both sides have one shield; rounds can hit planets or miss moving ships. Defense resolves at swept contact.
Standard rounds drain Sentinel's shield but cannot penetrate armor; confirmed blockage stops ineffective new launches.
Piercing rounds penetrate. Hold fire stops new launches; flying rounds retain their original ammunition and trajectory.
Navigation never changes weapon modes or ammunition. Victory/defeat stops both weapons and movement.
Fixed 120 Hz simulation uses 160 projectile slots and 32 impact slots. Practice damage and unlimited ammunition are temporary.
One stationary target and coarse contacts; mount arcs and own-hull obstruction remain unrestricted.

Current details: [architecture](../../knowledge/architecture.md), [development](../../knowledge/development.md),
[combat design](../../knowledge/combat-design.md) and [game direction](../../knowledge/game-direction.md).
Missiles, countermeasures, automatic evasion, upgrades, crew, engineering, quarters, ground activities, economy, multiplayer and finished art remain future work.

## Verification and limits

Production Angular build and Electron TypeScript compilation passed: 2.98 MB raw bundle; existing Node 25.8.1 non-LTS warning.
Thirty-five model tests passed: universe 11, interior 6, combat 13, navigation 5.
Thirty-six native browser groups passed: navigation 6, combat 9, interior 7, camera 6, battleship 8.
Isolated installed Chrome 157.0.8103.78 on RTX 5090; successful runs have no application errors.
Glass order panels and narrow layout were visually inspected. Gameplay source has not changed during wrap.
Current proof: [automatic helm receipt](../Testing/automatic-helm-verification.json), with 26 source/test hashes and 27 media hashes.
Earlier receipts are historical; do not rewrite their hashes to match later source.
Six repository safety tests passed again during wrap; those fixtures do not perform real backup transfers.

First navigation attempt queried console controls while walking. One combat attempt was interrupted by my development reload.
Corrected, stable-source reruns passed. Later interior checks verified the final active-course launch-stop guard.
Scale/performance script was adapted but not rerun; earlier long terrain, quality and depth checks remain historical.
Physical devices, other engines, long play, Electron runtime, fresh agent sessions and actual provider capture remain untested.
Knowledge Hub OpenAI query "autopilot" returned three unrelated video-course records; none reused. No catalog-wide absence claim.

## Ownership, services and report

Session-owned scope: package changes, app wiring/styles, universe source, nine browser QA scripts, four model-test files,
knowledge/docs/checkpoints/history/verification and this task's report. No peer staging was present at the wrap inspection.
Publication must use a private index, explicit paths and the repository guard; reconcile only unchanged owned baseline entries.
Media and tooling remain ignored. No tests or transfers remain active.
Preview 4200/PID 37488; task report 8953/PID 69596; setup report 8952/PID 69828 remains untouched.
Report `reports/game-direction/` currently has round 29; wrap adds round 30. Earlier rounds and actual inbox are preserved.
Last workspace-browser check returned `browser_no_host`; HTTP and gameplay checks do not prove a report browser snapshot.
Last-read inbox: `reports/game-direction/inbox/20261115T015927Z.json`; no newer submission at wrap inspection.
After "submitted" or "I replied", read the newest real inbox first. No submission was fabricated.
Exact pre-wrap checkpoint: [archive](../History/game-direction/20261115T223241Z.md), [hash manifest](../History/game-direction/20261115T223241Z.json).
Copyable restart prompt: [saved prompt](PROMPT_2026-11-15_game-direction.txt).

Do not start any of this. Read, say where things stand, then wait.
