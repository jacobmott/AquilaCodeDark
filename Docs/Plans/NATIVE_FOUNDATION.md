# Native foundation milestone

2026-11-15 UTC. Owner authorized starting this milestone; local foundation implementation underway.

## Purpose and authority

Prove that Aquila's existing gameplay direction works as a native Unreal game across desktop and phones,
with the same single-player campaign continuing between them.
Owner now says, "Let's do that," and specifies launcher Unreal 5.8.4, independent from DuskForge's source engine.
This authorizes the local native foundation and readiness checks. No paid service, store submission, deployment, commit or push is authorized.
Accepted requirements and source evidence: [native release direction](../../knowledge/native-release-direction.md).
Current runtime and verification: [checkpoint](../Handoffs/HANDOFF_2026-11-15_game-direction.md).

## Product requirements carried forward

- Coordinated Steam, iPhone/iPad and Android release is the target; no date or minimum device is promised.
- Single-player only. Battles, navigation and world simulation require no multiplayer networking.
- A shared game profile and campaign save work across platforms through login or linking.
- Local saving supports offline play, with synchronization when connected and protection against silent loss during conflicts.
- Battleship navigation and weapons remain automatic. Players choose orders and modes.
- Visible traveling projectiles, deliberate combat, shield impacts and weapon/defense compatibility retain the agreed design.
- The public website has no playable game. It retains space/glass styling, Three.js presentation, gameplay backgrounds, information and store links.

## Approved sequence and available targets

Project location: `native/Aquila/` in this repository. Source and configuration stay in Git; generated Unreal content and builds stay ignored.
Launcher installation verified: `C:/Program Files/Epic Games/UE_5.8`, version 5.8.4, changelist 58902052.
The wrapper `scripts/native.ps1` checks the installed-build marker, exact version and launcher registration before invoking it.
DuskForge remains separate, using its existing source-engine association. No DuskForge files, engine files or shared settings are changed.
Owner described a Samsung Galaxy S26 Plus. Connected hardware reports SM-S948U, Android 17/API 37 and arm64-v8a.
Use the measured model identifier for test records; sustained performance remains unmeasured.
No Mac or iPhone is available. iOS remains a release target but its build and device proof are deferred, not assumed passed.
Android target support was added through Epic Launcher after the initial inventory.
Matching SDK/NDK and platform tools are installed under this repository's ignored runtime directory; build settings are process-scoped.
USB connection, authorization, installation and initial physical-device rendering/input/save checks pass. Sustained checks remain required.

Initial engineering targets, not measured results or owner-approved minimum specifications:
Windows 1080p at 60 fps; Galaxy landscape at sustained 30 fps minimum, with 60 fps optional after measurement.
Check a 20-minute battle/bridge session for thermal slowdown, frame pacing and memory growth before setting final device budgets.
No Nanite, Lumen, virtual shadow maps or ray tracing in the shared baseline.

1. **Build-readiness review.** Inventory the available Unreal installation, target devices and Mac/Xcode access.
   Confirm Windows, Android and iOS packaging paths. Choose actual test devices and performance budgets before claiming feasibility.
   Record missing access or equipment; do not install tools or buy services merely to fill an unknown.
2. **Small native playable.** One flagship, one planet backdrop, one bridge room, one enemy and two automatic projectile turrets.
   Include a movement order, camera controls, readable shield/armor outcomes and a usable touch interface.
   Keep the browser prototype as reference. This is not the full universe, crew system or first commercial release.
3. **Portable local saves.** Restore position, ship configuration, encounter/progression state and the procedural world identity.
   Define a versioned format and migration policy; keep device graphics and controls separate.
   Start with local persistence before integrating a hosted service, while preserving the cross-device contract.
4. **Shared identity and synchronization.** Evaluate and choose one suitable account/storage service before provisioning.
   Link verified platform identities to one profile. Add visible sync status, recoverable history and conflict handling.
   Never merge identities merely because two accounts report the same email address; linking must prove account control.
5. **Real device round trip.** Continue the same save on desktop, iPhone and Android, then return to desktop.
   Exercise interrupted connections, app suspension, offline divergence and compatible game updates.

## Evidence needed to finish the milestone

| Check | Required proof |
| --- | --- |
| Builds | Installable native builds run on the selected desktop, iPhone and Android devices; editor preview alone is insufficient |
| Controls and presentation | Player can issue movement and weapon modes, use the camera and enter the small interior on each device |
| Combat consistency | Projectile travel and defense outcomes remain consistent across quality levels; reduced effects do not alter damage |
| Local saves | Relaunch restores the intended campaign state; offline progress survives application suspension or interruption |
| Shared progress | Linked devices receive the acknowledged saved revision; ship state, world identity and progress agree |
| Conflicts | Two divergent offline saves remain recoverable; neither silently destroys the other |
| Updates | A supported save migration works, and unsupported versions are refused without damaging the saved campaign |
| Performance | Sustained physical-device checks meet the previously selected frame-time and memory budgets; heat and battery behavior are recorded |

Track measured native results separately in the native verification receipt. Browser receipts prove only the browser prototype.
Full phone coverage, store acceptance and long-term service reliability require further work beyond this first milestone.

## Boundaries and later work

Retain the existing project and published browser checkpoint. Do not remove the playable site during native feasibility work.
Choose native project/repository placement deliberately before creating files; keep generated assets and secrets outside Git.
The website conversion follows stable game identity and usable native gameplay footage. Its deployment needs separate explicit authority.
Full exploration, crew, expanded interiors, surface activities, economy and long-term progression remain later scope discussions.
Purchases and store ownership do not automatically follow campaign saves; commercial rules remain undecided.

Current state: Windows/Android small playable and portable local saves work; larger phone menus are owner-accepted.
Shared identity/sync, sustained performance and Apple build/device proof remain unfinished. First-expedition work resumed November 17.

## Proposed next step: first expedition

November 16, 2026, round 62. Owner asks what comes next after saving the business discussion.
Recommendation only; the request does not resume implementation or approve a revised milestone order.
Build one compact expedition, targeting roughly 10–15 minutes of play, using the existing battleship, bridge and command controls.

1. Choose a nearby signal, order automatic travel and survey it on arrival.
2. Resolve one hostile encounter through automatic weapons, positioning orders and ammunition/mode choices.
3. Recover salvage, then spend it on one useful upgrade to a starting turret, with a clear stat change.
4. Save and return later; mission progress, earned salvage and the upgrade persist into the next encounter.

Acceptance proposal: the owner can complete the loop on Windows and Galaxy; rewards cannot duplicate through reload;
an upgrade changes the next fight; older saves remain recoverable. Combat should clearly show rounds, shield hits and armor outcomes.
Follow with the already planned 20-minute device session to measure frame pacing, heat and memory under repeated play.
Performance figures above remain provisional targets, not measured results or shipping commitments.

This proposes gameplay and sustained performance before hosted account/provider selection, which previously came next.
Shared saves remain a requirement; company formation is not a technical prerequisite for planning or prototyping them.
The original cross-device foundation milestone stays incomplete until its remaining evidence gates pass.
Planet descent, crew, an infinite native universe, final asset production and the marketing-site conversion remain later work.
No store account, service purchase, generation, business setup, commit, push or deployment is included in this proposal.

November 17, 2026, round 73: reaffirmed this proposal after Northwest's inquiry submission was confirmed.
Its reply can proceed independently. The owner asks what comes next; implementation remains paused until direction to resume.
No change to the proposed scope, acceptance checks or unfinished cross-device requirements above.

November 17, 2026, round 74: owner says, "yes, let's resume. Let's do that" and accepts this expedition and sequence.
Owner questions using the phone as the primary development environment. Windows is the daily build, play and debugging target.
Android builds and touch-layout previews can run without a connected phone. Physical Galaxy checks are batched at playable milestones,
after relevant mobile changes or for phone-specific bugs. No continuous USB connection is required.
Proceed with local implementation and Windows validation now. Keep phone performance and human playtest evidence pending until a later device session.

Round 74 implementation: one repeatable signal/survey/battle/recovery loop; 120 salvage reward and a 100-salvage port-turret refit (+50% impact).
Schema 3 preserves mission progress, rewards, upgrades and shot damage, with migration from earlier saves.
Eleven simulation/save groups pass; editor, Windows and Android packages pass. Packaged Windows captures and upgraded-journal relaunch inspected.
Baseline automated loop takes 547.35 simulated seconds; the 10–15-minute player-time target awaits human playtesting.
New expedition mouse/touch input, physical Galaxy play and sustained performance remain unverified.
[Current expedition evidence](../Testing/native-expedition-verification.json) separates these results from the earlier foundation/phone receipts.

Round 75: owner explicitly agrees to Windows-first development and occasional Galaxy installs/tests, and asks to save and proceed.
This is the accepted workflow: daily Windows work, batched phone checks at milestones or for relevant mobile changes/bugs.
Continue the existing expedition validation. No phone connection is needed for routine work; no new runtime proof added by this confirmation.
