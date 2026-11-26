# Lessons

## 2026-11-26 [M] Measure a curved armor edge in each source view

Glacier’s first fit brought the bay lip within half a pixel of the original painting but raised it above the side drawing. Folding the edge along the gallery sightline reduced the side discrepancy. The new face angle also darkened the finish under the same studio lights. Check the outline and color again after a depth correction. [Measured views and finishes](Docs/Testing/research-battleship-bay-armor-and-drives.json).

## 2026-11-26 [M] Grade doorway lights and their surround separately

Glacier’s first lit room nearly matched the painted interior brightness while the dark casing exaggerated its contrast. Sampling the collar separately brought its average to 74.8 against 76.1 in the painting. Rays through the opening and along the corridor independently confirmed the modeled depth. [Lighting, gallery marks and clearance probes](Docs/Testing/research-battleship-docking-and-windows.json).

## 2026-11-26 [M] Use the tread and back wall to check a ship landing

The painting gives the lip’s apparent position. The overhead sheet supplies its depth. A side ray checks the actual recess behind the deck. Sampling the entire model distinguishes a tread from its edge and supports. [Forward landing comparison](Docs/Testing/research-battleship-landings-and-cabin.json).

## 2026-11-26 [M] Check a glazed cabin against each reference view

All six overhead roof marks and four of five side roof marks hit Delta’s plated cabin. Only two of six roof marks in the original painting hit it. Retain the fixed gallery while fitting the footprint from the sheet, and record where glass marks encounter metal frames. [Observatory and armor evidence](Docs/Testing/research-battleship-forward-armor-and-observatory.json).

## 2026-11-26 [M] Check lit windows separately from reflective hull plating

With every studio light off, Glacier’s cabin panes remained visible and Delta’s metal seams went dark. Under fixed lighting the silver plates and slate floors moved closer to their reference colors. The glazing still averaged brighter than the painting. [Paired renders and measurements](Docs/Testing/research-battleship-feature-finishes.json).

## 2026-11-26 [M] Check camera tilt before fitting an exposed feature

The bow-only overlay put Glacier’s blade tip above its painted mark. Three fixed hull landmarks corrected the camera roll, scale and position. Four other landmarks moved from 80.75 to 23.92 pixels mean distance. Under the corrected camera the earlier tip sat below the mark. Compare two shapes in one checked camera before moving the mesh. [Frame and blade measurements](Docs/Testing/research-battleship-blade-and-courtyards.json).

## 2026-11-25 [M] Check a second landmark after matching the outline

Glacier’s overhead width reached 32.8% of its length against the sheet’s 33.3%. With the bows aligned in the 1,024 px painting, the model’s blade tip still sat about 46 px forward and 36 px high. Register a known point at fixed scale, then inspect another part before moving surface detail. [Measured landmarks and hulls](Docs/Testing/research-battleship-sheet-blockouts.json) · [Reusable comparison method](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#build-an-exterior-from-two-checked-views>).

## 2026-11-25 [M] Compare beam and handedness within a single generated sheet

The bow panels drew Delta Tree at twice its overhead beam and Glacier Tongue at about 1.5 times. Glacier’s underside kept the blade on the same page edge as its top with both noses pointing left. Calibrate from hull length, use the open bay to check handedness, and preserve original crops beside any corrected working view. [Measured sheets and marked outlines](Docs/Testing/research-battleship-turnarounds.json).

## 2026-11-25 [M] Check the reference pictures against each other

Every selected ship generation received its original. Twenty-one also received a picture later replaced in the pack. Glacier’s two selected stern views show different outlet counts, despite sharing the original and port drawing. Check part counts, spacing and handedness between the pictures before judging the mesh. Trace which earlier picture a later view used when a detail repeats. [Verified inputs and paired artwork](Docs/Testing/research-battleship-reference-consistency.json).

## 2026-11-25 [M] Check visible finishes from the side as well as the original camera

Glacier’s upper windows reached twelve marked spots in the main painting but only four in the port study. Moving their rim along the main camera’s sightline raised the port count to seven while preserving all twelve gallery hits. The overhead view still showed an oversized deck. Use named mesh and material hits to locate occlusion, then inspect the third view for changes in width. [Saved samples and drawings](Docs/Testing/research-battleship-upper-hulls.json).

## 2026-11-25 [M] Preserve the stern width when shortening a side fairing

Moving Delta’s full side fairing aft covered the arrival room. Shortening it from the front kept the doorway clear. Narrowing it also increased mean error against four marked stern widths from 1.97 to 29.42 metres. Restoring its original beam recovered those widths. Measure the profiles that a new change could affect even when they sit outside the main review angle. [Compared geometry and side studies](Docs/Testing/research-battleship-branch-deck-and-blade.json).

## 2026-11-25 [M] Use two views to place a ship entrance

Delta Tree’s first open hatch matched the oblique painting but sat too far forward in the port drawing. Using the bow, stern and doorway station placed it 71.3% aft. Sampling the interior and surrounding frame separately then exposed which hull faces blocked the room. Keep the straight elevation beside the gallery view when judging attached armor. [References, meshes and checked sightlines](Docs/Testing/research-battleship-arrival-portals.json).

## 2026-11-25 [M] Check the report named inside a Perplexity export

The loose camera conversation pointed to a separate full report inside a ZIP. Its three loose figures matched the ZIP members byte for byte. The full report identified its exact console request in the header. For a mixed download, follow the attachment list, preserve relative figure names and compare the console download with the original report bytes. [Verified imports and attachment inventory](Docs/Research/research-battleship-reference-matching-returns-v1.json).

## 2026-11-25 [M] Inspect the material assigned to the visible surface

Delta Tree’s export includes roughness and normal textures on several materials. Its painted upper deck uses one constant roughness and no normal texture. Inspecting textures across the whole asset would miss that distinction. On Glacier, fixed-crop brightness identified a darker dock. The crop includes geometry and reflected light, so its brightness alone cannot determine lamp power. [Material assignment and measured crop](Docs/Testing/research-battleship-material-review-baseline.json).

## 2026-11-25 [M] Check the full width of an opening and the supports beside it

The centers of Delta Tree’s twelve exhausts all looked open. A ring of samples around each center found a mounting rail across two lower rims. On Glacier Tongue, probing the complete model identified the blade as the first obstruction at five marked gaps. Checking intersections at each end also found a detached crossing that looked connected in the gallery view. [Model, artwork and physical checks](Docs/Testing/research-battleship-stern-shoulders-and-dock.json).

## 2026-11-25 [M] Inspect ship geometry from the stern as well as the gallery

Three drive outlets could meet their painted centers while their racks sloped across the stern. Setting lateral mounting planes let all three rows share one axis and stay level. A straight rear view also exposed the remaining offset between the stack and pressure hull. Keep secondary cameras with the original art and give the owner both angles. [Matched renders and reopened geometry](Docs/Testing/research-battleship-stern-and-armor-seams.json).

## 2026-11-25 [M] Check the far bank when aligning the near bank

Matching Delta Tree’s three near-side exhausts too closely from the gallery camera raised the far drives above the mast. The stern study constrained the bank spacing. That kept the skyline credible while bringing the visible outlets closer. [Model comparisons and measurements](Docs/Testing/research-battleship-registered-profiles.json).

## 2026-11-25 [M] Keep some painted features out of the hull fit

Thirteen painted corners guided Glacier Tongue’s bow. Five separate edge heights, checked in the same gallery camera, averaged 62.57 pixels from the painting before and 9.61 after. That check exposed a real change in the mesh while leaving taper, depth and finish to visual review. [Checks and original art](Docs/Testing/research-battleship-stern-and-prow.json).

## 2026-11-25 [M] Compare ship features at the same camera

Reframing each revision hid errors in the mast and port blade. Saving the earlier Blender camera made both errors measurable. A line through the rebuilt hangar also confirmed real depth behind its lights. [Aquila measurements](Docs/Testing/research-battleship-reference-refinement.json) · [Reusable Blender methods in Asset Pipeline](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md>).

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

Resolve swept contact against current bodies and planets, then inspect the defense layer.
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

## 2026-11-22 [M]: Select the play mode before the pawn starts

A packaged bridge regression caught the pointer inheriting menu visibility on a direct match launch. Select the session in game-mode initialization, before pawn startup, and refresh the pointer once its controller and world are ready. Check direct entry as well as menu entry. [Windows simulator verification](Docs/Testing/native-combat-simulator.json).

## 2026-11-22 [M]: Verify the shortcut on the Windows desktop

The duel link in chat failed for the owner. A shortcut on the Windows desktop opened the packaged game directly. The game log recorded a standard duel and Windows reported a responsive window. Use the desktop shortcut for this playtest. [Direct launch and controls](native/Aquila/README.md#combat-simulator-on-windows).

## 2026-11-22 [M]: Keep impact position and display time separate

The damage sphere sits inside the drawn hull. Carry the incoming line into the struck ship's frame and intersect the visible shell or plating. Age the effects in presentation seconds so an 8× duel still shows the blast. Packaged frames checked contact position, pause, replay and both defeats. [Windows combat feedback](Docs/Testing/native-combat-feedback.json).

## 2026-11-22 [J]: Show both sides of an arena matchup

The owner could see Resolute stop firing but could not see Sentinel’s rounds or explain the difference. Show each ship’s fitted gear beside its meters. State which battery stopped and which armor blocked it. Give each ship independent fitting controls so a player can repeat the duel after changing one item. [Simulator design](knowledge/game-direction.md#fitting-both-sides).

## 2026-11-22 [M]: Check a visual switch against the rendered frame

The oval button and component visibility both reported success while the rim disappeared on screen. Keeping the powered oval registered and switching its material opacity restored its rim. Check against the previous working view at the same camera and simulation time. [Windows shield comparison](Docs/Testing/native-mesh-shield-trial.json).

## 2026-11-22 [M]: Measure the camera eye in simulation space

A constant distance from an orbit center can pass a camera test while the camera travels around the scene. Reconstruct the eye from a ship’s known simulation position and the rendered camera-to-ship offset. Check both orientation and travel during a drag and when releasing follow. [Windows free-camera measurements](Docs/Testing/native-free-camera-pivot.json).

## 2026-11-23 [M] Compare resolved shots across different firing rates

The faster plasma weapon missed more often by raw count because it fired more shots. It also landed a larger share. Hold the target course, armor and trial duration fixed. Compare hits against all resolved shots and count armor blocks separately. Projectiles still in flight have no outcome yet. [Windows lab measurements](Docs/Testing/native-combat-lab.json).

## 2026-11-23 [J] Keep turret ownership separate from mount capacity

An empty mount remains inspectable after all owned turrets are fitted. Move the actual item with its chip and ammunition. Both opponents need independent inventories. [Ship fitting checks](Docs/Testing/native-ship-fitting.json).

## 2026-11-23 [M] Keep defenses active until the final volley resolves

An opposing ship can die before its last rounds enter interception range. Keep ticking each surviving turret’s cooldown and checking incoming rounds. Test a real kill with hostile shots still unclaimed and a control where the shot reaches the survivor. [Final volley checks](Docs/Testing/native-phase-rounds.json).

## 2026-11-23 [M] Test the floating model itself

Click the projected center of each model and the physical port in a packaged game. A caption click alone cannot prove either target lines up with the 3D view. Centering rotation on the mesh bounds keeps asymmetric turrets over their hit areas. [Windows placement checks](Docs/Testing/native-turret-gallery.json).

### 2026-11-23 [M] Test shield depletion with the live damage scale

The standard cannon’s base damage is 10. The current duel applies the expedition scale of 0.035, so one live hit removes 0.35 shield. Starting the packaged trial at one scaled hit produced an exact refill to 100 at both speeds. A separate simulation check drives larger hits through both banks and into the hull. [Reserve Capacitor checks](Docs/Testing/native-defense-fitting.json).

### 2026-11-23 [M] Verify both contact surfaces in a miniature firing range

Map the projectile’s shield entry and hull entry to their respective visible surfaces. A phase round can then be seen inside the charged shell before it deflects. In the Windows fitting preview, a short hold on depletion also makes the later armor impact readable. The independent combat world supplies the hits, shield charge and hull health. [Packaged firing examples](Docs/Testing/native-firing-preview.json).

### 2026-11-23 [M] Measure the gap under a rotating fitting model

Check the visible silhouette against its caption at the smallest window size. A rotated mesh can have empty bounding-box corners below its last visible pixel. The three ammunition models had 9 to 34 pixels of clear space above their names across four packaged Windows views. [Ammunition fitting checks](Docs/Testing/native-round-models.json).

### 2026-11-23 [M] Keep the inventory when clearing a fitting

A ship without an initialized turret rack receives starter guns when a duel starts. Keep its rack and reset each body to an unassigned spare. Resolute returned all 30 bodies to stock, and replay left both cleared ships unarmed. [Windows reset checks](Docs/Testing/native-clear-fitting.json).

### 2026-11-23 [M] Apply fitting changes inside the current duel

Clear must update the ship’s equipment and live shield charge together. A turret held by the previous armor needs to try the new defenses. The simulation checks cover both ships, including a wounded hull, a spent reserve and a round fired before clearing. [Live fitting checks](Docs/Testing/native-live-fitting.json).

### 2026-11-23 [M] Fixed mount positions shaped both first hulls

Both first briefs prescribed the same three straight rows of turret sockets. Both exterior studies came back with broad, stepped decks dominated by those rows. The owner wanted to discuss the outlines and walkable spaces first. Preserve the fitting capacity while developing each ship’s silhouette and deck plan. [Ship design discussion](Docs/Plans/ShipHulls/README.md).

### 2026-11-24 [M] Keep the concept seed and the image job ID

The design seed records how the ship prompt was assembled. Sunburst exposes no image seed in its current model settings. The exact prompt, provider job ID and original file hash identify the image a player picked from the numbered gallery. [Ship collection](Docs/Plans/ShipHulls/README.md#the-60-ship-collection).

### 2026-11-24 [M] — Verify a textured exterior from the standalone export

Fresh Blender scenes loaded both portable models with their maps embedded. The unpacked map files matched the embedded bytes. Rendering with the same lights and camera checked the exported finish against the editable scene. [Ship material checks](Docs/Testing/research-battleship-materials.json).

### 2026-11-24 [M] Check the height of the docking bay itself

Glacier Tongue’s arch gives the model height while the dock beneath the plates reads as a slot. Rendered angles all inherit that geometry. Compare the bay opening and hull thickness with design references before detailing a revised model. [Hull profile comparison](Docs/Plans/ShipHulls/README.md#hull-proportions-and-reference-views).

### 2026-11-24 [M] Give overhead art a registered camera and floor plan

Glacier Tongue’s first overhead study still showed the side of its docking bay. A true overhead Blender guide corrected the camera. The three first deck studies also inherited exterior details. A flat routing diagram for each deck fixed the orientation and preserved shared access points in the reference set. Use the registered diagrams when placing the lift and stair. [Reference packs and earlier studies](Docs/Plans/ShipHulls/README.md#completed-reference-packs).

### 2026-11-24 [M] Verify the boarding world in the rendered frame

The first packaged walk passed through both doorways while its hull and deck rendered black. Movable runtime mesh components and lights restored the scene. The Windows boarding check now inspects the middle of each saved frame, away from the HUD, alongside its collision and input checks. [Boarding evidence](Docs/Testing/native-glacier-boarding.json).

### 2026-11-24 [M] Compare ship size from a fixed berth

The whole-ship orbit fits either silhouette in the frame. At the fixed shuttle sill, the deck-to-roof height measured 58.72 m and 116.11 m. At corresponding hull positions, the roof height doubled exactly within measurement precision. Keep the player, doorway, shuttle and camera fixed for an on-foot size comparison. [Windows size trial](Docs/Testing/native-glacier-scale.json).

The roof passed that check even when the landing pad still left 3.5 m beside each wing on both hulls. Measuring the slab’s four collision edges caught the missing scale change. The corrected larger pad leaves 39 m beside each wing. [Windows pad comparison](Docs/Testing/native-glacier-pad.json).

### 2026-11-24 [M] Check the entire shuttle approach

On Delta Tree, a box containing Peregrine’s wings, hull and tail cleared the approach to its parked position. Lowering that box onto the apron registered contact. Keeping the dock, nearby hull and fittings in both checks tests clearance against the same world the player sees. [Packaged boarding checks](Docs/Testing/native-delta-boarding.json).

### 2026-11-24 [M] Check a shield hit from the player's eye

A shield impact projected inside Delta Tree’s window while the roof still hid part of the flash. Moving the bridge to the tower’s forward edge exposed the bow. A light on the sill now follows damage to the visitor’s ship. Compare the quiet window and the hit from the same standing position. [Windows bridge duel](Docs/Testing/native-bridge-duel.json).

### 2026-11-25 [M] Check the gun base with the shield on

Trace the center and rim of each gun base against the rendered hull. Check the same mount up close with a powered shield. Scaling a gun with the entire shield drew a second barrel above a seated cannon. Using only the hull for the shield removed that copy. [Packaged checks and views](Docs/Testing/native-duel-hulls.json).

### 2026-11-25 [M] Check bridge transfers with rounds in flight

Pause during a volley and switch bridges. Compare round IDs, positions and velocities before advancing the clock. Then walk several steps, turn, and make the round trip. This checked the same fight from both sides and exposed each bridge’s saved footing and view to a separate test. [Packaged two-bridge checks](Docs/Testing/native-two-bridges.json).

### 2026-11-25 [M] Check texture coverage on the playable mesh

The early exterior models had packed textures while the playable hulls assigned one texture coordinate to every corner. Checking texture area per face now covers all 10,364 Delta Tree triangles and all 5,596 Glacier Tongue triangles. Compare fitted armor to the sample for that ship. Each hull uses its own painted panels under the armor tint. [Surface checks](Docs/Testing/native-ship-finishes.json).

### 2026-11-25 [M] Compare the mesh the game actually loads with the selected artwork

Both Windows hull exports matched the boarding proportion studies byte for byte against the finish receipt. Rendering that geometry in gray beside each gallery original exposed straight spokes in place of Delta Tree’s curved branch walls and a thin rail in place of Glacier Tongue’s deep blade. The prior texture and boarding checks did not measure those features. [Compared sources and views](Docs/Testing/native-hull-likeness.json).

### 2026-11-25 [M] Check the review geometry after removing the paint

Each revised ship reopened from its self-contained 3D export at 1,280 m. Rays through Delta Tree’s modeled deck found eleven recessed courtyards. Separate rays through Glacier Tongue found all three crossing roofs and floors. A painted view, gray view and free orbit let the owner judge likeness separately. [Geometry and interchange checks](Docs/Testing/research-battleship-hull-review.json).

### 2026-11-25 [M] A matching highlight can hide a flat wall

Glacier’s uniform dock finish came closest to the painting’s whole-opening highlight. Zooming into the rear wall showed bright, blank panels. Two patterned surfaces improved the interior color and brought back service details. Sampling nearby armor isolated the change to the dock. [Lighting comparisons and measured regions](Docs/Research/research-battleship-reference-matching-review-v1.md#materials).
