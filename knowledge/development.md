# Development and recovery

## Native Unreal foundation

The separate native project uses launcher Unreal 5.8.4. Follow [its guide](../native/Aquila/README.md) for build, test and packaging.
Its SDK and generated files remain local and ignored; the build wrapper does not alter DuskForge or global SDK settings.
Native checks and remaining device gaps belong in [native verification](../Docs/Testing/native-foundation-verification.json).
Commands below describe the retained browser/Electron reference, not the native runtime.

## Existing commands

Run in the repository root. Preserve `package-lock.json`; use `npm ci` on a fresh checkout.
Dependencies already existed during setup; no application package was upgraded or reinstalled.
Record the tested Node/npm versions in the setup receipt; a successful build does not establish version support.

| Purpose | Command |
| --- | --- |
| Browser development server | `npm run serve:dev` |
| Angular and Electron development build | `npm run build:dev` |
| Angular and Electron production build | `npm run build:prod` |
| Electron development app | `npm run electron:dev` |
| Electron production app | `npm run electron:prod` |
| Desktop packaging | `npm run package` or `npm run make` |

Build output is `dist/aquila-code-dark/`; it is not source and stays ignored.
Browser and desktop runtime checks are separate from TypeScript/Angular compilation. Packaging is not part of repository setup.
The current browser entry is the Three.js universe. The 2026-11-15 build also compiles Electron, but its desktop runtime remains untested.

## Universe checks and preview

The local development preview currently listens on `http://127.0.0.1:4200/`, started with:

```powershell
npm run serve:dev -- --host 127.0.0.1 --port 4200
```

Pure world and save checks use Node's TypeScript stripping, verified with Node 25.8.1:

```powershell
node --test scripts/tests/universe-model.test.mjs
node --test scripts/tests/interior-model.test.mjs
```

The browser regression uses installed Chrome through isolated Playwright tooling. It does not use the user's browser profile:

```powershell
npm install --prefix .runtime/browser-qa --ignore-scripts --no-audit --no-fund playwright-core@1.58.2
node scripts/qa-universe.mjs
```

The script requires the development preview for its read-only diagnostics. Set `AQUILA_QA_URL` for another local address.
Set `AQUILA_QA_CAPTURE_PREFIX` to a new round prefix before publishing captures; the default is `latest-`.
Never replace media already displayed in a historical report round. The glass/material refinement uses `r6-`.
It checks persistent canvas lifetime, ship switching, left steering/right camera input, canvas-only menu cancellation, pause, saves, sector travel, shuttle return,
responsive layouts, pointer release, graphics loss/recovery and unavailable graphics. Screenshots stay ignored in the task report's media folder.
The detailed local receipt is `.runtime/universe-browser-qa.json`; the durable summary is [universe verification](../Docs/Testing/universe-verification.json).
Refinement proof: [glass and materials verification](../Docs/Testing/universe-refinement-verification.json).
Quality-control regression: `node scripts/qa-quality.mjs`; use a new `AQUILA_QA_CAPTURE_PREFIX` for report media.
It checks real pointer/touch/keyboard choices, effective pixel density, buffer resets, frame progress and preference persistence.
Quality fix proof: [quality verification](../Docs/Testing/quality-control-verification.json).
Scale/performance check: `node scripts/qa-planet-scale.mjs`, against the development preview with a new capture prefix.
Run it alone, without another browser benchmark or build competing for CPU/GPU resources.
It measures first and warmed approaches, descent, 30 seconds of surface travel, bounded terrain allocation and quality budgets.
It also checks carrier collision, return, travel to another world and the logarithmic depth fallback.
Results: `.runtime/planet-scale-qa.json`; durable summary: [planet scale verification](../Docs/Testing/planet-scale-verification.json).
CPU-throttled phone viewports do not establish physical-phone or laptop GPU, thermal or power performance.
Thirty-second resource checks are not long-session leak tests. Fresh agent-session compliance remains untested.
Battleship exterior checks use `node scripts/qa-battleship.mjs` with a fresh capture prefix.
They cover actual mesh dimensions, slower capital handling, open-gallery entry, boosted wall contact, inspection saves/return,
planet clearance, surface return, replacement-geometry cleanup and narrow layouts. Use the development preview for read-only diagnostics.
Current exterior evidence: [battleship verification](../Docs/Testing/battleship-verification.json).
The inspection action is a temporary exterior flight tool; it does not establish walkable rooms or docking.
Camera checks use `node scripts/qa-orbit-camera.mjs` with a fresh capture prefix.
They exercise native right-drag orbit, return radius, mixed buttons, pause/blur/cancel, inspection, arrival assist and surface flight.
Reduced motion returns directly. Current height and hangar anchors are checked against rendered mesh bounds.
Evidence: [camera and height verification](../Docs/Testing/orbit-camera-verification.json).
Interior checks use `node scripts/qa-interior.mjs` with a new capture prefix.
They enter the bridge, use native mouse lock and Escape, issue helm orders, switch views, walk through the passage,
launch Peregrine, fly away, return aboard and walk back to command without simulation setters.
They also check bounded repeated visits, preserved route state, quality changes, narrow controls, context recovery and orbital reloads.
Use the same isolated browser tooling and development diagnostics described above. This is not a physical-phone performance benchmark.
Current proof: [interior verification](../Docs/Testing/interior-verification.json).
Walking camera regression: `node scripts/qa-walk-camera.mjs`. It checks native capture, drag, walking, Escape and recapture.
Separate injected faults cover large movement samples, unsupported raw input and denied lock. They do not capture the user's hardware fault.
Evidence: [walking camera verification](../Docs/Testing/walk-camera-verification.json).
Combat model checks: `node --test scripts/tests/combat-model.test.mjs`.
Practice browser regression: `node scripts/qa-combat.mjs` with a new `AQUILA_QA_CAPTURE_PREFIX`.
It starts practice through native controls, observes automatic acquisition/fire, toggles Automatic/Hold fire, and checks continued fire while walking.
It also covers shared views, pause/routes, victory/defeat, reset, lost/reacquired range, bounded resources, narrow controls, recovery, reload and travel.
Diagnostics never set combat or position. Current evidence: [projectile combat](../Docs/Testing/projectile-combat-verification.json).
The focused suite also checks delayed damage, blocked armor, ammunition changes, live-round preservation and fixed pool reuse.
Use F at the starboard tactical console, then Start practice. Weapons acquire, aim and fire themselves; no T/X actions remain.
Exterior Start practice also works. Two kinetic turrets fire automatically; damage follows projectile contact.
Standard rounds drain Sentinel's shield but cannot penetrate its reinforced armor. Blocked hits cause no hull damage and stop new ineffective fire.
Choose Piercing rounds under Practice ammunition to defeat armor. Fitting rounds changes future launches only.
Hold fire stops new shots, not flying rounds or incoming damage. Automatic resumes when a target is valid and ammunition is suitable.
Walking preserves mode and ammunition; change them at a station or outside. Reset selects Automatic/Standard; End practice returns to exploration.
Practice damage is temporary; changing ship, taking a shuttle or traveling ends the encounter. Reload keeps only the voyage.
Enter bridge appears during free capital flight. Return aboard restores the previous room after an exterior view.
Walking controls: WASD, Shift to run, click or drag to look, arrows as an alternative, F to use a nearby station, C outside.
Escape releases the cursor and pauses. At the command console, choose navigation orders; F leaves while the helm continues.
Shuttle return is guided and immediate; reloading resumes the battleship outside. Interior personal position is visit-local.

Automatic helm regression: `node scripts/qa-navigation.mjs`, with a fresh capture prefix and the development preview.
It verifies hidden/gated capital inputs, both camera drags, automatic approach, walking during navigation, order cancellation,
pause/settings/recovery, planetary clearance, retained shuttle flight, range/withdrawal orders, narrow controls and reload behavior.
Pure destination/turn checks: `node --test scripts/tests/navigation-model.test.mjs`.
Current evidence: [automatic helm verification](../Docs/Testing/automatic-helm-verification.json).
Battleship commands are Hold position, Approach orbit/Atlas, Keep combat range and Withdraw; no manual flight input is required.
The existing manual flight implementation and controls remain available for shuttle and surface play.
Pause/settings retain an unfinished battleship course. Manual shuttle input still cancels its arrival assist.
Launching from the hangar stops capital movement before the shuttle departs. Reload clears navigation orders.
Camera, interior, battleship and scale scripts now reflect command navigation; earlier performance receipts remain historical.
The installed odd-numbered Node version emits an LTS warning; a passing build does not establish upstream support for that version.

## Repository safety checks
The package has no npm test script. Setup safety tests run with:

```powershell
.\.venv-tools\Scripts\python.exe -B -m unittest discover -s scripts/tests -v
py -B scripts/check-startup-context.py --handoff Docs/Handoffs/HANDOFF_2026-11-14_setup.md
```

## Recover a checkout

1. Obtain source from `git@github.com:jacobmott/AquilaCodeDark.git` at the intended revision.
2. Read the shared core and task index. Create the isolated storage-helper environment per [storage](storage.md).
3. Restore an approved manifest into a new empty recovery directory. Check every hash.
4. Compare against the checkout and place only approved missing asset directories at their recorded relative paths.
   A fresh checkout needs `src/assets`; it also keeps original art in `AllAssets` and authoring data in `Tiled`.
5. Restore deployment configuration separately from an authorized private source. Never put tokens or credentials in Git or the asset bucket.
6. Install locked JavaScript dependencies, then build. Verify the game in a browser and Electron before calling runtime recovery complete.
7. Reinstall the local Git hook with `git config core.hooksPath .githooks`; config does not travel in a clone.
   Configure account memory and shared-service paths deliberately on another machine; loopback bindings are machine-local.

## Deployment boundary

The live site is `https://aquilacode.io`, using its own S3 website bucket through CloudFront.
Private asset backup is separate. Do not run upload, CloudFront invalidation, DNS, certificate or hosting-policy changes on a build request.
Deployment needs explicit authorization, review of the build output and exact website destination.
Legacy detailed instructions are retained in the setup history, not the current operating route.
