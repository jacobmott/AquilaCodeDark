# Game and website direction

Discussion began 2026-11-15 UTC (2026-11-14 local). The owner subsequently approved the shared world and glass interface build.

## Standalone matches

2026-11-22, round 115. The player starts a standalone, real-time match against one computer rival. The owner ruled out a story campaign. Beginning with the homeworld and Resolute, the player chooses where to explore, settle and fight. Both sides expand and seek each other across star systems. Their decisions shape the match until one eliminates the other.

### Starting and resuming a run

2026-11-22, round 116. At startup the player can start a new run or resume a saved one. Each run is one match. A new run starts from scratch with a homeworld, Resolute and one computer rival. The player can start again after a loss or whenever they want a different game.

The owner suggested generating a different universe for each run. A fresh map seed per run would vary what lies beyond the homeworld. Save points belong to their run. Loading one restores its map, discoveries, fleets, settlements and rival at that point. Keeping earlier runs available lets the player try a fresh start and return later.

The Windows prototype already saves and restores the active expedition, including its world seed. It uses a fixed default seed. Choosing among saved runs and generating a new universe for each are design work. [Existing local saves](../native/Aquila/README.md#local-save-contract).

## Combat Simulator

2026-11-22, round 118. The owner wants **Combat Simulator** beside **Play Match**. Enter a small arena directly, with every ship and all its compatible weapons, ammunition and armor available to try. Players can repeat a fight, change one part of a loadout and decide what they want to earn. Equipment descriptions should say where to find or earn the item during a match. A player can also stay in the arena for combat practice or a quick fight.

The same arena serves development. Start it without generating a universe or loading a run. Both modes use the same combat code and equipment rules, so a balance change can be tested against the same opponent under the same starting conditions. Simulator trials have their own temporary state. Damage, equipment, rewards and time spent there stay out of match saves. The owner mentioned aim practice. Capital ship turrets currently aim and fire automatically. The simulator's exact practice controls remain open. [Combat direction](combat-design.md#confirmed-direction).

**Recommended first Windows duel.** Use Resolute and the armored Sentinel already in the game. Select standard or piercing rounds, see the effect on shields and hull, then reset either loadout to the same starting positions and defenses. Compare both outcomes against the match’s damage settings. Check that entering, resetting and leaving the arena preserves a disposable match journal byte for byte, and that the match resumes at the same point. Expand ship and equipment choices after this comparison works.

The existing **Practice** command fights within the loaded expedition state, shares its save path and uses a different damage multiplier. A standalone arena and equipment catalog remain to build. [Encounter code](../native/Aquila/Source/Aquila/AquilaSimulation.cpp), [game startup and saves](../native/Aquila/Source/Aquila/AquilaGameMode.cpp) and [existing Windows controls](../native/Aquila/README.md#play).

## One computer rival

2026-11-22, round 113. Each side aims to find and eliminate the other. Both expand across star systems during the same game time. Settling more worlds gives either side somewhere to carry on after losing a world. The time spent strengthening the player's civilization also gives the rival time to settle places the player has yet to discover.

Scouting has a strategic purpose alongside finding resources, ship upgrades and places to settle. A survey can reveal the first sign of the rival or show that it has spread farther than expected. The precise loss condition, including whether a surviving ship can rebuild after the last settled world falls, remains open.

Resolute's current Windows expedition reveals an armed contact after a survey. A computer civilization that explores, settles worlds and plays to win the match remains to build. [Playable expedition](../native/Aquila/README.md#first-expedition-vesper-echo).

## Homeworld and fleet exploration

2026-11-22, round 112. The owner compared the game to building a civilization across star systems in real time. In that comparison, inhabited worlds fill the role of cities. Each is a place to return to, see what has developed and choose what to build. Ships are individual RTS units. The player can check where any ship has reached, then jump to another world. Voyages, construction and settlement growth share the same elapsed game time during play.

2026-11-22, round 111. The player manages a growing fleet and multiple inhabited worlds. The opening starts in one system with little discovered beyond it. An existing civilization on the home planet built Resolute, the first ship. The owner left the choice of Sol or another starting system open.

Send Resolute to survey another system. While it travels, return to the homeworld, use its resources and start building a second ship. Jump the view back to Resolute to see where it has reached, watch from outside or walk its bridge. The voyage and construction each keep progressing while the player looks elsewhere during play. Each planet offers the actions its capabilities support.

New discoveries provide resources for further ships and expeditions. Terraforming worlds and founding more civilizations are part of that expansion. Over time, settled worlds could supply a route, develop local technology or build their own ships. How a new civilization starts and how fast it grows remain open. The relaxed pace and automated travel and fighting carry forward from the first voyage.

The Windows prototype demonstrates one automated expedition and a free observer camera. Homeworld construction, simultaneous fleet missions and terraforming remain design work. [Playable expedition](../native/Aquila/README.md#first-expedition-vesper-echo).

### Ship loss and replacement

2026-11-22, round 114. Destruction costs the player that individual ship, including Resolute. Another ship can carry on exploring while a world with shipbuilding capability builds a replacement. Building extra ships ahead of a loss also lets the player keep more expeditions underway.

The current Windows expedition lets the player retry a failed mission. Fleet losses and replacements belong to the planned match. [Playable expedition](../native/Aquila/README.md#first-expedition-vesper-echo).

## Quiet exploration aboard Resolute

2026-11-22, round 110. The owner initially described Resolute as the player's home and presence in the universe. Round 111 broadened that role to the [homeworld and fleet](#homeworld-and-fleet-exploration). From aboard a ship, the player can walk around. From the free RTS camera, they can watch the same ship carry out its orders. Both perspectives belong to one voyage. The intended pace leaves room to stop and watch something happen.

The player chooses a destination, stocks the ship and decides which capability to improve. A faster or farther-reaching drive could open a distant route. Armor and weapons could let Resolute pass a defended region. Those upgrades expand where the player can go, what they can witness and which capabilities they can discover. The helm and weapons handle routine execution.

In round 110 the owner proposed terraforming planets and using fictional, renewable human incubation technology to establish a first generation. A settlement could develop independently while Resolute travels. On returning, the player could take a shuttle down to see what people built, collect supplies, or encounter locally developed technology and ships. A port on a previously empty stretch would change the range of the next voyage.

Imagine leaving a nursery beside an empty bay. On a later visit, a locally built tender meets Resolute in orbit. From the surface the player can recognize the bay beneath the new harbor. Refueling there enables a longer crossing. The harbor itself gives the player a reason to come back. The bay and tender are design examples. Travel time, settlement growth, and other reasons to explore remain open.

The existing native expedition demonstrates automatic travel, an encounter, salvage and a turret refit. The drive, stocking and settlement ideas above are design discussion. [Playable expedition](../native/Aquila/README.md#first-expedition-vesper-echo) and [native observer decision](interface-direction.md#native-observer-decision-2026-11-22).

## Owner intent

- Remake the existing website and game around the landing page's promise of cosmic exploration.
- Travel through a practically endless browser universe; find planets and explore their surfaces.
- Large battleships stay in orbit. Launch or use a smaller craft for planetary visits.
- Reconsider the original isometric perspective. Three.js is the owner's tentative rendering choice.
- Redesign landing page and left navigation. Preserve the appealing ship catalog concept.
- Discuss appearance and gameplay direction before implementing the remake.
- 2026-11-15 [J]: Free movement above and below other ships. The flight model is not confined to a plane.
  Exact chat answer: "Yeah, freely above and below other ships." This does not decide roll controls or flight realism.
- 2026-11-15 [J]: Default camera follows behind the ship. Exact submitted choice: "Follow behind the ship".
  Evidence: `reports/game-direction/inbox/20261115T015927Z.json`, question `aquila-direction-r2-camera`, value `chase`.
  Read in full after the owner said "I replied." No additional note accompanied the choice.
- 2026-11-15 [J]: Inspect the Robinhood reference: the full-screen 3D application is the website, with glass-like UI over it.
  Reference source and Aquila adaptation are recorded in [interface direction](interface-direction.md).
- 2026-11-15 [J]: "Yep, let's do that." This follows the shared universe, glass navigation, fleet, flight and settings proposal.
  Local implementation is authorized. Deployment, paid generation, publication and transfers remain separate.

- 2026-11-15 [J]: Make all box-like UI almost invisible glass, strengthen text as needed, improve textured ships and planets,
  and use the right mouse button as a game control on the canvas. Local changes proceed; deployment remains separate.

## First playable slice

Home, Fleet, Atlas, Play and Settings now share one persistent Three.js scene with glass HTML overlays.
Three original modeled ships, free 3D flight, chase camera, deterministic sector travel, local saving and shuttle surface visits work locally.
The atlas supplies arrival assist. Separate sphere-based surface exploration returns to the carrier's saved orbital position.
The current visual direction uses a dark sky, warm ivory type, restrained translucent panels and procedural worlds.
These are implementation choices for review, not a claim that the owner individually approved each art or control detail.
Architecture and limits: [application structure](architecture.md). Validation: [verification receipt](../Docs/Testing/universe-verification.json).

## Remaining design choices

Follow-behind camera is selected, not pending. Its exact height, distance and smoothing remain to be designed and tested.
An angled overview could remain an optional view; no optional camera was selected through this submission.
The old default-camera question is retired in round 3. Round 4 discusses the user's Robinhood glass-overlay reference.
Round 5 presents the first local implementation. Round 6 refines glass, procedural materials and canvas right-drag input. The website keeps the universe visible beneath ordinary HTML/CSS panels.
The initial planar-flight recommendation is superseded by the owner's explicit choice; report round 1 retains the historical discussion.
Stable seeded sectors, disposal and orbital saves are implemented. Persistent world modifications are not.
Finite spherical surface travel is implemented; seamless descent, detailed geography and surface activities remain future design work.
Distinct discoveries and meaningful activity matter more than the raw number of generated locations.

## Planet scale discussion, 2026-11-15 UTC

The owner notices ships look comparable to planets when approaching them and asks about real scale, shrinking ships and perceptual tricks.
This request is discussion only. No scale change is approved or implemented by it.

Measured source values: the home planet has radius 285 world units, so diameter 570. The starting ship's central hull spans
60 units before bevels and engines: 9.5 central hull lengths across the globe. Generated planet radii range from 180 to below 390.
Fleet length labels are 68, 46 and 112 metres; labels are not exact mesh bounds. Shuttle geometry uses scale 0.22.
Orbital collision currently keeps the ship center at least planet radius plus 90 units away; arrival assist ends at radius plus 180.
The surface view uses a separate radius-6000 sphere. These mismatched scales are prototype choices, not physical planetary proportions.
Sources: `src/app/universe/universe-model.ts`, `universe-art.ts` and `universe-scene.ts` in that folder.

Proposed direction, not an owner decision: much larger planet-to-ship ratios with compressed travel time; simple distant globes,
an orbital approach where the horizon fills the frame, and local terrain detail revealed through shuttle descent.
Retain consistent geography and curvature across representations. Keep the main ship outside the atmosphere and the chase camera close
enough for readability. A ship can occupy substantial screen space because the camera is nearby while remaining tiny beside a planet.
At a distant planet-wide view the ship should become a dot. Camera tricks alone cannot make tiny collision geometry convincing up close.

Shrinking only ships and enlarging only planets are equivalent ways to change their relative ratio; uniform scene scaling preserves it.
Smaller numbers do not inherently reduce rendering cost. Sphere radius and geometry subdivisions are independent; detailed visible terrain,
screen coverage, resource use and numerical precision need separate budgets. Real dimensions are possible in principle, but are not required
for this proposed experience. Current ship-relative rendering is a useful foundation, not proof of astronomical-scale precision.

Official references inspected: [sphere geometry](https://threejs.org/docs/pages/SphereGeometry.html),
[distance-based detail](https://threejs.org/docs/pages/LOD.html), and [camera precision](https://threejs.org/manual/pages/cameras.html).
These support engine mechanics, not a tested Aquila performance or perceptual result. No scale prototype or new runtime test ran.
Knowledge Hub searches used Aquila's OpenAI profile: `"floating origin"` was empty in the selected catalog;
`"planet scale"` returned three passage hits from two Duskforge source records, with complete selected coverage and no next page.
Those source cards concern terrain seams and Unreal water studies; they were not used as Aquila runtime evidence or copied into source.

## Browser performance constraint, 2026-11-15 UTC

The owner explicitly raises browser performance as a concern for massive planets. This continues the discussion; it does not request implementation.
Recommended design: bounded nearby terrain patches with distance-based detail, controlled texture/resource memory, capped pixel count and effects,
and gradual terrain preparation or background workers to avoid input-thread stalls. Large screen coverage can increase shading cost even with
unchanged geometry. Hidden objects can still occupy memory. Numerical diameter alone is not a useful performance estimate.

Existing code bounds the space-sector neighborhood and disposes evicted geometry/materials. Quality controls cap render pixel ratio.
Detailed planetary patch streaming, adaptive effects and worker-based terrain generation are proposals, not present capabilities.
Current surface terrain is one separate fixed-detail sphere. Earlier checks used hardware rendering on an RTX 5090;
responsive phone layouts do not prove physical-phone frame rate, heat, power or memory behavior.

Proposed next implementation slice, only after owner direction: one convincing planet approach/descent, measuring frame-time distribution,
loading spikes and memory over sustained travel on representative target devices. No universal frame-rate promise or device target selected.
No new application benchmark ran during this discussion. Engine guidance does not establish Aquila performance:
[browser graphics acceleration](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API),
[levels of detail](https://threejs.org/docs/pages/LOD.html),
[render-resolution cost](https://threejs.org/manual/pages/responsive.html),
[resource cleanup](https://threejs.org/manual/pages/cleanup.html).

## Approved scale implementation, 2026-11-15 UTC

Owner: "Yeah, let's go ahead and do that. Do do all your recommendations."
This authorizes local implementation of the scale and browser-performance proposals above, superseding their discussion-only status.
It does not authorize publication, spend or transfers. No new packages, downloaded art or provider generations were needed.

Vesper now spans 120,000 world units, compared with a 60-unit starting central hull: 2,000 hull lengths across.
The chase camera remains near the ship. Generated worlds have radii 30,000–70,000; sectors span 800,000 units.
Deep-space boost and arrival assist compress travel. Main ships stop outside terrain; the shuttle descends over four seconds.
Shared geography, height and radius connect orbital globes with nearby cube-sphere patches, replacing the separate miniature surface globe.
One patch builds per frame. Ancestor coverage, inward skirts and disposal keep terrain bounded while moving or changing quality.
Quality also limits pixel count, clouds, stars and shader detail. No background worker was necessary for these small patch jobs.
Reversed depth and a separately tested logarithmic fallback protect close ships against the wider view range.
Version 1 voyage saves migrate into the new sector scale. The save key remains unchanged; payload version is 2.

Implementation evidence and measured limits: [planet scale verification](../Docs/Testing/planet-scale-verification.json).
Hardware here is an RTX 5090 desktop. CPU throttling and phone layouts are proxies, not physical target-device results.
Real laptops and phones remain a required performance-validation gap. Low-detail distant hills remain a visible art limitation.
Descent is still a controlled camera/craft transition, not unrestricted seamless atmosphere flight or a ground landing.

Knowledge Hub follow-up used Aquila's OpenAI identity and read the permitted terrain source excerpt.
It informed separate checks for topology and normals; no external course text or implementation was copied.
Evidence stays local in the Hub receipt; independently authored game source and actual browser tests establish this implementation.

## Research battleship and walkable bridge, 2026-11-15 UTC

- [J] Positive feedback on the larger-world implementation: "That actually works really well."
- [J] Existing small aircraft-like space ships should become the shuttle class. A new much larger battleship becomes the main vessel.
  The battleship is also home, research vessel and shuttle carrier. Enterprise and Battlestar are role/scale references.
  Combat capability and military appearance are central; "Return to carrier" should mean returning to this actual vessel.
- [J] Desired interior: walk around in first person, approach working panels, configure ship systems and weapons,
  select/lock targets, and fight from either the bridge or an external third-person view.
  This turn asks whether the concept is feasible. Direction is recorded; the full interior/combat system has not been built.

Recommended first playable scope: one battleship exterior, bridge, short passage and shuttle hangar.
Walk to a working helm/tactical console, select a training target, fire one weapon and observe hull/shield damage.
Switch between bridge and external cameras without changing the ship's position, target, damage or settings.
Launch an existing small craft from the hangar and return to the same battleship. Research and living areas can expand later.
This scope is an agent proposal, not a claim that each feature or room is individually approved or present.

Proposed architecture: one persistent ship simulation with two camera/control modes. Bridge panels send commands to that same simulation.
Keep character motion in ship-local coordinates; synchronize the outside view with the vessel's actual location and orientation.
Render the active interior section and visible exterior; load adjacent sections as needed and release unused detail.
Browser feasibility depends on detail, lights, effects, screen resolution and simulation budgets, not only the ship's dimensions.
Art, movement/collision, docking, targeting, damage, weapons and saving make this a substantial development phase.
No whole-ship physical destruction, complete crew simulation or finished combat loop is implied by first-person camera support.

Primary references inspected: [first-person controls](https://threejs.org/docs/pages/PointerLockControls.html),
[object picking](https://threejs.org/docs/pages/Raycaster.html), and [transforms and visibility](https://threejs.org/docs/pages/Object3D.html).
These establish engine building blocks, not a tested Aquila interior, combat system or target-device performance result.
Knowledge Hub OpenAI search `spaceship interior` was empty with complete selected coverage.
Shortened `spaceship` returned ten first-page passages across five source identities, mostly media/course and plugin discovery.
Remaining pages were not inspected; no Hub source was adopted as implementation evidence or copied into project content.

## Battleship design requirements, 2026-11-15 UTC

Owner clarification, recorded for early design; do not build all these systems now:

- [J] Interior rooms may be separate spaces and need not fit precisely inside the exterior hull. Believable experience takes priority over exact architectural packing.
- [J] Battlestar Galactica is a scale and carrier-design influence, not a request for an exact replica. Give Aquila its own exterior design.
- [J] Approximate dimensions relative to the craft currently flown: 200 times the length, 75 times the width, and 30–50 times the height.
  Preserve these as relative targets. Reference craft, final metre dimensions and any shuttle resizing remain unsettled.
- [J] Plan for a crewed vessel. Crew is future work, explicitly not required now.
- [J] Interior destinations include bridge, engineering/engine room, hangar decks and personal quarters; the previously requested research/home role remains.
- [J] Hangars launch the player's planetary shuttle and crew-piloted combat craft. Crew should eventually sortie and fight alongside the battleship.
- [J] The battleship has its own guns and a visible shield response: enemy shots strike, deflect or explode against it.
  Exact projectile response, shield appearance and rules are still design choices.
- [J] Personal quarters have a window showing planets and the surrounding space, with room for future personal features not yet specified.

Design implications proposed by the agent, not existing implementation:

- Keep one persistent battleship identity and shared position, orientation, damage, power, weapons, hangar and crew state.
  Detached interior rooms address that same ship; switching views or rooms must not reset it.
- Give each window a logical position and facing on the vessel. Show the actual surrounding world from that viewpoint,
  even if the room dimensions differ from the exterior. Preserve outside direction and changes as the ship moves.
- Reserve exterior hangar openings, docking points and clear launch routes early. Interior/exterior alignment matters most where craft cross the boundary.
  Doors, passages or lifts can connect separately built rooms without requiring a complete physically packed deck plan.
- Reserve future crew stations, assignments and launch commands in the design without creating crew or autonomous combat now.
- Treat shield impact visuals and damage accounting as parts of the same system; camera changes should not change combat outcomes.
- Validate the proposed huge vessel alongside a shuttle and a planet before freezing dimensions. Camera offsets, turn rates,
  docking space and collision/orbital clearance must use the battleship's actual hull bounds, not the former small craft's center-only clearance.
- Load nearby rooms and budget visible exterior/effects. Separate interiors are an authoring strategy, not automatic proof of low memory use.

This turn updates requirements and report only. No exterior, interior, crew, guns or shields were implemented; application source remains unchanged.

## Proposed next milestone, 2026-11-15 UTC

Owner asks where to go next. Recommended sequence, not new implementation authority:

1. Rough battleship exterior in the existing universe. Establish an original armored silhouette, engine banks and hangar openings.
   Compare it with the selected reference shuttle and existing planet before fixing final metre dimensions.
   Pass criteria: shuttle visibly tiny, hull readable, controllable flight, usable hangar approaches and hull-aware planetary clearance.
   Reserve weapon positions and window viewpoints; detailed art and interior rooms remain later work.
2. Small aboard/launch/return loop: bridge, short passage and hangar. Walk aboard, launch the shuttle, return and resume command.
   Both views use the same persistent battleship identity and state.
3. One combat encounter: training target, target lock, one weapon, hull/shield damage and visible shield impacts.
   Verify that bridge and external control share targets and outcomes.
4. Expand into crew sorties, engineering, research and personal quarters after the core loop works.

Report round 13 records this proposal. Application code remains unchanged; no new gameplay or battleship performance was tested.

## First battleship exterior, 2026-11-15 UTC

Owner accepts the preceding sequence: "Okay, yeah, that sounds like a good plan."
This turn implements its first local exterior milestone. The later walkable, combat and crew stages remain unbuilt.
Publication, paid generation and transfers remain unauthorized.

- Working name Resolute; original angular armored prow, central spine, asymmetric command island, twin open lateral hangar galleries and six drives.
  Repeated armor, ribs, floor plates and running lights use shared instanced geometry. No model downloads or generated media.
- Peregrine is the implementation reference: solid hull 68 m long, 64 m wide and 16 m high.
  The three earlier craft are now shuttle classes; their displayed dimensions describe their solid meshes, excluding exhaust light.
- Provisional battleship dimensions: 13,600 m long, 4,800 m wide and 640 m high.
  Ratios are 200×, 75× and 40×. Name, reference, final dimensions and art remain reviewable; these are agent implementation choices.
  Planet dimensions remain compressed game proportions, not real astronomical scale.
- Fresh voyages select Resolute. Existing valid saves keep the selected craft and position; Home's "Meet Resolute" opens its fleet view.
  The fleet carries a true-scale reference shuttle and label. Camera framing, acceleration and steering vary with vessel size/role.
- "Inspect with shuttle" leaves the battleship stationary and places Peregrine outside its starboard gallery.
  Free flight and boosted inspection work around the hull; sampled swept rays stop the shuttle at solid walls while leaving bay mouths open.
  This is an exterior inspection tool, not a docking or walkable boarding system. "Resume command" restores the parked battleship's position and orientation.
  Saves during inspection record the battleship, so reload resumes command rather than abandoning it.
- Orbital collision, arrival routing and descent eligibility include a conservative hull radius, independent of orientation.
  Turning at the boundary therefore keeps the entire capital hull outside terrain. Chase-camera obstruction checks also avoid planets.
- Existing surface survey remains available; capital excursions use Peregrine, while standalone shuttle trips use their selected craft.
  Logical bridge, quarters and hangar anchors reserve later window/launch connections. Hardpoints are inert; no weapons or shields are simulated.

Local verification and limits: [battleship verification](../Docs/Testing/battleship-verification.json).
Report round 14 contains the rendered exterior, hangar approach, planetary comparison and narrow layout.
Inspection hull collision is a coarse sampled sweep, not a complete rigid-body physics system; no ship damage or docking latch exists.

## Camera orbit and taller hull, 2026-11-15 UTC

Owner requested right-button camera orbit with release returning behind the ship, and twice the battleship's current height.
Left-drag remains steering; right-drag turns only the view around the controlled battleship or shuttle.
The camera returns along an arc at its follow distance, avoiding a straight path through the hull. Reduced motion skips the return animation.
Orbit does not cancel arrival assist or change surface heading/altitude. Pause, route changes and capture loss clear held camera input.
Camera offsets are temporary and never enter voyage saves. Existing touch steering and canvas-only browser-menu suppression remain.

Resolute now measures 13,600 × 4,800 × 1,280 m; only height changed. Peregrine ratios become 200×/75×/80×.
The hull's existing vertical transform scales all exterior parts and future anchors; hangar approaches and opening height follow it.
Whole-hull planet clearance derives from the revised dimensions. No additional geometry or textures were added for this enlargement.
Evidence: [camera and height verification](../Docs/Testing/orbit-camera-verification.json); report round 15.
Knowledge Hub's first page for `orbit camera` contained cinematic/course references, not directly useful browser-control implementation guidance.
Remaining pages were not inspected. No course content was copied and no new research was submitted.

## Accepted exterior and next interior loop, 2026-11-15 UTC

Owner confirms: "Yep, that's perfect. That all works. I like it." Current camera controls and taller proportions are accepted.
Asked what to work on next. Recommend the second milestone from the staged plan:

1. Enter a small bridge in first person, with windows showing the battleship's actual surrounding space.
2. Use the helm to command the same vessel; switch between bridge and external flight views, then leave the controls.
3. Walk through a short passage to one hangar, board Peregrine and launch.
4. Return through a simple guided interaction, disembark and resume command aboard the same battleship.

Keep one persistent ship position/orientation and control state across those views. Interior rooms need not match the exterior exactly.
Load and budget active areas; check transitions, input capture, return position and resource use during implementation.
Crew, combat, engineering and private quarters remain later work. The immediate goal is one complete aboard/launch/return loop.
Report round 16 records this recommendation. No application source changed or interior runtime behavior was tested this turn.

## Approved interior loop, 2026-11-15 UTC

Owner: "Yes, let's do that. Proceed." This accepts the bridge, helm, passage and hangar loop proposed in report round 16.
The local implementation is complete: walk aboard, view nearby space through the bridge windows, command the capital from its helm,
switch outside, walk to Peregrine, launch, fly and return to the same parked battleship.
WASD walking, mouse lock with drag/arrow alternatives, wall/furniture bounds, contextual F actions and narrow touch controls are included.
The bridge and hangar use separate human-scale room geometry with exterior window anchors, rather than a physical fit within the hull.
Return is an immediate guided interaction; it does not simulate approach or docking. Reload resumes external capital flight.
The cached interior has three rooms, source-authored signs and a full-scale Peregrine, with one renderer and bounded repeated visits.
Crew, combat, engineering, personal quarters and more detailed interior art remain later work.
Evidence: [interior verification](../Docs/Testing/interior-verification.json). Report round 17 shows the local result.
The Knowledge Hub query for `first person` used Aquila's OpenAI identity. Its first page held mostly Unreal/course references.
No directly useful browser-interior implementation was reused; further pages were not inspected and no new research was submitted.

## Walking camera follow-up, 2026-11-15 UTC

Owner reports that the interior works fairly well, but the walking view occasionally snaps up or down.
Injected oversized mouse input reproduced a 74.48-degree vertical jump in the previous implementation.
Walking input now discards excessive individual-axis samples, requests raw mouse capture with a supported fallback, and clears old drag origins.
Normal sensitivity, full continuous turns, walking and the interior loop pass local checks.
Owner's subsequent chat retry: "Yeah, it seems like it's fixed now." Retry request closed in report round 19.
The exact original hardware event was not captured. Confirmation changes the reported outcome, not the earlier test evidence.
Proof and external reference limits: [walking camera verification](../Docs/Testing/walk-camera-verification.json).

## Next proposed milestone: first practice battle, 2026-11-15 UTC

Owner asks for current state and what comes next after confirming the camera fix. This is discussion, not implementation authority.
Recommend the next stage of the existing plan: a repeatable practice encounter aboard Resolute.
A bridge tactical console selects and locks one training drone. One battleship weapon produces visible shots and hits.
The drone returns fire; shield impacts are visible, and depleted shields expose hull damage.
Bridge and exterior controls share the same target, weapon, shield and hull state. A reset allows repeated practice.
Crew sorties, research, engineering, quarters and richer art remain later work. No combat code or new game testing this turn.
Report round 20 records the proposal. Existing flight, planet visits, interior loop and camera verification remain the current evidence.

## First practice battle built, 2026-11-15 UTC

Owner accepts the complete preceding proposal: "Yep, that all sounds great. Let's do it."
Local implementation adds a starboard tactical station and a Sentinel drone encounter, also reachable from the exterior controls.
Lock with T and fire with X or HTML buttons. One pulse lance and returning drone fire share shield-first damage and terminal outcomes.
Bridge, helm and exterior controls use one battle state. Reset restores both sides without moving the ship.
Enemy fire starts only after engagement. Pause/routes freeze it; reload, travel, shuttle launch and craft changes end the exercise.
Training damage is temporary. Sentinel is stationary; pulse hits are instantaneous with beam/impact effects, not projectile ballistics.
The cached visual group and six-effect pool remain bounded. Crew, research, engineering, quarters and finished art stay deferred.
Validation and precise limits: [combat verification](../Docs/Testing/combat-verification.json). Report round 21 presents the result.
Knowledge Hub searches used the configured OpenAI identity. The first projectile page contained ten passages from four Unreal-oriented sources.
No implementation was reused; further pages were not read. The exact Three.js/combat query was empty in the selected catalog.
No paid research or generation occurred. Source-authored geometry and existing installed dependencies supplied this milestone.

## Automatic weapons correction, 2026-11-15 UTC

Owner clarifies that all weapons lock on and fire themselves. Console interaction changes operating modes, not aim or individual shots.
This supersedes the manual T/X controls recorded in round 21. The local practice battle now starts in Automatic mode.
Weapons acquire valid targets and fire on cooldown. Hold fire retains sensors and stops outgoing shots without stopping incoming damage.
Automatic weapons continue while the player walks aboard. Mode changes use tactical/helm consoles or the equivalent exterior controls.
Range and planetary obstruction still limit fire; reacquisition is automatic. The prototype turret has unrestricted aiming rather than manual bow alignment.
This remains one stationary training target, temporary damage and original prototype art. General enemy tactics and mount-specific occlusion remain future work.
Proof: [automatic weapons verification](../Docs/Testing/automatic-weapons-verification.json). Report round 22 records the correction.
Configured OpenAI Knowledge Hub query for target acquisition returned no permitted selected-catalog results; no external implementation was reused.

## Combat design discussion, 2026-11-15 UTC

Owner confirms automatic weapons are the intended design and asks to discuss broader weapon and combat rules.
Confirmation concerns design; it does not establish a new owner playtest. No game code or game tests changed this turn.
Draft foundation and weapon families: [combat design](combat-design.md). Only the automation and earlier vessel requirements are confirmed.
Proposed rules tie player decisions to positioning, power, equipment and broad orders; weapons execute those orders automatically.
First open choice is battle pace. Recommendation: deliberate battleship combat with fast fighters and missiles.
All further weapon, damage, cost and counterplay details remain proposals. Report round 23 carries the discussion.
Owner asks what deliberate battles means. Round 24 clarifies time for several ship decisions while weapons remain automatic and craft move quickly.
No pace selected, gameplay changed or new systems approved. The clarified question follows the earlier pace question.

## Projectile combat and progression direction, 2026-11-15 UTC

Owner selects deliberate battles and describes a capable starter battleship that grows through offensive and defensive upgrades.
Available weapons must use meaningful projectile travel. Instant or effectively instant long-range attacks leave the available design.
Such weapons remain future notes only: exceptionally rare, with long cooldowns. This supersedes the earlier beam-family proposal.
One simple starter shield can grow into stronger or multiple shields, sustain systems, or a shieldless special-armor build.
Guided projectiles can lose lock to countermeasures; automatic evasion becomes another upgrade path with details still open.
Starting examples: two basic kinetic turrets, left and right. Long-term example: around thirty independently upgraded mounts.
The count, mount map and year-long progression example are not fixed balance commitments.
Updated [combat design](combat-design.md) separates owner requirements from proposed weapon families, guidance rules and capacity tradeoffs.
The prototype still uses instant pulse hits; replacement is the first required gameplay correction under this direction.
No game source changed during this discussion. Report round 25 closes the pace question using the owner's direct chat reply.

## Cinematic weapons and defense compatibility, 2026-11-15 UTC

Owner accepts the proposed weapon families and adds two requirements: visible cinematic fire at every level, and true zero-damage matchups.
Autocannons need readable traveling streams; representative visible rounds are allowed when performance requires them.
Higher levels can increase spectacle; starter weapons must already be visually satisfying.
Unsuitable weapons may be completely ineffective against particular defenses. Ship configuration and equipment choice must affect this outcome.
The updated [combat design](combat-design.md) proposes separate attack mechanisms, penetration and defense compatibility.
Readable blocked-hit effects, per-layer resolution, useful automatic targeting and quality-independent simulation remain recommendations.
Report round 26 records the direct chat direction. No projectile visuals, protection model or other game code changed this turn.

## Combat vision accepted, 2026-11-15 UTC

Owner agrees with the discussed vision. Cinematic firing, defense compatibility, penetration, per-layer outcomes and clear blocked-hit feedback are accepted principles.
Automatic equipment choices use known effectiveness; graphics detail must not alter combat outcomes.
Exact balance, discovery, effects and progression details remain open. Report round 27 records confirmation; gameplay is unchanged.

## First projectile encounter built, 2026-11-15 UTC

Owner authorizes round 27's milestone. Two automatic kinetic turrets replace the instant pulse; enemy shots also travel before dealing damage.
Standard practice rounds drain shields but cause zero hull damage against reinforced armor. Confirmed blockage stops ineffective new launches.
Piercing practice rounds demonstrate compatible damage. Station/exterior ammunition modes require no manual aim or target lock.
Both sides have one simple shield; no regeneration, upgrade ownership, ammunition economy or automatic evasion is implemented.
Muzzle flashes/recoil, separated tracer streams, shield ripples, sparks and hull impacts show firing and outcomes across camera views.
Fixed simulation steps, swept planet/body contacts and bounded pools preserve delayed damage and support misses when the ship moves.
Build, 30 model tests, six safety tests and nine focused browser groups pass. Source hashes and limits: [projectile verification](../Docs/Testing/projectile-combat-verification.json).
Report round 28 presents the local result. No new paid assets, research submission, commit, push, deployment or backup occurred.

## Battleship command instead of piloting, 2026-11-15 UTC

Owner says the projectiles work well and the current prototype works well. This confirms their preceding combat playtest.
Owner changes battleship control: keep the underlying controls, hide them from the player, and let decisions drive automatic navigation.
The player commands destinations and movement intent. Direct capital keyboard/mouse flight is gated; shuttle and walking controls remain.
The first local helm supports Approach orbit/Atlas, Hold position, Keep combat range and Withdraw.
The 35 km standoff and 85 km withdrawal are provisional practice distances. Weapon modes remain independent.
Orders continue while walking aboard. Pause/settings suspend them; launching a shuttle stops the capital first.
Neither general tactical planning nor automatic projectile evasion is implemented by this change.
Details: [architecture](architecture.md). Evidence: [automatic helm](../Docs/Testing/automatic-helm-verification.json).
Report round 29 records the change. No commit, push, deployment, transfer, paid generation or research submission is authorized.

## Native release proposal, 2026-11-15 UTC

Owner requests discussion of rebuilding the proven browser concept in Unreal for Steam, Apple and Android.
The proposed website becomes an information and store-link destination. No migration or website changes are approved yet.
[Native release direction](native-release-direction.md) records the recommendation, official sources and supplied AI-disclosure conversation.
Recommended release order: Steam first, with physical phone testing from the first native slice. The owner's preference remains open.
Accepted gameplay principles remain intact; platform choice, first release scope and minimum devices remain proposals.
Implementation stays paused. Report round 31 records this discussion; previous wrap publication authority is consumed.

## Shared progress and website direction, 2026-11-15 UTC

Owner favors releasing Steam and mobile together, with the same account and save available through linking or login.
Single-player is explicit: no multiplayer or networked gameplay. Online identity and save synchronization are a separate service concern.
This removes multiplayer from the planned game scope; older references remain historical, not future commitments.
The website will contain no playable game. Keep Three.js, space/glass styling, news, development logs and real store links.
Background gameplay videos are a proposed website feature. Existing browser gameplay remains a development reference.
[Native release direction](native-release-direction.md) separates these choices from proposed offline saves, conflict handling and account architecture.
Report round 32 answers the previous release-order question from direct chat. No migration, backend or website implementation started.

## Native direction agreed and saved, 2026-11-15 UTC

Owner agrees with the preceding proposal, including shared progress, offline local saving, later synchronization and preserved conflict versions.
The coordinated native release target and separate marketing website are accepted direction; service provider and commercial scope remain open.
The owner asks to preserve discussion and decisions and asks what comes next. This does not resume implementation or grant new publication authority.
The [native foundation brief](../Docs/Plans/NATIVE_FOUNDATION.md) proposes a small Unreal battle/interior and desktop/phone save round trip.
Report round 33 records agreement and the proposed sequence. The current browser prototype remains unchanged.

## Native foundation started: 2026-11-15

Owner authorizes the next milestone and selects launcher Unreal 5.8.4, separate from DuskForge's source engine.
Windows desktop and Galaxy S26 Plus lead feasibility; Mac/iPhone access is unavailable.
Native project and workflow: [project guide](../native/Aquila/README.md). Native evidence stays separate from browser receipts.
Local foundation includes flagship/planet/bridge, automatic command navigation and projectile combat, plus versioned local save revisions.
Cloud accounts, hosted synchronization, Android execution and Apple proof remain unfinished. Existing playable website remains unchanged.
No new commit, push, asset transfer, paid generation or website deployment is authorized.

## Evidence and limits

Local source contains the quoted landing introduction, ship catalog markup and isometric Phaser code.
Current application architecture remains in [architecture](architecture.md). The subsequent implementation received browser runtime checks.
Official Three.js documentation confirms orthographic and perspective cameras, rendering-library scope, and explicit resource cleanup:

- [Orthographic camera](https://threejs.org/docs/pages/OrthographicCamera.html)
- [Perspective camera](https://threejs.org/docs/pages/PerspectiveCamera.html)
- [Game guide](https://threejs.org/manual/pages/game.html)
- [Resource cleanup](https://threejs.org/manual/pages/cleanup.html)

Those sources support library behavior, not scale or performance claims. The local prototype checks are recorded separately.
Knowledge Hub searched using Aquila's OpenAI identity: `isometric` returned unavailable (`budget_exhausted`);
the inspected first page for `three.js` returned source references but no directly useful architecture guidance.
Further results were not inspected; no corpus-wide absence claim. No course text copied into project records.

Current task: [implementation checkpoint](../Docs/Handoffs/HANDOFF_2026-11-15_game-direction.md).
