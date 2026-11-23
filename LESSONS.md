# Lessons

## 2026-11-18 [M] D&B chat could collect details without forwarding a request

D&B’s website agent acknowledged the contact details, then said it could not forward an email or trigger a reply.
The chat produced no case number or human answer. A live representative can join that text chat when available, according to the agent.
Use the [saved question and chat receipt](Docs/Research/dnb-google-play-address-chat-v1.json) when the owner reopens the topic.

## 2026-11-17 [R] Check whose contract covers a mailbox

Direct Regus terms offer another center after a permanent closure. Davinci advertises that center under its own agreement.
Tie price, notice, inspection rights and mail handling to the same seller and plan.
For ordinary USPS mail, check the mailbox agreement's post-termination instructions. Legal notices follow separate arrangements.
See the [review and primary sources](knowledge/publishing-business.md#reliability-review-round-88).

## 2026-11-17 [J]: Provider quality precedes price

Owner prefers robust, stable, proven service even at a higher cost. The earlier cost-led comparison did not establish a reliability leader.
In round 90 the owner confirmed the order for address plans. Protect home-address privacy first, then favor dependable handling of important letters over unlimited scans.
Keep observed public-address outcomes separate from mail/service reliability; retain both useful evidence and its limits.
Compare support and address continuity before price, without treating higher expense as proof of quality.
Evidence: [current publishing priorities](knowledge/publishing-business.md#quality-before-price-round-84).

## 2026-11-17 [J]: Use collegial wording for outside inquiries

Owner prefers friendly questions such as "Could you please..." over directives such as "Please provide..." in provider emails.
Keep the needed details, use plain language, and show appreciation without sounding demanding.
Scope: correspondence tone, not new authority to send messages or purchase services.

## 2026-11-16 [J]: Reuse the saved publishing discussion

Owner asks to save the business/privacy discussion so later sessions do not repeat it.
Start with [the publishing reference](knowledge/publishing-business.md), then follow only relevant evidence links.
Keep confirmed owner facts, tentative preferences, researched rules and unresolved checks distinct.
Refresh changing requirements before action; do not restart broad research or ask settled questions merely because the session changed.

## 2026-11-14 [M]: Check real storage permissions
The legacy shared-bucket policy allowed anonymous reads of the entire Aquila backup prefix, including its Secrets folder.
Folder naming and a private Git remote do not establish S3 privacy. Verify bucket policy, public-access blocks and anonymous access.
Evidence and remediation: `knowledge/storage.md` and the setup receipt.

## 2026-11-14 [M]: Preserve client-specific loading
Claude imports the shared core through `CLAUDE.md`; Codex discovers repo skills through `.agents/skills/`.
Three Claude accounts need links to the same project-memory folder. Configured files do not prove fresh model loading.

## 2026-11-14 [M]: A text extension can contain media
An existing SVG template and HTML snippet contain embedded raster images. Check content as well as extensions.
Keep grandfathered exceptions hash-pinned; do not ban authored SVG templates or silently expand exceptions.

## 2026-11-15 [M]: Verify an actual isolated restore
The first concurrent restore stopped during missing-parent path resolution on Windows; the upload had passed.
Preparing directories serially before parallel downloads passed the nested-directory regression and all 1,557 real asset hashes.
Evidence: `Docs/Setup/storage-receipt.json` and `scripts/tests/test_repo_safety.py`.
Treat upload receipts as upload proof; verify a separate restore before claiming recoverability.

## 2026-11-15 [M]: Separate index bytes from staged content
The Git index byte hash changed after checks while staged content still exactly matched the starting commit.
A byte-hash mismatch alone does not prove a peer staged changes. Inspect exact mode/blob/stage entries before any reconciliation.
Evidence: `Docs/Setup/verification.json`. Preserve unknown changes and use an index lock for authorized owned-entry reconciliation.

## 2026-11-15 [M]: Build success does not prove scene startup
The production build passed while the development browser caught an initial change-detection error after scene creation.
Explicitly finish initial view synchronization before Angular's verification pass. Test actual navigation and input around the shared renderer.
Evidence: `scripts/qa-universe.mjs` and `Docs/Testing/universe-verification.json`. Final checks show no page or console errors.

## 2026-11-15 [M]: Keep glass and pointer rules shared
Mobile navigation and pause/help overrides can silently restore opaque fills despite a transparent shared glass class.
Use one glass fill across those surfaces, with text shadows for changing scene brightness.
Cancel context menus on the flight canvas only, and verify both canvas cancellation and ordinary HTML menu behavior.
Wait for route content and entrance animations before assertions or visual captures; immediate counts can race Angular rendering.

## 2026-11-15 [M]: Exercise the control, not only its value
The original browser regression changed quality with direct select-value assignment; it never exercised opening the native popup.
Use actual pointer, keyboard and touch actions for interactive controls. Quality now uses direct radio choices.
Three.js `setPixelRatio()` already resizes; following it with `setSize()` repeated the buffer reset on every preference update.
One conditional drawing-buffer update avoids that work. Evidence: `Docs/Testing/quality-control-verification.json`.

## 2026-11-15 [M]: Validate depth modes against the pinned renderer
Three.js 0.186.0 can enable reversed and logarithmic depth together, despite newer documentation describing automatic precedence.
Select them exclusively. Custom shader materials need common and logarithmic-depth chunks for the fallback path.
Actual approach/descent checks cover both modes; compilation alone did not catch incorrect occlusion.
Evidence: `Docs/Testing/planet-scale-verification.json`.

## 2026-11-15 [M]: Profile first use as well as warmed travel
Adding scene fog on first descent compiled new ship shaders and produced a 637.4 ms desktop frame.
Terrain and sky already provide haze; removing redundant scene fog reduced the final descent maximum to 12.8 ms.
The constrained first approach still peaked at 158.5 ms, versus 20.9 ms when warmed. Keep both measurements and their hardware limits.
Bounded terrain buffers and short travel checks are not proof of total GPU memory use or long-session leak freedom.

## 2026-11-15 [M]: Apply capital scale to every clearance path
Enlarging only the visible hull leaves planet collision, assisted routes and descent eligibility using small-craft distances.
Use the same conservative whole-hull envelope for all three; rotation at the boundary must remain safe.
Check actual mesh bounds against declared dimensions and test both boosted impacts and departure paths.
Evidence: `Docs/Testing/battleship-verification.json` and the capital-envelope model tests.

## 2026-11-15 [M]: Test mouse-button chords and camera return paths
Chrome emitted a button-state pointer move, then dropped capture when one mouse button released while the other remained held.
Preserve the remaining drag only for that known chord transition; clear other capture losses and all input on pause/blur.
Recenter orbit angles at a fixed radius. Linear camera-position interpolation from the bow cuts through the ship.
Native input checks cover both button orders and return-radius bounds in `scripts/qa-orbit-camera.mjs`.

## 2026-11-15 [M]: Order pointer capture before mouse lock
Chrome granted a mouse-lock request during pointer-down and rejected the subsequent pointer-capture call with InvalidStateError.
Capture first, then request lock. Skip capture for clicks while already locked, and release late grants after mode or route changes.
Native click, mouse movement, repeated click and Escape checks pass in `scripts/qa-interior.mjs`.
Keep drag and keyboard alternatives; mouse-lock support and permission can differ across browser hosts.

## 2026-11-15 [M]: Reject mouse spikes before updating camera angles
One injected 700-pixel vertical movement drove the walking camera from level to its 1.3-radian pitch limit in a single event.
Discard excessive individual-axis samples instead of clamping them into a smaller visible snap; preserve valid movement on the other axis.
Reset absolute drag origins across capture transitions, and keep supported fallbacks when requesting unadjusted input.
Native input and injected-fault tests are distinct evidence. Neither proves the exact event that occurred on the owner's mouse.
Evidence: `Docs/Testing/walk-camera-verification.json`.

## 2026-11-15 [M]: Projectile hits need arrival-time defense checks

Traveling visuals alone do not create evasion. Resolve swept contact against current bodies and planets, then inspect the defense layer.
Shield overflow must check armor; failed penetration must not become minimum hull damage. Existing rounds retain their launch ammunition.
Keep simulation cadence independent of visual quality; graphics changes must not erase flying rounds or alter their damage.
Evidence: `scripts/tests/combat-model.test.mjs` and `Docs/Testing/projectile-combat-verification.json`.

## 2026-11-15 [M]: Separate helm orders from personal input and pause

Hiding buttons alone does not remove piloting: keyboard and canvas handlers must also respect the active craft and control context.
Walking or changing room views must not cancel the battleship's order. Freeze route progress on pause; cancel only for explicit order/lifecycle changes.
Before a hangar launch, stop capital navigation before transferring the player's position to the shuttle.
Native checks cover both continued navigation while walking and the moving-ship launch boundary.
Evidence: `scripts/qa-navigation.mjs`, `scripts/qa-interior.mjs` and `Docs/Testing/automatic-helm-verification.json`.

## 2026-11-15 [M]: Pin installed Unreal and regenerate authored content safely

Launcher 5.8.4 requires build settings V7 for its shared editor products. V6 fails compatibility checks; do not force overrides.
Check the launcher registration, installed-build marker and exact Build.version before building a separate native project.
An existing generated material package must be fully loaded before resaving. Prepare initially failed until the commandlet did that.
Instanced meshes require their material usage flag before cooking; a successful C++ compile alone did not catch missing visual materials.
Evidence: native build/content commands and `Docs/Testing/native-foundation-verification.json`.

## 2026-11-16 [M]: Native captures catch coordinate and startup-order errors

Unreal's entry map offset the pawn at spawn. A world-space camera viewed pawn-local bridge geometry from below the floor.
Keep the camera and scene in the same coordinate frame; inspect packaged interior output, not only editor compilation.
Pawn startup can precede the game mode's persistence setup. Refuse early saves until the intended save directory exists in memory.
The corrected packaged runs preserve the isolated directory, restore its revision and retain the earlier file.
Evidence: `Docs/Testing/native-foundation-verification.json`.

## 2026-11-16 [M]: SDK verification needs its report, not only its exit code

Unreal Turnkey returned exit code 0 while reporting Android SDK status Invalid and missing Android Studio.
After installing Aquila's separate command-line SDK, it recognized NDK r27c but retained its Studio-specific host prerequisite failure.
Read the detailed status and verify the actual command-line build separately. Do not treat Turnkey's overall exit code as readiness.
Android File Server's default editor module also writes an authentication token into project configuration; disable unused server support before publication.
Keep SDK tools, caches, signing material and generated configuration private and outside Git.

## 2026-11-16 [M]: Android archive success does not prove cooked content is inside the APK

BuildCookRun with build, cook, stage, pak and archive succeeded but copied the earlier uncooked APK when package was omitted.
Inspect the final APK for its embedded content and verify its signature; compilation, archive exit status and an APK filename are insufficient.
The wrapper now requests the package phase explicitly. Device installation and launch remain separate evidence.

## 2026-11-16 [M]: Earlier projectiles cannot invalidate newly selected ammunition

Physical Galaxy testing switched to piercing rounds while standard rounds remained in flight.
An old standard impact set a global armor-block flag, stopping the new loadout and showing the wrong refit instruction.
Keep the old projectile's blocked impact and zero armor damage, but base firing eligibility on the currently fitted ammunition.
Handle stale flags in prior saves without erasing progress. A new native regression covers late impacts and restored piercing loadouts.
Six automation groups pass; physical input injection remains separate from human touch usability and sustained performance.

## 2026-11-16 [J]: Successful touch injection does not establish handset readability

Owner's first phone playtest found menu text small and hard to see despite successful command injection.
Use a separate phone layout with larger type and touch areas, fewer simultaneous controls and clear text over glass.
Verify the rendered device output and ask the owner about reading comfort; desktop previews cannot settle handset readability.

## 2026-11-16 [M]: Camera exclusion checks need an active-camera control

Paused gameplay disables camera rotation, so a steady paused view cannot prove that UI bounds exclude camera input.
Resume gameplay, compare a swipe over the visible menu with the same area after hiding it, and inspect scene movement.
Small temporal pixel differences are not camera movement. Avoid simultaneous owner interaction during injected input checks.

## 2026-11-16 [J]: Publishing research must include practical developer experience

Owner asked for guides from solo developers after repeated policy summaries left address privacy unresolved.
Find first-hand accounts with concrete setup steps and reported outcomes, then check their dates, jurisdiction and current platform rules.
Separate personal accounts, sole proprietorships and incorporated organizations; do not turn one country's success into a universal guarantee.
Do not stop at generic uncertainty when documented working routes can guide a practical plan.
Sources and applicability limits: [publishing guides](knowledge/native-release-direction.md#practical-developer-publishing-guides-round-49).

## 2026-11-17 [J]: Windows is Aquila's daily development target

Owner explicitly confirmed this workflow in round 75 and asked to save it. Use Windows for coding, debugging and normal gameplay checks.
Batch phone checks at playable milestones, after relevant mobile changes or for phone-specific defects.
Android compilation and desktop touch previews need no phone; retain separate physical-device and sustained-performance evidence gaps.

## 2026-11-17 [M]: Persist projectile damage at launch and rewards with their claim state

The expedition upgrade changes one turret's impacts without changing rounds already flying.
Store each projectile's launch damage scale alongside its clocks and trajectory; old saves default to their original damage.
Save reward plus completion, and upgrade plus debit, in the same revision. Reload tests must check balances and repeated attempts.
The expedition tests cover all mission phases, 30/60 Hz parity, practice isolation and duplicate rewards/refits.
Evidence: [expedition verification](Docs/Testing/native-expedition-verification.json).

## 2026-11-18 [M]: Test desktop look with relative motion at a stationary cursor

A packaged Slate hit test found the camera under open space and command buttons above it.
In both camera views, one 240-count movement and eight opposing 30-count movements canceled at the same cursor position.
This catches dropped fast movements and window-edge dependence. Physical mouse comfort still needs the player.
The desktop command canvas also scaled twice. Fitting it once doubled the 960×540 expedition button's interior from 15 to 30 pixels.
[Windows playtest evidence](Docs/Testing/native-windows-camera-playtest.json).

## 2026-11-18 [J]: Size desktop commands separately from touch controls

The owner accepted larger controls on the Galaxy and later found the enlarged Windows controls too big.
At 1280×720, the revised Windows strip measured 854 pixels wide. The top touch buttons kept their bounds in Windows previews.
Test each layout at its own target size. The owner confirmed the exterior mouse orbit in a separate check.
[Control measurements](Docs/Testing/native-windows-desktop-controls.json) and [owner response](Docs/Testing/native-windows-orbit-acceptance.json).

## 2026-11-22 [J]: Keep the native flight view clear

The owner wants all current readouts and controls hidden at launch. Put them behind one reversible command.
Carry that choice across exterior and bridge views. [Interface direction](knowledge/interface-direction.md#native-playtest-decision-2026-11-22).

## 2026-11-22 [M]: Release injected playtest keys

The previous live launch forced E every frame. At combat range, the expedition order became available each time the helm reached Hold.
The game repeatedly saved the order and retried the first expedition after defeat. The journal closed normally at revision 151060.
Start owner sessions without persistent input. For an automated command, send a single press and release in the game’s input path.
The isolated panel capture paired `Input.+key H` with `DEFER Input.-key H`. The packaged keyboard test sent separate press and release events.

## 2026-11-22 [J]: Observe the expedition independently

The owner chose a free RTS camera for the main view. Selecting a ship opts into follow. Orders still operate Resolute and the first-person bridge remains aboard. [Interface direction](knowledge/interface-direction.md#native-observer-decision-2026-11-22).

## 2026-11-22 [M]: Frame the restored ship on the first ready-world tick

A pawn can begin play before its campaign is restored. A fresh campaign at the origin concealed an incorrect camera start. A save almost 492 km away exposed it. The packaged check now verifies the actual saved coordinates and heading before testing free movement. [Observer evidence](Docs/Testing/native-rts-observer.json).

## 2026-11-22 [J]: Ships and worlds keep working when the player looks elsewhere

The owner's example pairs a survey with building another ship at home. Returning to Resolute should show how far it traveled during that time. [Fleet direction](knowledge/game-direction.md#homeworld-and-fleet-exploration).

## 2026-11-22 [M]: Keep each run's save history separate

The native loader pauses writes when one journal contains multiple run identities or divergent revision tips. For the planned run picker, open the selected run's own history. Restoring a save point should recover its universe and both sides' progress. [Native save contract](native/Aquila/README.md#local-save-contract) and [run design](knowledge/game-direction.md#starting-and-resuming-a-run).

## 2026-11-22 [J]: Share combat rules and isolate practice state

The owner wants to test a loadout or a balance change without starting a universe. Use the match's combat code in a temporary arena with repeatable starting conditions. The current practice command shares the expedition's state and changes damage scaling. Check both match damage parity and journal isolation before using the arena to judge balance. [Simulator decision and source](knowledge/game-direction.md#combat-simulator).
