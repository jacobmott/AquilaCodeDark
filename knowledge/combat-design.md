# Combat design discussion

2026-11-15. Combat vision agreed; first projectile encounter implemented. Exact balance and broader systems remain open.

## Confirmed direction

- The [Combat Simulator](game-direction.md#combat-simulator) uses the same equipment and combat rules as a match. A repeatable arena lets players compare builds and lets developers check balance changes without starting a universe.
- Weapons acquire targets, aim and fire automatically. Players change operating modes at consoles, never aim or lock manually.
- The player commands the battleship through decisions; its helm steers and navigates automatically.
  Manual capital controls remain in source but are hidden and gated. This supersedes the earlier manual-override proposal.
- Deliberate battles leave time for tactics and strategy. Impacts should feel fast, explosive and visually satisfying.
- A powered shield stays faintly visible. Its rounded shell follows the ship’s proportions. A hit brightens the point of contact. Mark hull hits on the struck plate. A destroyed ship ends in a large explosion. The owner refined the shield direction during the Windows duel on November 22.
- The starting research battleship is capable, but begins at its lowest offensive and defensive level.
- Ordinary available weapons must be projectiles with meaningful travel time. Shots can miss or face suitable defenses.
- Remove hitscan and effectively instantaneous long-range attacks from the available combat design.
  The current pulse lance does not fit. Retain such weapons only as future, exceptionally rare equipment with major cooldowns.
- Capital ships should expect to take hits. The starter has one simple energy shield that absorbs a few hits before failing.
  Exact capacity, what constitutes a hit, recharge and hull durability remain untuned.
- Future defense progression includes capacity, sustain, shield types, multiple shields and special armor that can replace shields entirely.
  Reactive armor and shieldless builds are explicit future directions; their damage rules remain open.
- Guidance can be disrupted. A missile that loses lock should no longer perfectly follow the target.
  Owner examples: continue straight or travel toward the last known position; exact behavior remains undecided.
- Individual physical turret mounts have a finite maximum. Each turret can have its own levels and upgrades.
- Starting direction: two basic kinetic turrets, one left and one right.
  Mature-ship example: around thirty turrets on top. These examples are not a finalized mount map, numeric cap or grind schedule.
  The owner's year of play illustrates long-term growth, not a required year-long progression.
- Existing full 3D movement, bridge/exterior command, home/research/carrier roles, visible shields and eventual crew sorties remain settled.
- Owner accepts the proposed weapon families as a design direction; exact roles, values and unlocks remain open.
- Every weapon must visibly fire and feel cinematic, including basic starter guns. Spectacle can grow with weapon level.
  Autocannons should show a recognizable stream of individual bullets/tracers toward the target.
  Showing a representative subset is acceptable if needed for performance; weapon use must remain visible.
- Weapon/defense compatibility must allow complete protection against unsuitable attacks, including genuinely zero damage.
  The owner's wood-versus-metal example describes a gameplay relationship, not a universal real-world material claim.
  Configuration and weapon choice must matter; some loadouts cannot damage particular defenses.

Earlier design confirmation: "Yes, I agree with that." The discussed visual and defense principles are accepted.
This includes cinematic launch/travel/contact, quality-independent outcomes, separate damage type/penetration/amount, layer-specific defenses,
no hidden chip damage through complete protection, readable blocked hits and automatic use of known effective equipment.
Specific formulas, thresholds, art, discovery rules and upgrade costs remain open; example matchups are not a final balance table.
This confirmation settles the design vision, not a new playtest or a request to implement every progression system now.
The earlier beam-family proposal and unanswered pace choice are superseded by this direction.

## Native combat feedback

**Ship fitting and projectile defense, round 131.** Both Windows arena ships have 30 selectable mounts. Turret ownership and mount capacity are independent. A turret item carries its weapon, kinetic ammunition selection and optional chip. Moving it between mounts preserves the whole item. Spare lab stock can bring either side to 30 guns. This loan applies to the duel. Acquisition, progression and chip scarcity remain open.

Round 133 changes how players place a weapon. Choose its rotating model in space, then click a port on the hull. The chosen model fits immediately and stays selected for further placements. Inventory rules still apply to every port. [Fitting view](interface-direction.md#windows-turret-fitting).

**Clearing one ship, rounds 137–138.** Clear immediately returns all turret bodies to that ship’s stock, including borrowed bodies. Their ammunition and chip settings reset. Armor, shield and reserve come off. Incoming rounds can damage the exposed hull. Cancel closes fitting at the same point in the fight. Existing damage, airborne rounds and spent reserve charges belong to that encounter. Both ships may start a fresh duel with empty mounts. [Live fitting checks](../Docs/Testing/native-live-fitting.json).

**Reserve Capacitor, round 134.** Either simulator ship can add this enhancement to its standard shield. The first hit that depletes a charged shield restores 100 shield points. Excess damage from that hit consumes the new charge before reaching armor. A second depletion leaves the shield empty. Phase rounds skip both banks. Removing the generator also removes the capacitor. Replaying a duel rearms each installed capacitor. Match acquisition and balance remain open. [Windows defense checks](../Docs/Testing/native-defense-fitting.json).

**Fitting range, round 135.** Each firing sample runs a disposable duel using the selected weapon, payload and chip. A short training shield makes the sequence readable. Standard fire exposes reinforced armor and bounces. Piercing fire exposes it and damages the hull. Phase rounds fly through the active shield before deflecting. Plasma damages reinforced armor. A chipped sample also intercepts an incoming practice shot. The original fight, inventories and journals have their own state. [Fitting controls](../native/Aquila/README.md#combat-simulator-on-windows).

A chipped turret prioritizes incoming kinetic or plasma projectiles before its regular ship attack. Two turrets cannot claim the same shot. The defensive shot travels, and only a completed interception removes its marked projectile. A projectile that reaches a ship first resolves its normal damage. Different weapons and armor stop rules are independent per turret. [Windows lab checks](../Docs/Testing/native-ship-fitting.json).

**Phase rounds and the final volley, round 132.** Either arena ship can fit phase rounds on a burst turret or kinetic cannon. They pass through the shield without draining it or causing a shield flare. A hit on light or prism plating damages the hull. Reinforced armor stops phase rounds in this trial. A round that crosses only the shield and misses the hull does no damage. A chip can intercept phase rounds in flight.

Destroying a ship stops new attacks against it. Its incoming rounds remain live. Each surviving chipped turret keeps its own cooldown and can acquire those rounds after the explosion. A late round can still decide the duel. [Windows test results](../Docs/Testing/native-phase-rounds.json).

Light plating accepts every payload. Reinforced armor blocks standard kinetic rounds and takes 75% of piercing damage. Prism plating stops both plasma weapons and takes kinetic damage. Shields absorb every payload first. These values are provisional.

**Hull-shaped shield trial, round 126.** The owner asked to try a slightly enlarged copy of the ship as its shield. The Windows arena offers **Ship mesh** and **Oval** on the live duel panel. Ship mesh follows the actual plates, turrets and gaps between the side sections. The test uses one uniform scale of 1.08 about the ship’s pivot. Switching while paused shows exactly the same strike on either surface. Both keep faint idle light and brighten at contact. The expedition uses the fitted oval pending the owner’s comparison. [Windows trial](../Docs/Testing/native-mesh-shield-trial.json).

The oval fits around the hull and turrets. An elongated hull gets a long oval, a tall hull gets an upright oval, and equal dimensions give a sphere. Both shield styles send a ripple from the point of contact. Depletion lets the last ripple fade. A shieldless ship has a bare hull. Gold sparks mark an armor deflection. A penetrating round heats the struck plate and throws orange fragments. One shot can exhaust the shield and damage the hull.

At zero hull the ship disappears into an expanding fireball. Smaller blasts run along its length, followed by a shockwave and cooling fragments. Effects use real elapsed play time at both duel speeds. Pause holds them in place. Replay clears them. [Windows controls](../native/Aquila/README.md#combat-simulator-on-windows) and [packaged visual checks](../Docs/Testing/native-combat-feedback.json).

## First implemented projectile encounter

Owner's "Yes, that sounds good. Proceed" authorizes round 27's starter encounter. The local implementation replaces instantaneous hits on both sides.
Two automatic kinetic turrets visibly launch traveling rounds with muzzle flash/recoil. Contact resolves shield, armor and hull damage.
Standard rounds drain Sentinel's shield but fail against its reinforced armor. The ship stops new ineffective launches after a blocked hit.
Practice ammunition can switch to piercing rounds, which damage the armored hull. No manual target lock or trigger is required.
Already launched rounds keep their trajectory and ammunition; Hold fire blocks new launches only.
Both sides have one simple shield. Shields do not recharge; ammo is unlimited within this temporary practice exercise.
Prototype ammunition choices are test loadouts, not a progression system or approval of exact starter inventory.
Fixed steps, swept collision, bounded reusable projectile/effect pools and quality-independent combat support the first slice.
Specific values are implementation choices, not final balance. Missiles, counters and the upgrade tree remain future work.
See [architecture](architecture.md) and [verification](../Docs/Testing/projectile-combat-verification.json).

Round 29: owner reports that projectiles and the existing prototype work well. This is owner playtest acceptance of the preceding encounter.
Owner also requests automated battleship movement driven by decisions, retaining but hiding manual controls.
First helm orders cover destinations, holding position, maintaining 35 km combat distance and withdrawing to 85 km.
Those distances are provisional practice behavior. Broader enemy tactics remain open.
Weapons remain independently automatic; a navigation order does not change the firing mode.

## Recommended combat rules

These are proposals supporting the owner's direction, not additional approved mechanics.

1. **Damage follows arrival.** Launch creates a traveling munition; damage occurs only on contact or a valid detonation.
   Automatic aim predicts motion, but never guarantees a hit. A cosmetic tracer cannot hide instant damage.
   Balance travel time against combat distance and movement, rather than literal interstellar travel speeds.
2. **Guidance has limits.** Give missiles finite speed, turning ability, seeker range and lifetime.
   Proposed lost-lock baseline: keep the last flight direction without further target updates.
   Last-known-position flight and later reacquisition can be explicit seeker variants; never silently grant perfect tracking.
3. **Helm orders guide the ship.** Explicit hold, range and withdrawal orders govern movement.

4. **Weapons need roles, costs and counters.** Define suitable targets, travel behavior, mounting limits and a weakness for every family.
   Higher levels improve capability without removing every counter. Even fast kinetic weapons must preserve useful travel at engagement range.
5. **Turrets respect their mounts.** Tracking speed, firing arcs and hull obstruction shape positioning.
   Mixed loadouts should be useful. Heavy offensive mounts, small defensive guns and launchers need not share identical requirements.
6. **Automation distributes effort.** Defensive weapons choose incoming threats; offensive weapons choose appropriate hostile ships.
   Account for shots already in flight to limit waste. Hold fire stops new launches; existing projectiles continue.
7. **Defense offers different builds.** Keep the starter shield simple. Later shield sustain, armor and interception offer competing strengths.
   Shields-first damage is the prototype baseline; exact armor, bypass, splash and subsystem rules require discussion.
8. **Both sides share rules.** Enemies use the same physical and equipment limits.
   Bridge/exterior views and graphics quality must not alter simulation outcomes.

## Accepted weapon families

Owner accepts these families as a starting direction. Detailed fictional roles, counters, effects and numbers remain proposals.

| Family | Proposed role and visible behavior | Main weakness or counter |
| --- | --- | --- |
| Autocannons | Reliable streams or bursts of small kinetic rounds; starter general-purpose guns | Travel and limited heavy-armor pressure |
| Heavy kinetic cannons | Fewer, larger solid projectiles with forceful shield and armor impacts | Slow reload and tracking; maneuver between shots |
| Flak / point defense | Small rounds or timed bursts against incoming missiles and light craft | Short range, limited coverage and saturation |
| Unguided rockets | Explosive salvos aimed at predicted positions or an area | No course correction; escape the salvo's path |
| Guided missiles | Traveling threats that adjust course within seeker and turning limits | Decoys, jamming and interception |
| Heavy torpedoes | Large, readable capital-ship threats with powerful impacts | Slow approach, limited ammunition and interception |
| Disruption rounds | Delivered payload temporarily pressures shields, sensors or other systems | Specialized effect, limited hull damage and recovery time |
| Nuclear / siege warheads | Later large projectile payloads with major spectacle and a broad danger area | Scarcity, long reload, interception and unsafe blast proximity |

Point defense is a role fulfilled by projectile guns or interceptor missiles, never an exception permitting ordinary hitscan.
Actual flak bursts and explosion radii would participate in hit resolution; visual effects alone do not inflict damage.

## Cinematic weapon requirements and proposals

Confirmed: all weapon use must remain visually evident, including a basic autocannon's traveling bullet stream.
Starter weapons already need satisfying firing and impact feedback; higher levels increase spectacle without introducing the first visible effects.
Representative visible rounds are allowed when performance requires them. This is not permission to remove projectile travel or damage checks.

Recommended visual chain: mount motion/recoil and muzzle flash; traveling projectile or trail; a response at contact.
Autocannons use separated bright streaks that visibly progress along trajectories, rather than a continuous instant beam.
Heavy rounds emphasize recoil and chunky projectiles; rockets/missiles use distinct launch flashes and trails; torpedoes have readable heavy impacts.
Use visual size and brightness for readability at the ship's camera distances without inflating collision sizes or damage areas.
Effects should correspond to actual firing cadence and projectile progress; do not invent extra damaging shots to thicken a stream.

Prefer reducing secondary sparks, smoke and debris before losing the firing stream, incoming threats or impact verdict.
Where fewer rounds are drawn, preserve the combat simulation and show a coherent subset across the burst.
No quality setting may change damage, accuracy, interception or simulation outcomes. No performance budget has been verified for this design.
Higher-level effects can grow in rhythm, trail richness, recoil and impact detail without covering the battle in glare.
Sound can reinforce the identity, but cannot replace the required visible firing. Exact art and sound remain undecided.

## Defense compatibility and complete protection

Confirmed: some weapon/defense pairings should deal zero damage. This is stronger than a small resistance modifier.
The separation below is accepted as a design principle; exact arithmetic and matchup values are not finalized:

1. Give each attack a damage mechanism, penetration capability and damage amount. Keep these separate from cosmetic spectacle and weapon level.
2. Give each defense layer explicit compatibility, protection thresholds and any remaining resistance.
3. At impact, resolve the layer actually struck. An incompatible attack or failed penetration can deal exactly zero to that layer.
4. A compatible attack that penetrates applies the remaining reduction and damage rules. Exact arithmetic and overflow remain undecided.

Proposed examples, not physical claims or settled balance:

| Attack and defense | Proposed result | Tactical implication |
| --- | --- | --- |
| Light kinetic round against hardened armor above its penetration capability | Zero armor/hull damage | More identical shots do not accumulate hidden chip damage |
| Armor-piercing heavy round against armor within its penetration capability | Can damage the protected ship | Fit a suitable weapon or ammunition type |
| Disruption payload against electronics fully protected against that effect | No disruption effect | Another attack mechanism is needed |

Protection is scoped to the relevant layer/effect, not automatically the entire ship.
A round might damage an outer shield but fail against armor after the shield falls; resolve and display those results separately.
If complete protection applies, do not quietly add minimum damage, armor wear or saturation that bypasses it.
Any later erosion, bypass or disabling interaction must be explicit, with a suitable attack mechanism and clear player feedback.
Upgrading raw damage or firing rate does not automatically change compatibility or penetration; specific equipment upgrades can change those properties.
Avoid making every high-level ship immune to all lower-level gear purely through a global level comparison.

Recommend clear combat feedback: shield absorption, blocked/no penetration, resisted damage or penetration.
Blocked hits remain cinematic through sparks, deflection or shield ripples, without showing a damaging hull explosion or decreasing hull health.
Scanning or observed hits should reveal relevant protection; unknown defenses should stay visibly unknown rather than being silently classified.
Automatic weapons should use known effective equipment/targets and report when no fitted weapon can damage a target.
Player options then include another target, a compatible weapon, help from another craft, refitting or withdrawal.
Do not require manual aiming to exploit these matchups. Targeting policy, discovery and possible subsystem weaknesses remain proposals.

## Progression proposal

Separate three understandable upgrade tracks:

- **Ship capacity:** available mounts, reactor output, cooling, ammunition storage and defense support.
- **Turret development:** each mount's tracking, reliability, reload and handling of compatible weapon classes.
- **Installed equipment:** actual weapon, payload, shield, armor, and countermeasure choices.

Weapon upgrades can increase damage or cadence, but also open specialized roles. Turret levels and weapon selection remain distinct choices.
Capacity creates loadout tradeoffs: thirty mounts should not imply thirty strongest guns firing without limits.
Power, heat and ammunition are recommended constraints; their complexity and logistics are not settled.
An upgraded ship can have a durable shield build, armor-focused build or avoidance build without making each universally best.

Growth must preserve deliberate battles at both ends. More guns require corresponding defensive options and readable salvo pacing.
The starter's few shield hits cannot become one late-game frame containing dozens of unavoidable impacts.
No fixed damage, projectile speeds, shield counts, unlock levels, acquisition costs or playtime commitments are approved.

## Next discussion after the first slice

The approved two-turret practice encounter now demonstrates shield failure, blocked hits and piercing rounds.
A basic incoming missile could then prove guidance loss or interception in a separate small step.
That follow-on remains a proposal, not work performed or authorization to build the entire system.

Open choices: tuning the first defense matchup and feedback; starter shield recharge;
mount compatibility and ultimate cap; missile lost-lock/reacquisition behavior;
shield versus armor tradeoffs; ammunition and heat complexity; combat/cruise interaction; friendly fire and splash;
repair, persistent damage, defeat/ship loss, research and acquisition.
No publication, transfer, paid generation or research submission is authorized by this discussion.
