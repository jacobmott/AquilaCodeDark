# Aquila matches, Combat Simulator and Windows playtest

2026-11-22 local, round 121. The first Windows duel, its tests and launch instructions reached `origin/main` in `dfe5c02`. The simulator plan is in `3e66124`. [Verified publication](../Testing/native-combat-simulator-publication.json) and [copyable restart prompt](PROMPT_2026-11-22_combat-simulator.txt).

## Windows duel

On the playtest PC, double-click **Aquila Combat Simulator** on the Windows desktop. Normal Windows startup also offers **Combat Simulator** beside **Play Match**. Resolute and the armored Sentinel use the expedition's automatic turrets, projectiles and damage rules. Choose standard or piercing ammunition, replay from the same starting state, pause, or switch between 1× and 8× speed. Leaving discards the arena. Play Match restores the expedition. [Controls and direct launch](../../native/Aquila/README.md#combat-simulator-on-windows).

The chat launch link failed for the owner. The desktop shortcut opened the standard duel in a responsive Windows window. The subsequent game log recorded both ammunition types and a normal exit. The game was closed at wrap.

Epic Launcher Unreal 5.8.4 CL 58902052. Eight packaged Windows input runs and eleven simulation/save groups passed. Damage matched an expedition battle frame by frame. Standard rounds exhausted Sentinel's shield and left its hull at 100. Piercing rounds disabled Sentinel after 220.97 simulated seconds on both attempts. The direct duel kept both saved test revisions byte-for-byte intact. Play Match restored the full saved fixture. The wrap rechecked hashes for eight native sources, the package, the test reports and two rendered views. [Duel verification](../Testing/native-combat-simulator.json).

The agreed simulator lets players try every ship and compatible loadout before acquiring it. Show where to earn each item. Developers can check combat without starting a universe. One pairing and two ammunition types are playable. Additional ships, armor fitting, weapons and acquisition information remain open. [Simulator design](../../knowledge/game-direction.md#combat-simulator).

## Match and observer

A fresh standalone, real-time match starts with one homeworld and Resolute against one expanding computer rival. Ships and worlds keep working while attention moves elsewhere. Ships can be replaced, including Resolute. Settling and defending additional worlds extends a civilization's reach. The exact defeat rule remains open. [Standalone matches](../../knowledge/game-direction.md#standalone-matches).

Save points belong to their run and restore both sides. A different generated universe per run remains proposed. The current Windows prototype restores one expedition with a fixed default seed. Full fleet construction, rival expansion, planet development and run selection remain open. [Decisions](../DECISIONS.md) and [backlog](../BACKLOG.md).

The free Windows observer can follow either ship. Home finds Resolute, C visits its bridge and H shows the hidden command panel. The owner previously confirmed exterior mouse orbit. Camera feel, desktop sizing and full human expedition pacing remain open. Windows is primary. Physical Galaxy checks happen at milestones. Sustained mobile performance, cloud saves and Apple packaging remain open. [Observer verification](../Testing/native-rts-observer.json).

## Local state and authority

A read-only check at 03:19 UTC on November 23 found expedition save 151681 and verified parent 151680. The expedition recorded **Failed**, helm **Hold**. Recheck the current save before acting. Preserve the full journal. Launch owner sessions without injected input. Earlier held input caused a defeat and retry. [Incident](../Testing/native-command-panel.json) and [restart verification](../Testing/native-windows-restart.json).

The package, shortcut, playtest media and live journal remain on this PC. Report 8953 and Research Console 8776 were listening. Report 8952 and preview 4200 were inactive. Last real inbox read was `reports/game-direction/inbox/20261115T015927Z.json`. Later decisions came through chat. The content push left the working tree and shared staging clean.

D&B waits for owner direction. The owner chose an LLC with a dedicated business address. Keep the home address private. Favor dependable mail handling. Northwest leads and Regus Plaza 7000 is a conditional fallback. Four developers list the same Austin suite on Google Play. Zensoul names Northwest in Wyoming. Those examples establish neither private verification nor this LLC's eligibility. No provider, package or filing date was chosen. D&B's chat produced no specialist referral or case. Before an owner-requested retry, read the [publishing reference](../../knowledge/publishing-business.md) and [saved D&B message](PROMPT_2026-11-18_dnb-follow-up.txt).

This wrap adds the receipt, restart prompt, checkpoint, archive and round 121. The owner authorized this session wrap, commit and push. That approval ends with this wrap. New contact, purchase, filing, signup, generation, publication, transfer or deployment needs fresh approval.

[Exact round 120 checkpoint](../History/game-direction/20261123T032221Z.md) and [hash record](../History/game-direction/20261123T032221Z.json). The archive links earlier checkpoints and publication receipts.

## Restart

Use the [restart prompt](PROMPT_2026-11-22_combat-simulator.txt). Do not start any of this. Read, say where things stand, then wait.
