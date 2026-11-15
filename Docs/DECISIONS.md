# Decisions

- 2026-11-14 [J]: Adopt Production Tools' shared session knowledge, owned-work commit rules, three-account Claude memory,
  Codex setup, private asset storage and four shared applications. Owner approved the complete proposed setup and additions.
- 2026-11-14 [J]: Project records are the shared source of truth. Session wrap saves context;
  explicit newsession additionally commits and pushes only owned work. Installing the skill does not invoke it.
- 2026-11-14 [J]: Repair legacy backup privacy, exclude secrets, retain existing assets and separate private backups from website deployment.
- 2026-11-14 [M]: Source-native SVG templates require an Aquila-specific guard. Existing embedded-image and favicon exceptions
  are pinned to exact hashes; this setup does not rewrite game UI or published Git history.
- 2026-11-15 [J]: Owner explicitly requested session wrap, commit and push of this session's complete setup work.
  Site changes are the next discussion; no specific site edit or deployment was requested yet.
- 2026-11-15 [J]: Discuss remaking the website and game around endless exploration, with smaller craft for planet visits.
  Three.js is tentative; camera, flight model and website look remain open. No implementation or deployment decision yet.
- 2026-11-15 [J]: Ships move freely above and below other ships. This supersedes the proposed planar-flight default.
  Camera, roll controls, flight realism and implementation scope remain open; no deployment authority added.
- 2026-11-15 [J]: Default flight camera follows behind the ship. Real report submission:
  `reports/game-direction/inbox/20261115T015927Z.json`, question `aquila-direction-r2-camera`, value `chase`.
  Camera tuning and optional views remain open. This selection does not start implementation or authorize deployment.
- 2026-11-15 [J]: Inspect Robinhood's 3D application with transparent glass-like UI as the reference for Aquila's interface discussion.
  Source review confirms the existing scene/overlay pattern; the complete reference website is still proposed.
  Aquila layout and styling remain discussion scope, with no implementation or deployment authority added.
- 2026-11-15 [J]: "Yep, let's do that" accepts the shared universe with glass overlays, fleet view and chase-camera flight.
  Proceed with the local website/game remake. This does not authorize deployment, commit, push, asset transfer or paid generation.
- 2026-11-15 [M]: First playable slice retains Angular and adds pinned Three.js 0.186.0 with matching types.
  Procedural art, bounded sector streaming, local saves and a separate shuttle surface view are implemented for review.
  Details and limitations: `knowledge/architecture.md`; proof: `Docs/Testing/universe-verification.json`.

- 2026-11-15 [J]: Push all box-like UI toward almost invisible glass; strengthen text, improve ship/planet surfaces,
  and make right mouse input a game control on the canvas. Right-drag steering is the local implementation choice.
  No publication, paid generation or transfer authority added.
- 2026-11-15 [J]: "Yeah, let's go ahead and do that. Do do all your recommendations."
  Approves local scale and browser-performance implementation following report rounds 8–9: larger proportions, nearby terrain,
  practical cruise, bounded resources, quality budgets and approach/descent measurements. Publication and spend remain separate.
  Physical target-device benchmarking remains outstanding because only this desktop is available.
- 2026-11-15 [J]: Existing small space ships become shuttles in the design. Main vessel should be a giant research battleship:
  home, research platform, carrier and armed combat ship. Owner wants walkable interiors and working bridge consoles,
  with first-person bridge combat and external third-person control of the same ship.
  This turn asks feasibility; a compact bridge/hangar prototype is proposed, not implemented. No publication or spend authority added.
- 2026-11-15 [J]: Interior rooms may be separate spaces without exact hull fit. Battleship should evoke Battlestar-scale carrier mass,
  with an original design and approximate relative dimensions of 200× length, 75× width and 30–50× height versus current craft.
  Plan bridge, engineering, hangars, personal quarters with live space views, crew sorties, ship guns and visible shield impacts.
  These are future design requirements; crew and the full feature set are explicitly deferred. No game changes requested this turn.
- 2026-11-15 [J]: "Okay, yeah, that sounds like a good plan." Accepts the staged battleship plan from report round 13.
  Proceed locally with its first milestone: rough exterior, shuttle comparison, handling, hangar approaches and planetary clearance.
  Walkable interiors, crew and combat remain later stages. No commit, push, deployment, transfer or paid generation authority added.
- 2026-11-15 [M]: First exterior uses the working name Resolute and Peregrine as its reference shuttle.
  Provisional dimensions: 13,600 × 4,800 × 640 metres, or 200× length, 75× width and 40× height.
  These implementation choices remain reviewable; they are not owner approval of a final name or physical size.
- 2026-11-15 [J]: Hold right mouse to orbit around the ship; release to return behind it. Left mouse remains steering.
  Double Resolute's height while keeping width and length. Current dimensions become 13,600 × 4,800 × 1,280 metres.
  This supersedes the initial 640 m height and 40× reference ratio. No publication or paid generation authority added.
- 2026-11-15 [J]: Owner confirms the current camera controls and taller hull work and likes the result.
  Asks what to work on next. Recommend the planned bridge, helm, passage and hangar launch/return loop.
  This turn records acceptance and the recommendation; no interior implementation, publication or spend occurred.
- 2026-11-15 [J]: "Yes, let's do that. Proceed." Authorizes the proposed local bridge, helm, passage and hangar loop.
  Walk aboard, command the same battleship, launch Peregrine, return and resume command. Separate room geometry and guided return are accepted scope.
  Crew, combat, engineering and quarters remain later work. No publication, transfer or paid generation authority added.
- 2026-11-15 [J]: "Yep, that all sounds great. Let's do it." Authorizes round 20's local practice battle:
  tactical console, target lock, one gun, return fire with visible shield/hull hits, shared bridge/exterior state and reset.
  Crew, research, engineering, quarters and richer art remain later work. No publication, transfer or paid generation authority added.
- 2026-11-15 [J]: Owner clarifies that all weapons acquire, aim and fire automatically. Players may change operating modes at consoles.
  This supersedes manual target-lock and trigger controls from the first practice implementation. Local modes are Automatic and Hold fire.
  Weapon simulation continues while walking aboard; no aiming or individual-shot input is required. Publication and spend remain unauthorized.
- 2026-11-15 [J]: "Yeah, that's that's how we want it." Confirms automated weapons and console modes as the intended design.
  Owner requests discussion of weapon behavior and combat rules for a combat-heavy game. This authorizes design discussion, not new gameplay implementation.
  Weapon families, costs, counters, shield rules and battle pace remain draft proposals in knowledge/combat-design.md.
- 2026-11-15 [J]: Owner chooses deliberate battles with readable projectile travel and fast, explosive impact feedback.
  All ordinary available weapons should be projectiles. Remove hitscan/effectively instantaneous long-range attacks from the current design.
  Keep those weapons only as future exceptionally rare equipment with large cooldowns; this supersedes the proposed ordinary beam family.
  The starter battleship is capable but begins at its lowest combat level, with one simple shield and basic kinetic weapons.
  Progression can add shield capacity/sustain/types/layers or replace shields with special armor, including reactive armor.
  Guided weapons can lose tracking through countermeasures. Automatic evasion is an upgrade direction, not guaranteed starter immunity.
  Turrets occupy finite physical mounts and have individual upgrades. Examples: two starter turrets and around thirty mature-ship turrets.
  Neither the final cap nor a year-long unlock schedule is fixed. Exact guidance, defense, balance and acquisition rules remain open.
  This turn records discussion; the running prototype's instant pulse remains pending replacement. No new runtime or publication work performed.
- 2026-11-15 [J]: Owner accepts the proposed weapon families as direction. Every weapon, including starter guns, must visibly fire cinematically.
  Autocannons should show a traveling stream of bullets/tracers; representative visible rounds are acceptable when performance requires them.
  Weapon level can increase spectacle, but starter weapons must already be satisfying and visibly active.
  Weapon/defense compatibility must support complete protection and genuinely zero damage from unsuitable weapons.
  Exact damage mechanisms, penetration thresholds, material matchups and feedback remain design proposals. No gameplay changed this discussion.
- 2026-11-15 [J]: Owner agrees with the discussed combat vision, including cinematic visible firing and meaningful weapon/defense compatibility.
  Accepted principles: separate damage type, penetration and amount; resolve defense layers; no hidden chip damage through complete protection;
  clear blocked-hit feedback, effective automatic weapon choices when defenses are known, and unchanged combat outcomes across visual quality.
  Exact balance, effects and implementation details remain open. This is design confirmation, not a new playtest or implementation request.
- 2026-11-15 [J]: "Yes, that sounds good. Proceed" authorizes round 27's first projectile encounter: two automatic kinetic turrets,
  visible traveling rounds, one shield and clear blocked versus penetrating hits. No publication, transfer or paid generation authority added.
- 2026-11-15 [M]: Local practice now replaces both sides' instant pulses with projectile contact damage.
  Standard/piercing practice ammunition demonstrates shield depletion and armor immunity; values and unlimited practice inventory remain provisional.
  Production build, 30 model tests, six safety tests and nine focused browser groups pass; details in the projectile combat receipt.
- 2026-11-15 [J]: Owner reports "projectiles work well" and that everything in the current prototype works well.
  This confirms the preceding projectile encounter through owner playtest, without extending that acceptance to subsequent changes.
- 2026-11-15 [J]: Keep battleship controls underneath but hide them. The player specifies decisions; the battleship adjusts and navigates automatically.
  This supersedes manual capital piloting and the earlier proposed manual evasion override. Shuttle piloting and walking remain available.
  No publication, transfer, paid generation or research submission authority added.
- 2026-11-15 [M]: Local automatic helm supports destination approach, Hold position, Keep combat range and Withdraw.
  Practice standoff 35 km and withdrawal 85 km are provisional implementation choices, not final balance or projectile-aware evasion.
  Orders continue while walking and survive paused views. Launching a shuttle parks the capital; reload clears orders.
  Manual capital input is gated, with original flight controls retained for shuttles. Verification is recorded in the automatic helm receipt.
- 2026-11-15 [J]: Owner pauses gameplay work and requests session wrap, commit and push before a new discussion.
  Publish only this continuing remake session's source, tests, records and report; preserve peer work and the shared index.
  The next topic will come from the owner. No further gameplay implementation, deployment, transfer or spend is authorized by this wrap.
