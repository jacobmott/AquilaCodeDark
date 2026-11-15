# Lessons

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
