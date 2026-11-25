# Aquila native foundation

Single-player Unreal proof of the browser design. This is an early blockout, not final art or a commercial release.
Launcher **Unreal 5.8.4**, changelist **58902052**, at `C:/Program Files/Epic Games/UE_5.8`.
Do not change the engine association to DuskForge's source engine or reuse its project configuration.
Windows is the primary development and playtest target. Keep the Galaxy disconnected during routine development;
batch actual phone checks at playable milestones, after relevant mobile changes or for phone-specific bugs.
Android compilation and desktop touch-layout previews need no USB connection. They cannot establish phone performance or touch comfort.

## Build and run

From the Aquila Code Dark repository root, in PowerShell:

```powershell
pwsh -NoProfile -File scripts/native.ps1 -Action Check
pwsh -NoProfile -File scripts/native.ps1 -Action Build
pwsh -NoProfile -File scripts/native.ps1 -Action Prepare
pwsh -NoProfile -File scripts/native.ps1 -Action Test
pwsh -NoProfile -File scripts/native.ps1 -Action Run
```

`Prepare` builds the hulls and fitting sockets, four turret meshes, six defense models, three ammunition models and their materials. The wrapper uses Blender on the path or at the standard Blender 5.2 installation. With local proportion studies it exports those hulls. The original loft recipes can also rebuild their geometry.

Ship finishes come from the sibling Asset Pipeline checkout at `Assets/AssetPipelineLab/Aquila_Research_Battleships/R003`. To use another local copy, point `AQUILA_SHIP_FINISHES` at the directory containing `delta-tree/textures` and `glacier-tongue/textures`. Prepare reads 42 painted maps and builds eight Delta Tree surfaces and seven Glacier Tongue surfaces. It can reuse previously prepared textures when the library is offline. The packaged game carries its own textures. Repeat preparation after changing geometry, sockets or materials. [Playable surface checks](../../Docs/Testing/native-ship-finishes.json).
Generated content, build output, IDE files, logs and saves stay ignored. No downloaded art or third-party project code is included.
Basic geometry and fonts come from the installed engine; Epic's licence governs those runtime dependencies.
The wrapper checks exact engine version, installed-build marker and Epic Launcher registration before invoking it.
Build closes no windows. It refuses while an Aquila editor process is open; other projects are untouched.
Use the wrapper rather than choosing another Unreal version in an IDE or file-association dialog.

```powershell
pwsh -NoProfile -File scripts/native.ps1 -Action Package
pwsh -NoProfile -File scripts/native.ps1 -Action Check -Platform Android
```

Windows Development package destination: `.runtime/native-package/Win64/` at the repository root; start `Aquila.exe` there.
Android ASTC Development package: `.runtime/native-package/Android/Aquila-arm64.apk`.
The package action explicitly packages after cooking, then verifies embedded game content and the APK signature.
This is a local development build, not a store release. Earlier foundation Galaxy installation, rendering and input checks passed.
The first-expedition build packages for Android but has not been installed or played on the Galaxy yet.
The connected device reports model SM-S948U, Android 17/API 37 and arm64-v8a; no retail-model inference is required.
ADB-injected taps and gestures exercised navigation, bridge movement/look, ammunition changes, pause and saves on the physical phone.
Background/return and process-stop/relaunch retained the exact saved campaign, including live rounds and earlier revisions.
The owner accepted the earlier enlarged buttons and labels. New expedition controls, multitouch, prolonged performance, heat and battery remain unverified.
Android target support was added through Epic Launcher. Aquila's SDK lives in ignored `.runtime/android-sdk/` at the repository root.
It contains SDK 36, build tools 36.0.0, NDK 27.2.12479018 (r27c), CMake 3.22.1 and platform tools.
Command-line tools build 15859902 reports version 22.0; its downloaded ZIP matched Google's published SHA-256.
The wrapper selects these paths and existing OpenJDK 21.0.8 only for its process and children.
Android user state and Gradle caches also stay under this repository's ignored `.runtime/`; no global SDK settings are changed.
Installed 5.8.4 metadata selects these SDK versions; its older setup batch names build tools 36.1.0.
Turnkey recognizes r27c but still marks host prerequisites invalid because Android Studio is absent.
The command-line build and cooked APK packaging pass with the explicitly selected SDK. Check the receipt for exact evidence.
The wrapper's `Check` action verifies required files, not installability, phone performance or store readiness.
For tool recovery, use Google's [command-line setup](https://developer.android.com/tools/sdkmanager) and the engine's installed version metadata.
Never repoint DuskForge or global toolchains. Android File Server is disabled; no generated authentication token belongs in source.
iOS requires future Mac/Xcode/signing access and an actual Apple test device. Neither is currently available.

## Play

On Windows, choose **Play Match**, **Combat Simulator** or **Boarding Test**. Play Match opens the existing expedition and restores its local journal. The Windows command panel has a **Main menu** button that saves the match before returning. Touch devices still open the expedition directly.

### Glacier Tongue boarding test

Open **Aquila Boarding Test** on the Windows desktop, or choose **Boarding Test** in this build’s main menu. The shortcut uses `-AquilaMode=Boarding`. In the editor, use `scripts/native.ps1 -Action Run -Mode Boarding`.

On either test ship, **WASD** moves at up to **13.5 m/s**, five times the original walking speed. Mouse look turns three times as far horizontally and vertically, both on foot and in the exterior orbit. [Windows input measurements](../../Docs/Testing/native-boarding-controls.json).

Start at Peregrine’s open hatch with the **1,280 m** Glacier Tongue. Look past the crew doorway toward the bay roof and press **B** to compare the **640 m** hull. Your feet, aim and field of view stay fixed on the ramp. The current length appears beside the ship’s name.

The landing pad is **142 × 180 m** on the larger hull and **71 × 90 m** on the smaller hull. Peregrine spans 64 m in both. Press **Tab**, then **V** for the view beside the shuttle. Press **B** there to see the room beside each wing change from 39 m to 3.5 m.

Walk down the ramp, follow the deck arrows and pass through **Airlock 01**. The two lift doors mark the end of this route. Both airlock frames are open for the trial.

| Control | Action |
| --- | --- |
| W, A, S, D and mouse | Walk and look |
| N | Board the other ship, or compare ships in the current exterior view |
| B on Glacier Tongue | Switch hull size on foot or in either exterior view |
| Tab | Switch between the exterior orbit and the same spot aboard |
| Mouse and wheel outside | Orbit and zoom |
| V outside | Frame the shuttle berth or the whole hull |
| R | Return to the shuttle ramp |
| Esc | Return to the main menu |

The whole-ship camera adjusts its distance to frame either hull. The berth camera holds its position beside the 68 m shuttle. R restarts the walk at the selected size. A fresh boarding session opens at 1,280 m. This trial has its own player and collision surfaces. [Windows size comparison](../../Docs/Testing/native-glacier-scale.json) and [first boarding walk](../../Docs/Testing/native-glacier-boarding.json).

### Delta Tree boarding test

Open **Aquila Boarding Test** and press **N** for Delta Tree. **Tab** frames the whole hull at combat distance. **V** outside frames the round terminal and Peregrine. **Tab** returns to the ramp. Walk straight down the ramp, cross **Arrivals 06**, pass through the two **Branch Airlock** frames and reach **Junction 01**. Its arms lead to the research and command doors. **R** replays the walk. Press **N** to compare Glacier Tongue from either orbit.

Delta Tree’s approved length is **1,280 m**. Peregrine measures 68 × 64 × 16 m aboard either ship. The desktop shortcut normally opens Glacier Tongue. A direct Delta Tree launch uses `-AquilaMode=Boarding -AquilaBoardingShip=DeltaTree`. [Submitted choice](../../reports/game-direction/inbox/20261125T012507Z.json) and [Windows playtest](../../Docs/Testing/native-delta-boarding.json).

### Delta Tree command bridge

Turn right at Junction 01 and follow the blue line into the Command lift. Press **F** inside the car. At the upper landing, walk into the circular bridge. The forward windows overlook the ship’s cannon and a shielded training drone.

Delta Tree opens fire on its own, even while you are still downstairs. Watch rounds cross the bow and flash against the shield. You can walk around and look freely. The shield recharges after ten hits, and the exercise repeats. Both views show the same rounds and target. [Packaged Windows checks](../../Docs/Testing/native-bridge-observer.json).

### Combat Simulator on Windows

On the playtest PC, double-click **Aquila Combat Simulator** on the Windows desktop.

Fitting, the exterior duel and Delta Tree’s bridge all use the boarding hulls. Look down from the bridge for Delta Tree’s bronze branches. Glacier Tongue has an open bay below its curved roof. Each gun base follows the slope at its numbered port. To start directly at the glass at normal speed, launch with `-AquilaMode=Simulator -AquilaDuelAboard`. [Windows hull and mount checks](../../Docs/Testing/native-duel-hulls.json).

Choose **Configure ship**, fit either ship, then click **Start duel aboard**. The fresh duel starts at 1× on Delta Tree’s bridge. Press **N** to walk on Glacier Tongue’s command deck. Press **N** again to return to your last position and heading on Delta Tree. **Tab** takes you outside and back to the last surviving bridge you visited. **Watch from bridge** also rejoins a running or paused fight. Both crews keep firing. Each window sill flashes cyan for its own shield and amber for armor or hull hits. Losing the ship you are aboard returns you to the exterior. **Replay aboard** starts a fresh fight with both fittings. For direct fitting entry, use `-AquilaMode=Simulator -AquilaDuelSetup`. [Two-bridge playtest](../../Docs/Testing/native-two-bridges.json) and [original bridge duel](../../Docs/Testing/native-bridge-duel.json).

The panel shows the fitted gear under each ship’s name. By default, Delta Tree fires standard rounds from twin burst turrets and has light plating. Glacier Tongue fires standard rounds from one kinetic cannon and has reinforced armor. Both start with an energy shield.

Choose **Configure ship** and select Delta Tree or Glacier Tongue. **Clear fitting**, beside their names, strips the selected ship immediately. All 30 ports empty. Turret bodies return to that ship’s stock with standard rounds and no chips. Armor, shield and Reserve Capacitor come off. You can clear either ship while the duel is running, paused or over. **Cancel** closes fitting. When you reopen it, the ship is still bare. [Windows fitting checks](../../Docs/Testing/native-live-fitting.json).

Four distinct turret models turn in place around the ship. Click a model, then click a numbered port on the hull. The selected turret appears on that port immediately. Click more ports to place the same type. Choose **Done placing**, or click the selected model again, to inspect ports. Cyan marks fitted ports. Amber marks the selected part.

Hover over a floating turret to see it fire at a miniature reinforced ship above the fitting hull. Click the turret to keep the example playing while you change ammunition. Hovering a different model temporarily shows that gun. Leaving it returns to the selected gun. **Done placing** releases the selection. The description scrolls toward the nearest screen edge, holds its final phrase for one second, then repeats.

The training shield lasts two hits from the selected gun. Standard rounds knock it down and stop on reinforced armor. Piercing rounds knock it down, then damage the hull. Phase rounds cross a charged shield and stop on the armor. The two plasma guns show their own bolts. A fitted interception chip also demonstrates an incoming round. The example repeats independently of the duel and its save.

A selected port supplies its ammunition and chip. Preview ammunition chosen for a different kinetic model or an empty port follows the next successful placement. Choosing ammunition for a matching fitted turret changes the rounds it will fire next. [Windows firing previews](../../Docs/Testing/native-firing-preview.json).

Each hull has 30 ports. Delta Tree starts with two turret bodies and Glacier Tongue with one. Refitting an occupied port keeps its ammunition and chip. Placing on an empty port uses a spare. With no spares, the port still opens. Choose **Move a turret here**, then click the source turret, or **Borrow full lab stock** to try 30 bodies. **Return turret to stock** frees a port. Chip and inventory actions sit below the ship while a numbered port is selected. [Windows fitting checks and views](../../Docs/Testing/native-turret-gallery.json).

For a burst turret or kinetic cannon, click a floating round or its name below the hull. **Standard** is a brass capsule, **Piercing** is a long silver dart, and **Phase** has violet shards inside two open rings. The selected round gets a marker, and the live turret example uses it. [Windows ammunition model checks](../../Docs/Testing/native-round-models.json).

Phase rounds cross a charged shield without draining it. Light and prism plating take the hit. Reinforced armor stops phase and standard rounds. Piercing rounds drain shields first and can damage reinforced armor. Plasma weapons carry their own bolts.

Click **Armor** on either hull, then click a rotating sample. Light plating has slim silver plates, reinforced armor has copper-edged ribs, and prism plating has violet facets. The hull immediately takes the selected finish. Prism plating stops plasma.

Click **Shield** to see the active shell. Fit the blue **Standard shield**, then optionally click the amber **Reserve Capacitor**. Click the capacitor again to remove it. **No shield** removes both pieces. On the first depletion each duel, the capacitor restores 100 shield points. Excess damage uses that charge first. The amber cells go dark when spent. Phase rounds bypass the shield and leave the reserve alone.

**Replay duel** rearms installed capacitors. [Windows defense checks](../../Docs/Testing/native-defense-fitting.json).

An **interception chip** lets a turret shoot incoming kinetic rounds, phase rounds and plasma bolts. A cyan shot and flash mark a successful interception. After the opposing ship explodes, the survivor’s chipped turrets keep engaging incoming rounds as their guns become ready. Rounds that arrive first can still damage or destroy the survivor. The **Intercepted** count records each successful defense. [Phase rounds and final-volley checks](../../Docs/Testing/native-phase-rounds.json).

Fitting holds the duel clock. Closing it preserves hull damage, rounds in flight and the previous pause state. **Start duel** and **Replay duel** begin a fresh fight using both ships’ current equipment, including empty mounts. [Windows fitting and interception checks](../../Docs/Testing/native-ship-fitting.json).

The duel starts at **8×** speed. Click the speed button for **1×** or use **Pause** to inspect a hit. A powered shield has a faint blue rim fitted to the hull. It flares where a shot lands. The last ripple fades when the shield fails. Gold sparks mark a deflection. A penetrating hit flashes orange against the plate. Either ship explodes when its hull reaches zero.

Under **Shield shape**, choose **Ship mesh** or **Oval**. Ship mesh scales the boarding hull to 108% around the ship’s pivot. Guns and fitted equipment appear once, on their own mounts. Switch shapes during combat or pause on a strike to compare the same shot. The Windows arena starts with Ship mesh. The expedition uses the oval. [Original shield comparison](../../Docs/Testing/native-mesh-shield-trial.json) and [current hull checks](../../Docs/Testing/native-duel-hulls.json).

Both speeds use the expedition's projectile and damage rules. **Main menu** discards the fight. Choosing Play Match afterward opens the saved expedition. The arena never loads or writes its journal.

The [two-ship fitting checks](../../Docs/Testing/native-arena-loadouts.json) cover both sides’ equipment and shield shapes. The [original simulator verification](../../Docs/Testing/native-combat-simulator.json) records the initial ammunition outcomes.

For direct development launches, use `pwsh -NoProfile -File scripts/native.ps1 -Action Run -Mode Simulator`. The packaged executable accepts `-AquilaMode=Simulator` too. An explicit `-AquilaSaveDir` by itself retains the existing direct match launch. Add `-AquilaMode=Menu` or `-AquilaMode=Simulator` to enter that mode with a specific match directory.

Add `-AquilaPreviewShield` to a direct simulator launch to pause once at the first shield strike. **Resume** plays the rest of the duel normally.

### Match controls

Match flight and bridge views open with the entire interface hidden. Press **H** to show the command panel and **H** again to dismiss it.
The panel groups ship status, expedition, navigation, tactical, view and save controls. Opening or closing it preserves the simulation clock.
On touchscreens, touch the view with two fingers to reveal the controls. **Hide menu** clears the whole display.
Navigation, Weapons and Expedition use large touch tabs. Bridge, Pause/Resume and Hide menu sit above them when open.
Phone text uses a bold, bright face with a dark shadow; buttons have larger touch areas and device safe-zone margins.
The phone uses a 960×480 reference canvas for finger-sized controls. Windows centers its panel inside a 1600×900 reference canvas.
For an offscreen desktop layout preview, add `-AquilaTouchUI`; a preview is not physical-phone input evidence.
The main exterior view is a free RTS observer. It opens near Delta Tree, facing the surrounding space. The ship keeps flying under its own helm while the camera stays wherever you leave it.

| Windows control | Action |
| --- | --- |
| W / S | Fly forward or backward along your line of sight. Aim up to climb or down to dive. |
| A / D | Strafe left or right. Hold Shift with any movement key for faster flight. |
| Middle-drag | Pan across the horizontal plane. Panning or using a movement key releases a followed ship. |
| R / F | Raise or lower the observer. |
| Left- or right-drag | In free view, turn in place. While following, orbit the selected ship. |
| Mouse wheel | In free view, move along your line of sight. While following, move closer to or farther from the ship. |
| Click a ship | Follow that ship. Delta Tree and the active Glacier Tongue can both be selected. In follow mode, releasing a drag eases back behind the selected ship. |
| Click empty space | Release follow at the current viewpoint. Home or **Find Delta Tree** follows the flagship again. |
| C | Enter Delta Tree’s first-person bridge. WASD walks and dragging looks around. Press C again to return to the exterior view you left. |

The arena opens facing the midpoint between the two ships. It starts in free view. The exterior pointer allows selection while the command panel is hidden. Camera movement and orders are independent. Open **H** to give Delta Tree an order in a match, including while observing Glacier Tongue.
On touch, drag to pan and tap a ship to follow it. Dragging then orbits that ship. Tap empty space to release.
The touch menu’s **Camera** tab offers zoom, free-view rotation and a shortcut back to Delta Tree. On the bridge, the movement buttons walk.
Exterior observation and mouse look also work while paused. Resume the expedition clock with Escape or the Pause / resume button.
Escape or Pause stops simulation. Losing focus/suspending saves locally and pauses; explicitly resume on return.
Weapons acquire and shoot automatically. Standard rounds drain shields but bounce off Glacier Tongue's armor.
Select piercing ammunition to penetrate. Hold fire stops new launches while flying rounds remain live.
Practice can be reset outside an active expedition. It has unlimited ammunition and grants no salvage.

### First expedition: Vesper Echo

1. Choose **Investigate signal**, or press E. Delta Tree travels automatically to a seeded signal 500 km away.
2. Within 8 km, a 90-second survey reveals an armed contact. Moving away pauses the survey; the expedition button resumes the route.
3. Weapons acquire and fire automatically. Choose positioning orders, hold fire or ammunition; standard rounds cannot penetrate armor.
   Expedition damage is paced for a deliberate engagement. Shot travel, contact tests and defense compatibility remain unchanged.
4. After the target is disabled, choose **Recover salvage**. Within 6 km, drones recover the wreck over 45 seconds.
5. Completion grants 120 salvage once. **Refit port turret / 100**, or U, purchases Mk II: +50% impact damage on that turret.
   Starboard stays Mk I, 20 salvage remains, and the refit cannot charge twice. The port mount gains a visible cyan panel.
6. Save or close, then reopen to retain the mission, rewards and refit. **Next signal** retains gains; failed missions can be retried.

Hold and withdrawal remain player decisions. Practice cannot interrupt an active expedition or erase its progress.
The automated baseline completes in 547.35 simulated seconds (9.12 minutes), without menu decisions or pauses.
The design target remains roughly 10–15 minutes of player time; human pacing needs a playtest.
The prototype offers one repeatable encounter and one upgrade. The planned [standalone match](../../knowledge/game-direction.md#standalone-matches) pits the player against an expanding computer rival.

## Design boundaries

- Gameplay positions use meters. Exterior rendering uses 0.1 cm per meter and the observer’s focus as its origin.
  The separate bridge uses ordinary centimeter dimensions; its interior need not fit the exterior exactly.
  This backdrop and bounded navigation do not yet port the browser's infinite sectors, shuttles or planet descent.
- Combat uses a 120 Hz fixed step, bounded catch-up, swept contacts, at most 160 rounds and 64 recent cosmetic contacts.
  Visual quality does not change damage. Two burst turrets, ballistic enemy fire and shield/armor compatibility are present.
  Shield, plating and destruction effects follow presentation time at either duel speed. Each of the four guns has its own fitted mesh. Recoil remains open.
- Desktop and touch command widgets are authored in Slate. The Windows camera uses relative mouse motion beneath the command buttons.
  The earlier Galaxy input checks cover the foundation. Physical touch on the new expedition remains open.
- Owner's first phone playtest found menu text too small. The larger layout is installed; owner confirms reading and tapping comfort.
  Actual phone checks caught clipped labels: movement buttons now fit Forward, and the hidden-menu button reads Menu.
- Shared rendering baseline disables Nanite, Lumen, virtual shadow maps and ray tracing.
  Desktop captures do not prove mobile frame rate, heat, battery use, memory stability or store readiness.

## Local save contract

Campaign revisions are UTF-8 JSON under Unreal's project `Saved/Campaign/` location.
Each commit writes a unique pending file, then renames it to an immutable revision. Earlier revisions are retained.
Version 3 stores world seed/generator version, ship/enemy state, navigation, ammo/modes, victory count and flying rounds/clocks.
It also records expedition identity/phase, signal, survey/recovery timers, claimed completion, salvage, port upgrade and each shot's launch damage.
Reward/completion and upgrade/debit each share one revision. Phase changes and commands save immediately; ordinary autosave remains every 30 seconds.
Profile/campaign and revision identifiers are UUIDs; parent links establish ancestry independently of device timestamps.
No credentials, graphics preferences or device controls are stored in campaign data.
Version 1 migration supplies safe defaults for missing projectile clocks. Versions 1/2 retain combat and start with no expedition rewards or upgrades.
Newer, malformed or unsupported saves never mutate live state.
Loading a corrupt revision or divergent tips pauses automatic writes and retains all files for review.
Conflict-selection UI, journal retention/compaction, account linking and cloud transport are not implemented.
No upload, cross-device sync or store ownership is implied by a local save. The UI says cloud is not connected.
Pending-file recovery tests do not establish survival under physical power loss or damaged storage.

## Evidence and repeatable captures

See [native verification](../../Docs/Testing/native-foundation-verification.json) for the exact checks and remaining gaps.
The [mobile UI receipt](../../Docs/Testing/native-mobile-ui-verification.json) records the installed layout, input checks and owner feedback.
The native automation groups cover combat travel and defenses, ammunition switching, timing, navigation, migration and save conflicts.
Expedition groups cover completion, commands, failure, rewards and upgrade damage. A separate contact test checks shield overflow, near-side contact and one destruction event per ship.
See the [expedition receipt](../../Docs/Testing/native-expedition-verification.json) for current builds, isolated captures and evidence limits.
Phone testing caught older standard rounds setting the armor block after piercing ammunition was fitted.
Blocked impacts still do no hull damage, but cannot stop the current piercing loadout or show an incorrect refit instruction.
Earlier saves with the stale flag also continue firing correctly; the save format is unchanged.
Tests use unique directories under `Saved/Automation/`, preserving player saves.
Render captures use Unreal's screenshot request with `-AquilaCapture=<absolute PNG path>` and `-AquilaExitAfterCapture`.
Offscreen captures need `-ForceRes` alongside `-ResX`/`-ResY`; verify actual PNG dimensions before judging UI scale.
Use `-AquilaSaveDir=<isolated directory>` to separate evidence runs from player saves.
The packaged Windows test `Aquila.Input.CombatFeedback` runs with `-AquilaDuelTest -AquilaMode=Simulator` and an isolated save directory. Add `-AquilaFeedbackFrames=<output directory>` to capture the duel viewport at 24 frames per second around shield contact, plating hits and both defeats. [Combat feedback evidence](../../Docs/Testing/native-combat-feedback.json).
Optional `-AquilaDemo=bridge` or `-AquilaDemo=combat` selects a capture scene through code, not actual mouse/touch input.
Those switches are development conveniences, not proof of input-device operation.
The entry map can offset the pawn at spawn: cameras and scene geometry must share pawn-local coordinates.
Early scene commands cannot save until the game mode has selected its save directory.
