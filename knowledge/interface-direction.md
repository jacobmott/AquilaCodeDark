# Interface direction: 3D world with glass overlays

2026-11-15 UTC. Read-only reference review for the Aquila design discussion.

Current controls supersede the historical refinement below: both battleship drags orbit; shuttles use left steering and right orbit.
Walking supports mouse lock with drag/keyboard fallbacks. See [current architecture](architecture.md) for the active input rules.

## Native playtest decision, 2026-11-22

The owner wants the ship and space clear by default. Open one command panel on demand. Health, orders, hints and buttons belong inside it.
Any future always-visible indicator needs a specific player need. Windows uses **H** in flight and on the bridge.
Touch uses two fingers to reveal the controls and **Hide menu** to dismiss them. [Native play controls](../native/Aquila/README.md#play).

## Boarding playtest controls, 2026-11-24

The command bridge is a place to watch the ship fight. Entering the room leaves walking and looking under the visitor’s control. The ship selects its target and fires independently. Keep the windows clear of controls, target markers and firing prompts. [Delta Tree bridge](../native/Aquila/README.md#delta-tree-command-bridge).

The owner requested five times the player’s walking speed and three times the mouse sensitivity during boarding playtests. Both hulls use 13.5 m/s for walking. The same mouse movement produces three times the turn on foot and in exterior orbit. Acceleration and braking scale with walking speed for short moves near doors and stations. [Boarding controls](../native/Aquila/README.md#glacier-tongue-boarding-test).

## Native observer decision, 2026-11-22

The main view is an RTS observer. The player can look around space independently while the flagship navigates and fights automatically. Clicking a visible ship opts into following it. Panning or clicking empty space returns to free observation. The first-person bridge remains aboard Resolute. In the Windows prototype, ship orders always address Resolute through the command panel. [Native controls](../native/Aquila/README.md#play).

Free look turns around the observer’s own position. The match initially frames Resolute and the arena initially frames the midpoint between ships. Those opening views set the initial aim. Following a ship puts the orbit center on that ship. Releasing follow keeps the current position and heading.

Forward and backward flight follow the camera’s pitch and heading. Aim at a ship above or below you and hold **W** to fly toward it. **S** backs away along the same line.

On November 23, 2026, the owner said “Okay, that works well” after trying this Windows free-camera movement. [Windows flight checks](../Docs/Testing/native-camera-flight.json).

For fleet play, the owner wants to jump between a traveling ship and an inhabited planet. Looking home gives access to what that world can build while the ship continues its orders. [Homeworld and fleet exploration](game-direction.md#homeworld-and-fleet-exploration).

## Windows turret fitting

Four turret models float around the selected ship. They rotate in place while the fight waits. Click a model, then a port to install that weapon. The same model appears on the hull. The choice stays active until the player finishes placing or switches ships. Every empty port can still be inspected when inventory runs out. [Fitting controls](../native/Aquila/README.md#combat-simulator-on-windows).

Click **Armor** or **Shield** on the hull to switch the floating models. Armor samples use the same geometry and finishes as the ship’s deck plates. The hull changes while the player compares samples. The shield fitting view shows its powered shell and fitted hardware. Returning to a turret port restores the weapon choices. Port actions sit beside the ship. **Start duel** and **Cancel** sit above it.

The owner accepted the armor and shield fitting view. Hovering a turret demonstrates its configured fire on a small shielded, reinforced ship above the hull. Clicking pins the example. Another hover temporarily takes precedence. Changing ammunition restarts the preview with that payload. A clipped ribbon moves readable phrases outward from the turret and pauses on its last phrase for one second. The target, readout and ribbon accept no pointer input. [Windows firing previews](../Docs/Testing/native-firing-preview.json).

The lower ammunition row shows three rotating 3D rounds when fitting a burst turret or kinetic cannon. Standard has a brass casing, Piercing has a long pointed silver tip, and Phase has violet shards suspended inside two rings. Clicking a round or its name updates the firing example and sets the payload for the matching turret or next placement. Each ship and port keeps its own loadout. [Windows ammunition models](../Docs/Testing/native-round-models.json).

**Clear fitting** sits beside the ship tabs. It immediately strips the selected hull from any fitting view and releases the current placement or move. The same ship and part stay selected, ready to rebuild. **Cancel** only closes fitting. Reopening returns to that ship unless the player has since selected another ship in the arena. **Start duel** begins a fresh fight. [Windows reset checks](../Docs/Testing/native-live-fitting.json).

## Owner's reference

The owner described the Robinhood project's newest direction: the main website is a Three.js application,
with transparent glass-like interface elements layered over that application. They asked us to inspect that approach.
The reference request itself supplied no implementation authority. The later "Yep, let's do that" authorizes the Aquila adaptation.
The browser reference used a follow-behind default camera. The native observer decision above supersedes that default for the game.

## Observed reference source

Read from `C:/RobinHoodTrading`, without edits, builds, launches, data calls or account actions:

- `portal/capital-orbit/style.css:2`: root and scene host fill the viewport.
- `portal/capital-orbit/style.css:16`: glass settings style uses a tinted gradient, border, shadow and backdrop blur.
- `portal/capital-orbit/style.css:27`: the inspector is transparent text; its empty surface passes pointer events through.
  Buttons and selectors receive pointer events separately. Glass panels and transparent hover text are distinct treatments.
- `portal/capital-orbit/demo.js:16`: scene host, activity labels, toolbar, inspector and settings share the root.
- `portal/capital-orbit/demo.js:46`: settings are an HTML overlay; this is not text rendered inside the 3D scene.
- `portal/capital-orbit/scene.js:86`: the scene creates a WebGL renderer and perspective camera.
- `Docs/Design/CAPITAL_ORBIT_FULLSCREEN_REVISION_2026-11-14.json`: earlier full-viewport design intent and historical fixture QA.
- `reports/crypto-gap-scanner-2026-11-11/build_page.py`, round 85: the saved surrounding website is still a layout proposal.
  It describes an orbit-led workspace, exact-information panels and quiet translucent surfaces, not a completed website shell.

The user's current message supplies the newest desired glass-overlay direction; not every detail is present in saved source.
Current source and proposal were inspected, not a freshly rendered runtime. Historical browser checks were not rerun.
The prototype is concurrent, untracked peer work. Ownership stays in Robinhood; no code, assets or data copied into Aquila.
Reference inspection did not resume any financial research, acquisition, trading or other project activity.

## Aquila adaptation

- A persistent full-viewport Three.js scene provides the main experience.
- Normal HTML/CSS controls above the canvas provide navigation, readable text, forms, keyboard focus and glass styling.
- Home can show the universe behind an introductory panel and entry controls.
- The left navigation can become a compact glass strip that opens overlays while preserving the scene.
- Ships can move into a hangar view, with ship choices and specifications in glass panels.
- Play can switch to the selected chase camera and reduce panels to a small flight display.
- Settings and other information can open in overlays and return to the same scene.
- Owner now prefers nearly invisible panel fills, faint edges and light blur; brighter text and shadows provide readability.
- Route panel input separately from flight controls; interacting with a panel must not steer or fire the ship.

These proposals are now implemented in a first local playable slice: Home, Fleet, Atlas, Play and Settings share one renderer.
The original code uses native procedural geometry and shaders; no Robinhood code, media, data or services were imported.
Glass styling, camera framing and ship designs remain open to owner review. Browser checks cover the new Aquila implementation.
This direction does not mean every menu must be a 3D mesh or that all regions stay loaded simultaneously.
Angular remains the frontend. The authorized implementation adds pinned Three.js 0.186.0 and matching types.

Current task: [game direction](game-direction.md) and [checkpoint](../Docs/Handoffs/HANDOFF_2026-11-15_game-direction.md).

## Glass and surface refinement — 2026-11-15

The owner requested much clearer backgrounds for all box-like controls, stronger text as needed, richer ship/planet textures,
and right-mouse input on the game canvas. This authorizes the local refinement; publication authority is unchanged.

Shared glass now uses roughly 1.2–2.4% tint and 3px blur. Mobile navigation and pause/help/error overlays use the same treatment.
Action buttons, ship selections, settings fields and touch controls also have faint fills. Text and icons have brighter tones
and local shadows rather than relying on dark rectangles. The later quality fix replaces the native select with direct choices.

Original procedural hull textures add plate seams, finish variation and scuffs. Softer bevels, inspection hatches, fasteners,
engine rims and reflective canopy material add detail. An original runtime light map supplies metal reflections.
Planets now have warped continents, shallow coast water, fine terrain, relief shading, ocean highlights and a separate cloud shell.
These are native meshes and shaders, not downloaded or provider-generated assets. Surface geography remains a prototype.

Right-drag steers only during active flight. Left drag and touch drag still work. The canvas cancels browser context menus only
on the Play route. HTML controls retain their normal menus. Pause, release, pointer cancellation, route changes and blur clear input.
The flight hint and controls overlay explain right-drag. No pointer lock or global browser-menu suppression is used.

Implementation references: [Three.js standard material](https://threejs.org/docs/pages/MeshStandardMaterial.html),
installed Three.js shader chunks, and [MDN context menu events](https://developer.mozilla.org/en-US/docs/Web/API/Element/contextmenu_event).
MDN documents Firefox's Shift/right-click bypass; this pass verifies Chrome, not Firefox.
Knowledge Hub query `three.js material` using Aquila/OpenAI returned `budget_exhausted`; no unavailable source was bypassed or reused.

## Quality control fix — 2026-11-15

Owner reported a hang opening/changing the quality dropdown. The original popup responded during one Paseo check;
the exact client lockup was not reproduced. The browser host later disconnected, so no specific browser-root-cause claim.
The control now uses native grouped radios styled as three faint glass choices: Light, Balanced and High.
All choices remain visible without an OS popup. Keyboard arrows and touch selection preserve native radio behavior.

Source inspection found `setPixelRatio()` calls `setSize()` internally, followed by another explicit `setSize()` in Aquila.
Every preference update called that path. The scene now copies incoming preferences, compares quality, caches viewport values,
and calls `setDrawingBufferSize()` once only when logical size or effective pixel ratio changes.
Motion, sensitivity and repeated selections do not reset the canvas. Display-density caps remain 1 / 1.4 / 2.

Four focused browser groups cover actual clicks at density 2.4, touch at phone size, arrows, reload, buffer changes and continued frames.
Eleven full game groups and production compilation also pass. Proof: [quality verification](../Docs/Testing/quality-control-verification.json).
Earlier tests used direct select-value assignment and did not open the native popup; that gap is now explicit.
Hub query `select popup` returned irrelevant course sources; none used. Evidence comes from installed code and runtime checks.
