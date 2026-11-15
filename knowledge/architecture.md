# Application architecture

## Active universe, 2026-11-15

The owner approved building the shared 3D world with glass overlays after the Robinhood reference discussion.
Angular 18 remains the application shell. Three.js and its types are pinned to 0.186.0; no existing package was upgraded.
The active routes no longer import Phaser. Legacy game and ship templates remain available as source history.

- `src/app/app.component.ts`: one router outlet. The former global sidebar and Phaser event listener are no longer mounted.
- `src/app/app.routes.ts`: a persistent universe parent with Home, Ships, Atlas, Play and Settings children.
  Angular's default route reuse keeps the parent alive. The old detach-every-route strategy is no longer registered.
- `src/app/universe/universe.component.*`: accessible HTML controls, glass panels, responsive layouts, route state and preferences.
- `universe-scene.ts`: one WebGL renderer, camera, animation loop, flight input, bounded streaming, local saves and cleanup.
  Rendering runs outside Angular; instrument updates enter Angular about six times per second.
- `universe-model.ts`: deterministic sector/world identity, fleet data, coordinate helpers and validated versioned saves.
- `universe-art.ts`: original sky shaders, stars and source-authored ship meshes. No generated raster assets.
- `battleship-art.ts`: original Resolute exterior, instanced hull details, open hangar galleries and named future window/launch anchors.
- `ship-interior.ts`: a cached human-scale bridge, passage and hangar scene, using the same renderer as space.
- `interior-model.ts`: connected deck bounds, furniture clearance, substepped walking and nearby interaction zones.
- `combat-model.ts`: fixed-step projectile practice, automatic bursts, swept contacts, ammunition, shield/armor resolution and bounded pools.
- `combat-art.ts`: two aiming kinetic turrets, one drone, shield shells and instanced rounds, flashes, impact rings and sparks.
- `navigation-model.ts`: combat standoff/withdrawal destinations and bounded shortest-angle helm turns.
- `planet-fields.ts`: shared CPU and shader geography and height equations.
- `planet-visuals.ts`: distant globes, clouds, rings and custom-material depth configuration.
- `planet-tiles.ts` and `planet-terrain.ts`: bounded cube-sphere terrain planning, gradual construction and disposal.

Flight uses a perspective camera behind and above the ship, with full three-dimensional translation and yaw/pitch steering.
Shuttles retain keyboard thrust, strafe, height, boost, brake, left-drag steering and phone-sized pointer controls.
Capital flight uses navigation orders. Manual controls remain in source but are hidden and input-gated for the battleship.
Either drag orbits the exterior battleship camera without moving the ship. Walking controls remain independent.
Right-drag orbits around the controlled ship without changing steering, surface altitude or arrival assist.
Release eases back behind the ship along a spherical arc; reduced motion returns immediately. Camera offsets are transient.
Surface camera positions stay above sampled terrain. Pause, route changes and lost input clear camera control.
Only the flight canvas cancels context menus; HTML controls retain browser behavior.
Hull materials use object-space procedural surface detail with PBR lighting and an original runtime reflection texture.
Each distant planet uses a modest globe and separate cloud shell. Reflection textures are disposed with the renderer.
Panels, route changes, blur and visibility changes prevent held input from leaking into flight. Escape pauses.
Two nearby shuttle models give depth references during small-craft flight; they have no AI, damage or collision simulation.
The three earlier craft are shuttle classes. Resolute is the default capital ship for new voyages; old valid craft selections persist.
Its current dimensions are 13,600 × 4,800 × 1,280 m: 200×/75×/80× the Peregrine reference shuttle.
The owner requested twice the initial height, keeping length and width. Hangar anchors and openings follow the vertical scale.
Ship meshes match declared solid dimensions; exhaust is visual only. Fleet framing and flight handling follow vessel size.
Home's battleship action opens its fleet entry; the fleet view labels a reference shuttle at the same physical scale.
The exterior inspection mode parks Resolute and flies Peregrine from a lateral hangar approach.
Seven swept sample rays provide coarse hull collision. Open galleries remain traversable; this is not rigid-body docking or damage simulation.
Inspection saves retain parked capital position/orientation, and command return restores them. Route changes clear held input without losing the excursion.

Space loads a 3×3×3 neighborhood of 800,000-unit sectors, creating one world per frame.
Seeds recreate each world's identity when revisited. World radii range from 30,000 to 70,000 units;
Vesper has radius 60,000. Its diameter spans 2,000 starting-ship central hull lengths, excluding engines and bevels.
These are compressed game proportions, not real astronomical dimensions.
World positions stay in JavaScript numbers; visible objects subtract the current ship position before GPU transforms.
Evicted sectors dispose their geometry and materials. This supports continuing travel, not a claim of mathematical infinity.
The atlas supplies a six-second arrival assist. Shuttle manual input cancels it; battleship input cannot override an order.
Exterior arc waypoints avoid planets blocking the route.
The same swept collision check remains a final safety boundary if an unresolved obstruction remains.
Boost reaches 450 times maneuvering speed in deep space, slowing near worlds. Swept collision prevents tunneling through planet limits.
Ships stop at the greater of 3,500 units or 6.5% of planet radius, plus a conservative hull envelope.
Collision and arrival arcs use the same envelope, independent of rotation; arrival stops another 1,300 units out.
Descent eligibility includes that hull margin. Chase-camera rays shorten their reach if the planet would obstruct them.

Near a world, a shuttle descends over four seconds to 350 units above terrain; reduced motion skips the animation.
Capital trips use Peregrine; standalone shuttle trips retain the selected craft.
The carrier's orbital position stays saved. Orbit and surface use the same radius, direction, continent field and terrain heights.
The camera switches to the shuttle; this is still a controlled transition rather than unrestricted seamless atmospheric flight.
Tangent movement curves around the actual globe. Shuttle clearance ranges from 80 to 6,000 units above sampled terrain.
Surface walking, ground landings, economy, missions and multiplayer are not implemented. Orbital practice combat is described below.

Only one nearby planet gets detailed terrain. Six coarse face roots remain as coverage while a bounded quadtree refines nearby patches.
Each frame builds at most one 12×12 patch; obsolete patches dispose geometry. Shared field normals and inward skirts cover LOD borders.
Quality leaf budgets are 54/90/138, with ancestors included in resident caps of 70/118/182.
Low-detail horizons remain visible; this is procedural prototype terrain, not finished geological art.
Quality also caps drawing pixels at 1.2/2.2/4.2 million, star counts and shader detail. Light disables cloud shells.
Shaders warm asynchronously where the driver supports it. First-use compilation can still cause a brief frame spike.
Use reversed depth when the context exposes `EXT_clip_control`; otherwise use logarithmic depth with matching custom shader chunks.
Select these modes exclusively: this pinned engine version does not disable one automatically when both flags are true.

Local storage keys: `aquila.voyage.v1` and `aquila.preferences.v1`. Voyage payload version 2 contains ship, orbital position and yaw/pitch.
Version 1 positions migrate by the sector-size ratio, retaining sector identity; startup enforces the new orbital limit.
Surface excursions resume at the carrier after reload. Corrupt or extreme saves fall back to a new starting voyage.
No cloud save or account services are called. Browser storage restrictions are handled without stopping flight.
WebGL context loss pauses with a recovery notice; restoration stays paused. Missing WebGL 2 shows a readable fallback.
Development builds expose read-only `window.aquilaDiagnostics()` for the browser regression script; production builds do not.
Quality uses three native radio choices instead of a popup. Viewport dimensions and effective pixel ratio are cached;
the renderer receives one drawing-buffer resize only on actual size changes. Other preferences do not resize it.

## Life aboard Resolute

The approved first interior loop adds first-person walking, a usable helm, connected passage, parked Peregrine and launch/return.
Rooms are authored separately from the hull, as the owner requested. Eye height is 1.75 m; walking/running speeds are 5/10 m/s.
Substepped X/Z movement slides along walls and blocks consoles, the parked shuttle and the hangar's open-space boundary.
Approaching the helm or boarding station enables an F-key action and equivalent HTML button.
WASD walks, Shift runs, arrow keys look, and dragging also looks. A left click requests mouse lock; Escape releases and pauses.
Pointer capture must precede the lock request because Chrome can grant it immediately. Further clicks while locked skip capture.
Late lock grants are released after leaving walk mode, pausing or changing route. Drag and arrow controls remain available without lock.
Walking mouse input rejects individual axis samples exceeding 0.35 radians after sensitivity, and rejects non-finite values.
Only the suspect axis is discarded; continuous normal turns keep their full range. Rejected samples are never queued for later motion.
This guards both captured mouse input and absolute-position dragging; drag positions rebase even when their angular sample is rejected.
Capture requests unadjusted mouse input, retries ordinary lock only for NotSupportedError, and retains drag controls when lock is denied.
Lock transitions clear the drag origin. This is a mitigation for the reported intermittent vertical snaps, not a captured hardware diagnosis.
At the command console, navigation orders replace manual capital flight. Dragging looks, then returns forward.
Leaving the console preserves automatic movement. C switches outside; Return aboard restores the previous room or station during this visit.
The initial exterior action is Enter bridge. Selecting another craft or resetting the voyage resets the cached personal position.

Space renders first. The room scene renders after clearing depth, using matching camera field of view and aspect ratio.
The world camera follows named exterior bridge/hangar anchors and the same vessel orientation; open windows reveal real surrounding space.
Own hull geometry is hidden inside. The passage blends the virtual room origin between those anchors; rooms need not physically fit.
One renderer/canvas remains throughout. Room geometry is created once on first entry, retained for reuse, and disposed on scene teardown.
Distant rooms are hidden behind the passage. Rendering counters accumulate both passes; authored sign textures are explicitly disposed.
This is a bounded three-room layout, not a general deck-streaming system or total-GPU-memory measurement.

Boarding first stops any capital navigation order, then launches Peregrine outside the starboard aperture, facing outward.
The existing inspection simulation keeps Resolute parked.
Return to hangar is an immediate guided return and disembark action, not a manual approach, landing animation or docking latch.
The original capital position/orientation are restored; the player stands at the boarding station and can walk back to command.
Room state survives route changes and quality selection during the current visit. Voyage saves still store the capital's orbital state.
Reload resumes external capital flight, not personal deck position or a separate persistent shuttle excursion.
Planet visits still use the existing orbital descent action; the new hangar sortie does not add atmospheric entry.
Crew, engineering and private quarters remain unbuilt. Rooms and instruments are prototype art.
Evidence and limitations: [interior verification](../Docs/Testing/interior-verification.json).
Camera follow-up: [walking camera verification](../Docs/Testing/walk-camera-verification.json).

## Automatic battleship helm

Owner's round 29 direction replaces piloting with decisions and orders. Projectiles and the prior prototype received owner playtest acceptance.
Approach orbit/Atlas choose a destination. The helm plots the existing planet-clearance path and arrives at station keeping.
Hold position cancels movement. Keep combat range moves toward or away from Sentinel to a provisional 35 km standoff.
Withdraw moves out to 85 km, beyond the 60 km practice firing range; a ship already farther away stays there.
The 1 km range tolerance avoids repeated tiny routes. These numbers are local practice choices, not settled combat balance.
Navigation and weapons are separate: moving or withdrawing never silently changes Automatic/Hold fire or fitted ammunition.
Any rounds already in flight retain the combat rules. This is positional navigation, not projectile-aware automatic evasion.

Courses use the existing six-second eased route, swept planetary clearance and conservative hull envelope.
Heading changes are limited to 0.45 radians/second, with bounded frame input and shortest-angle wrap.
This is kinematic prototype navigation, not an acceleration-limited physical helm or a general tactical planner.
If a route encounters unresolved obstruction, the ship stops and reports it instead of repeatedly pushing through.
The helm runs while walking or using consoles; room/exterior changes preserve the order.
Pause, focus/visibility loss, settings/routes and graphics recovery freeze course progress. Recovery stays paused.
Returning to Play resumes retained orders. Hold position, ship changes, launch, voyage reset and terminal battle outcomes cancel them.
Practice reset holds position; ending practice clears range/withdrawal orders. Starting a new destination ends temporary combat.
Reload restores saved position/orientation with Hold position; orders are not persisted in the voyage payload.

Orders appear in the existing glass action panels, without another opaque control panel.
Manual capital hints, flight reticle, touch thrust and helm piloting controls are hidden.
Keyboard/mouse flight input cannot steer the battleship; WASD still walks aboard and shuttles retain manual flight.
Combat maneuvers use station/exterior controls; global Atlas destinations remain accessible aboard.
Evidence and limits: [automatic helm verification](../Docs/Testing/automatic-helm-verification.json).

## First practice battle

The owner approved report round 20's complete practice encounter. Resolute's starboard tactical console is reachable from the helm.
F enters/leaves the station; dragging or arrows look without steering. The station releases mouse capture and preserves active navigation orders.
Start practice is available there, at the helm once an encounter exists, and from the exterior action panel.
One Sentinel drone deploys ahead where the segment clears nearby planets. Alternate offsets handle a blocked forward spawn.
The owner clarified that weapons acquire, aim and fire themselves; consoles select operating modes rather than individual targets or shots.
Start practice defaults to Automatic. The ship acquires the valid target and fires on cooldown without keyboard or mouse combat input.
Automatic and Hold fire are available at tactical, the helm and in the exterior view. Hold fire keeps sensors active but suppresses shots.
Walking away from a station does not stop automatic weapons. Mode changes require a station or exterior control.
Changing views preserves mode, target coordinates, health, tracking, cooldown and outcome. Incoming fire continues after selecting Hold fire.

Report round 28 replaces both sides' instantaneous pulse hits with real traveling kinetic rounds.
Two foredeck mounts sit left/right at local X ±1350, Y 940, Z -4400. They track automatically and fire staggered four-round bursts.
First rounds launch after 0.6/0.76 seconds; burst spacing is 0.14 seconds, followed by a 1.65-second gap.
Outgoing speed is 8500 m/s; enemy speed is 7000 m/s. These are provisional game values, not final balance or realistic physics.
Sentinel starts after 2.8 seconds of engagement, firing two rounds 0.3 seconds apart followed by a 2.8-second gap.
Hold fire before engagement leaves practice in standby. Once launched, rounds keep traveling through hold, lost tracking or ammunition changes.

Each side has one 100-point shield and 100 hull. Standard ammunition has damage 6 / penetration 12; piercing has 8 / 60.
Sentinel armor threshold 40 blocks standard hull damage completely; compatible hits receive 25% hull reduction.
Resolute armor threshold 8 admits enemy kinetics, which deal 10 damage with penetration 25.
Shield overflow checks armor separately. Incompatible mechanisms or failed penetration never leak minimum hull damage.
Standard rounds initially drain shields. The first blocked hit identifies armor; new ineffective launches stop with a clear message.
Fit Piercing rounds at a station or exterior controls to resume. Already flying rounds retain their original ammunition.
These unlimited practice loadouts demonstrate compatibility; they do not implement inventory, upgrade ownership or ammunition logistics.
Outcome feedback distinguishes shield impacts/breaks, no penetration and confirmed hull damage.

Simulation uses 120 Hz steps, bounded incoming frame time and independent presentation. Contacts are swept, including relative ship movement.
Player shield/hull use rotated ellipsoids; the stationary target uses a sphere. These are coarse envelopes, not triangle-level collision.
Enemy fire predicts current velocity at launch but cannot steer afterward; later movement can make it miss.
Every flying round checks intervening planets and expires after 12 seconds if it misses. Launch acquisition remains limited to 60 km and clear sight.
Terminal outcomes stop both weapons and clear remaining practice rounds; defeat disables capital flight until reset/end.
Per-mount own-hull occlusion, constrained arcs, tracking-rate limits, pursuit and tactical enemy AI remain future work.

The model holds 160 reusable projectile slots and 32 impact slots. No pool saturation occurred in checked encounters.
One cached visual group reuses instanced tracer cores/halos, impact cores/rings and sparks. Muzzle flashes and recoil accompany both turrets.
Every live round is drawn on all quality levels. Quality changes secondary spark count only; collision and damage remain unchanged.
The existing renderer handles combat and interior passes. World positions subtract the ship origin before GPU transforms.
Pause, hidden tabs, focus loss, settings and other routes freeze encounter time. Context recovery remains paused.
Reset restores both sides, selects Automatic/Standard rounds and redeploys ahead without moving the ship or changing the deck station.
End practice removes the encounter. Travel, shuttle launch, craft changes and voyage reset also end it.
Reload restores the voyage without an encounter. Practice damage is visit-local and never written into the orbital save.
This is one bounded training fight, not persistent ship damage, general enemy combat or physical destruction.
Evidence: [combat verification](../Docs/Testing/combat-verification.json).
Earlier automatic-weapons proof: [automation verification](../Docs/Testing/automatic-weapons-verification.json).
Projectile milestone proof: [projectile combat verification](../Docs/Testing/projectile-combat-verification.json).
Later combined navigation/combat proof: [automatic helm verification](../Docs/Testing/automatic-helm-verification.json).

## Retained application

Source inspection, 2026-11-14. This is an existing game/app, not a new product scaffold.
The original package declares Angular 18, Phaser 3, Rapier 2D, Spine/Phaser and Electron 32.
Exact resolved dependencies belong to `package-lock.json`; do not upgrade as an incidental setup action.

- `src/app/`: Angular UI, routes and ship views. SVG files can be Angular source templates.
- `src/game/`: Phaser scenes and gameplay. Asset preload uses `src/assets/` through the Angular build.
- `electron/`: desktop entry point, compiled after the Angular build by the npm build commands.
- `src/assets/`, `AllAssets/`, `Tiled/`: ignored runtime media, source art and map-authoring data.
- `public/`: static build inputs. The pre-existing favicon is a narrow hash-pinned Git exception.
- `configs/`: legacy configuration helpers; its older README references a separate AquilaCode application.
  Do not run those helpers until their actual target paths and inputs are reviewed for this repo.

Deployment uses the separate website bucket and CloudFront distribution for `aquilacode.io`.
Private backup setup does not deploy or alter game behavior.
