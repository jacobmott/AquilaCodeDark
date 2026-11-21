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

`Prepare` generates three local materials from the authored C++ commandlet. Run it after changing that generator.
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

Use the glass command strip to approach Vesper, hold, keep combat range or withdraw.
On phones, larger Navigation, Weapons and Expedition tabs show one group of commands at a time.
Bridge, Pause/Resume and Hide menu/Commands stay available at the top. The menu can hide during viewing or walking.
Phone text uses a bold, bright face with a dark shadow; buttons have larger touch areas and device safe-zone margins.
The phone uses a 960×480 reference canvas for finger-sized controls. Windows uses a 2304×1296 canvas with a centered command strip.
For an offscreen desktop layout preview, add `-AquilaTouchUI`; a preview is not physical-phone input evidence.
On Windows, left-click a command to give the helm an order. The ship steers automatically.
Hold either mouse button over the ship or stars and drag sideways to see the ship from the side. Drag vertically to tilt.
The pointer hides during a drag. The hint reads **Orbiting** outside or **Looking around** on the bridge.
Release to use the pointer again. The exterior view waits briefly, then eases behind the ship. The bridge keeps its heading.
Mouse look also works while paused. The expedition clock resumes with Escape or the Pause / resume button.
Enter the bridge with its button or C. Walk with WASD or hold the on-screen movement buttons; drag to look.
Escape or Pause stops simulation. Losing focus/suspending saves locally and pauses; explicitly resume on return.
Weapons acquire and shoot automatically. Standard rounds drain shields but bounce off Sentinel's armor.
Select piercing ammunition to penetrate. Hold fire stops new launches while flying rounds remain live.
Practice can be reset outside an active expedition. It has unlimited ammunition and grants no salvage.

### First expedition: Vesper Echo

1. Choose **Investigate signal**, or press E. Resolute travels automatically to a seeded signal 500 km away.
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
This is one repeatable encounter pattern and one upgrade, not a complete campaign or economy.

## Design boundaries

- Gameplay positions use meters. Exterior rendering uses 0.1 cm per meter and a ship-relative origin.
  The separate bridge uses ordinary centimeter dimensions; its interior need not fit the exterior exactly.
  This backdrop and bounded navigation do not yet port the browser's infinite sectors, shuttles or planet descent.
- Combat uses a 120 Hz fixed step, bounded catch-up, swept contacts, at most 160 rounds and 32 cosmetic impacts.
  Visual quality does not change damage. Two burst turrets, ballistic enemy fire and shield/armor compatibility are present.
  This is an early effect/geometry blockout; recoil, detailed turrets, shield shells and finished effects remain later work.
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
Eleven native automation groups pass: combat travel/defenses, ammunition switching, timing/hold, navigation, migration and conflict/interruption;
plus expedition completion, commands/failure, phase restoration/rewards, upgrade impacts/frame-rate parity and version-2 migration.
See the [expedition receipt](../../Docs/Testing/native-expedition-verification.json) for current builds, isolated captures and evidence limits.
Phone testing caught older standard rounds setting the armor block after piercing ammunition was fitted.
Blocked impacts still do no hull damage, but cannot stop the current piercing loadout or show an incorrect refit instruction.
Earlier saves with the stale flag also continue firing correctly; the save format is unchanged.
Tests use unique directories under `Saved/Automation/`, preserving player saves.
Render captures use Unreal's screenshot request with `-AquilaCapture=<absolute PNG path>` and `-AquilaExitAfterCapture`.
Offscreen captures need `-ForceRes` alongside `-ResX`/`-ResY`; verify actual PNG dimensions before judging UI scale.
Use `-AquilaSaveDir=<isolated directory>` to separate evidence runs from player saves.
Optional `-AquilaDemo=bridge` or `-AquilaDemo=combat` selects a capture scene through code, not actual mouse/touch input.
Those switches are development conveniences, not proof of input-device operation.
The entry map can offset the pawn at spawn: cameras and scene geometry must share pawn-local coordinates.
Early scene commands cannot save until the game mode has selected its save directory.
