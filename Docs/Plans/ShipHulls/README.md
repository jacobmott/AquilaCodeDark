# Research battleship roster

All 60 numbered designs are research battleships a player can own. A fresh match starts with one. Exploration can reveal another ship, and research can unlock one. Other routes, exact requirements and the starting hull are open. Each ship keeps its generated gallery name.

## The first two model studies

| Ship | What identifies it | Life aboard |
| --- | --- | --- |
| **58 · Delta Tree** | Branching channels reach round docks across a shallow fan. An observation dish rises ahead of stacked drives. | A route from an outer dock could follow a branch past the labs to the central bridge. |
| **41 · Glacier Tongue** | Undulating silver and blue plates overhang a recessed dock. Sealed crossings reach a narrow outboard plate. | The recessed bay could open into a boarding gallery, with crew quarters across the crossings. |

The gallery spells ship 41 **Glacier Tongue**. The prototype uses Delta Tree for the player and Glacier Tongue for the opponent. That temporary pairing leaves the match starter open. Both editable Blender scenes include the original concept. Both 3D exports reopened with 30 mount guides. Playable boarding routes and working gun mounts came later on the revised hulls. [Model studies and source references](<D:/OtherProjects/asset-pipeline/Docs/Features/AQUILA_RESEARCH_BATTLESHIPS_20261124.md>) and [verification](../../Testing/research-battleship-studies.json).

### Textured exteriors

Asset Pipeline finished the materials on the first two exterior studies, before the hull proportions changed. Delta Tree’s warm ceramic plates meet bronze rails and dark drive housings. Its export uses 12 materials and 33 embedded texture maps. Glacier Tongue’s satin aluminum overlaps blue enamel around the lit dock. Its export uses 13 materials and 34 maps. Both packages include color, normals, packed occlusion/roughness/metallic, and emission. [Surface checks](../../Testing/research-battleship-materials.json) and [local textured assets](<D:/OtherProjects/asset-pipeline/Assets/AssetPipelineLab/Aquila_Research_Battleships/R003/>).

The **1,280 m Windows ships** use the revised boarding models. Delta Tree has ivory ceramic branches, bronze rails, dark drive housings and amber windows. Glacier Tongue has silver armor, blue enamel bands and lit enclosed crossings. The fitting room, duel and boarding routes share the new surfaces. Armor samples use the selected ship's plating, so fitting a sample changes the same panels seen in combat. [Playable finishes and checks](../../Testing/native-ship-finishes.json).

Both command bridges, the existing dock interiors and their equipment have materials. Crew quarters, labs and engineering rooms still need building. [Boarding and bridge playtest](../../Testing/native-two-bridges.json).

### Likeness to the chosen ships

The owner reviewed the textured Windows ships and found that only their broad shapes resemble the chosen designs. The playable exteriors come from the Blender studies built to test proportions and boarding. The original gallery paintings still define the ships. [Originals beside the current hulls](http://127.0.0.1:8953/#i-aquila-direction-r173-delta) and [comparison sources](../../Testing/native-hull-likeness.json).

- **Delta Tree.** The continuous raised trunk forks around leaf-shaped sunken decks and reaches circular docks. The bridge rises in tiers ahead of individual drive housings. The playable foredeck is a wedge with straight spokes and exposed cylinders.
- **Glacier Tongue.** Separate silver plates sweep over blue armor at different heights. A deep framed hangar opens beside enclosed crossings and a substantial outboard blade. The playable upper bands share a smooth contour. Thin rods lead to a narrow rail.

The proposed next review pairs each rebuilt hull with its original from the same direction, then checks the side and stern. Plain gray geometry should show the branches, drive housings, armor overhangs and hangar depth. The scale, shuttle clearance, seated guns and two working bridges remain constraints on a future playable revision.

### Rebuilt exteriors for review

Round 179. Delta Tree’s paired window decks flank the twelve drives. Across nine heights in the rear study, the average shoulder offset from the engine centerline fell from 96.48 to 0.59 m. Its lower shoulders still curve inward sooner than the artwork. Both review ships retain their 1,280 m length. Delta’s beam fell from 595 to 584 m and overall height from 467 to 416 m. All 108 exhaust samples reach inside the rims. All eleven deck recesses and 60 mast footing contacts remain.

Glacier Tongue’s berth hangs from two ribbed piers beneath a raised spine. The three enclosed crossings join both the hull and the blade. Nine of fourteen marked openings now show through the full ship, up from zero. Five aft rays first hit the outer blade. Its angle and length follow the primary painting. Dock sightline depth is 64.51 m. The lighting around the opening remains flatter than the side study. Both local scenes and their exchange models include the materials. The Windows duel uses round 172. [Geometry, materials and source records](../../Testing/research-battleship-stern-shoulders-and-dock.json) · [Reusable Blender workflow](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md#replay-the-stern-shoulders-and-suspended-dock>).

| Ship | Compare in 3D | Artwork, round 178 and round 179 | Editable scene |
| --- | --- | --- | --- |
| Delta Tree | [Rotate Delta Tree](http://127.0.0.1:8953/hull-review.html?ship=delta-tree&round=179) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r179/comparisons/delta-tree-full.png) · [stern](../../../Assets/ShipConcepts/hull-review-r179/comparisons/delta-tree-stern.png) · [window decks and service pods](../../../Assets/ShipConcepts/hull-review-r179/comparisons/delta-tree-detail.png) | [Delta Tree](../../../Assets/ShipConcepts/hull-review-r179/seated-galleries/delta-tree/delta-tree.blend) |
| Glacier Tongue | [Rotate Glacier Tongue](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue&round=179) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r179/comparisons/glacier-tongue-full.png) · [port side](../../../Assets/ShipConcepts/hull-review-r179/comparisons/glacier-tongue-side.png) · [berth and blade](../../../Assets/ShipConcepts/hull-review-r179/comparisons/glacier-tongue-detail.png) | [Glacier Tongue](../../../Assets/ShipConcepts/hull-review-r179/hanging-berth/glacier-tongue/glacier-tongue.blend) |

#### Comparing materials against the artwork

Round 181 adds separate camera-matching and hull-reconstruction briefs alongside the earlier materials request. The camera brief asks how to tell a lens or viewpoint error from an incorrect hull. The geometry brief asks how to recover curved shoulders and hollow spaces while keeping the crossings physically connected. Each asks for a small experiment with an independent check. [All three research requests](../../Research/research-battleship-reference-matching-requests-v1.json).

Round 180 uses the existing round 179 models. The owner says the ships look closer and asked how to compare textures and materials more precisely. Both sets already include lit and unlit finish samples. Delta’s upper deck uses one roughness value across its painted panels. Glacier’s dock is darker than the painted opening in a matched crop. A moving light over the actual finishes, followed by an emission-on/off comparison at the dock, would help distinguish paint, reflections, relief and illumination. The trials are proposed. [Measured examples](../../Testing/research-battleship-material-review-baseline.json) · [Reusable method](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md#compare-painted-and-rendered-materials>) · [Perplexity research request](http://127.0.0.1:8776/?project=aquila-code-dark#3e598ae1e05e4d48aa38438c52ebc298).

#### Round 178 comparison

Round 178 compares both ships with their original paintings and round 177. Delta Tree’s twelve exhausts now share one centerline in three level rows. Placing the mast on that line increased overall height from 356 to 467 m. Moving the drive housings increased beam from 553 to 595 m. The upper housing outlines average 8.44 pixels from ten marks, previously 18.86. Glacier Tongue’s two window galleries taper behind the silver armor. Seventeen independent window and plate marks averaged 0.96 pixels from the revised edges, previously 33.78. Both ships retain their 1,280 m length. Delta’s offset stern shoulders and Glacier’s heavy dock underside remain visible in secondary views. Windows uses round 172. [Reopened geometry, material checks and source records](../../Testing/research-battleship-stern-and-armor-seams.json) · [Reusable methods](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md#replay-the-drive-racks-and-port-armor>).

| Ship | Compare in 3D | Artwork, previous and revised | Editable scene |
| --- | --- | --- | --- |
| Delta Tree | [Rotate Delta Tree](http://127.0.0.1:8953/hull-review.html?ship=delta-tree&round=178) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r178/comparisons/delta-tree-full.png) · [upper drives](../../../Assets/ShipConcepts/hull-review-r178/comparisons/delta-tree-detail.png) · [stern](../../../Assets/ShipConcepts/hull-review-r178/comparisons/delta-tree-stern.png) | [Delta Tree](../../../Assets/ShipConcepts/hull-review-r178/seated-cradle/delta-tree/delta-tree.blend) |
| Glacier Tongue | [Rotate Glacier Tongue](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue&round=178) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r178/comparisons/glacier-tongue-full.png) · [armor and windows](../../../Assets/ShipConcepts/hull-review-r178/comparisons/glacier-tongue-detail.png) · [port side](../../../Assets/ShipConcepts/hull-review-r178/comparisons/glacier-tongue-side.png) | [Glacier Tongue](../../../Assets/ShipConcepts/hull-review-r178/layered-port/glacier-tongue/glacier-tongue.blend) |

#### Earlier exterior comparisons

The owner marked both round 176 ships closer. Round 177 compares the original, that model and its successor at the same camera. Delta Tree’s eleven deck recesses have smooth sides, the command dish has a shorter support, and the three measured drive outlets average 13.99 pixels from their painted centers. Glacier Tongue’s narrowing bow averages 7.84 pixels across nine independently marked rows. It has lit windows in the glazing, worn blue enamel and a framed port dock with 63.38 m of clear sightline. Both exteriors reopen at 1,280 m. The upper drive positions and the regularity of the armor seams remain visible differences. [Owner’s two ratings](../../../reports/game-direction/inbox/20261125T193707Z.json) · [Measurements and material checks](../../Testing/research-battleship-registered-profiles.json).

| Ship | Rotate and switch revisions | Compare original and models | Editable Blender scene |
| --- | --- | --- | --- |
| Delta Tree | [Open review](http://127.0.0.1:8953/hull-review.html?ship=delta-tree&round=177) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/delta-tree-full.png) · [courtyards](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/delta-tree-deck.png) · [drives](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/delta-tree-detail.png) · [observatory](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/delta-tree-observatory.png) | [Delta Tree](../../../Assets/ShipConcepts/hull-review-r177/observatory/delta-tree/delta-tree.blend) |
| Glacier Tongue | [Open review](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue&round=177) | [Whole ship](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/glacier-tongue-full.png) · [leading armor](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/glacier-tongue-detail.png) · [dock](../../../Assets/ShipConcepts/hull-review-r177/comparisons/selected/glacier-tongue-berth.png) | [Glacier Tongue](../../../Assets/ShipConcepts/hull-review-r177/berth/glacier-tongue/glacier-tongue.blend) |

The [reusable Blender guide](<D:/OtherProjects/asset-pipeline/Tools/asset-pipeline/experimental/blender_reference_modeling/README.md>) includes the scripts and a part independent of the ships. The [Aquila replay instructions](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md#replay-the-courtyard-observatory-and-tapered-bow>) describe each ship’s recipe.

Round 176 revises the stern of Delta Tree and the forward armor of Glacier Tongue. Comparing three near-side nozzle centers with the original painting, mean distance fell from 73.20 to 34.17 pixels. Five independent armor heights on Glacier Tongue now average 9.61 pixels from the painted edges, down from 62.57. Both use the same 1,024-pixel gallery frame as before. Delta’s upper outlets are still tucked inward. Glacier’s leading plates are blunter than the painted taper. The playable hulls remain at round 172.

| Ship | Inspect beside original and switch revisions | Detail comparison | Editable Blender scene |
| --- | --- | --- | --- |
| Delta Tree | [Rotate the stern](http://127.0.0.1:8953/hull-review.html?ship=delta-tree&round=176) | [Three drive banks](../../../Assets/ShipConcepts/hull-review-r176/comparisons/delta-tree-detail.png) | [Delta Tree](../../../Assets/ShipConcepts/hull-review-r176/selected/delta-tree/delta-tree.blend) |
| Glacier Tongue | [Rotate the bow](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue&round=176) | [Forward armor and windows](../../../Assets/ShipConcepts/hull-review-r176/comparisons/glacier-tongue-prow-detail.png) | [Glacier Tongue](../../../Assets/ShipConcepts/hull-review-r176/glacier-prow/glacier-tongue/glacier-tongue.blend) |

Glacier’s shared Shape Match fit uses 13 marked bow corners and the side and section studies for depth. The saved recipe carries the windows with the surrounding plates. Delta’s stern uses modeled collars, ducts and exhausts arranged in banks of five, four and three. [Reusable methods](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md#fit-layered-armor-from-a-painting>) · [Reopened exports, saved artwork and checks](../../Testing/research-battleship-stern-and-prow.json).

Round 175 uses Asset Pipeline’s [saved Blender workflow](<D:/OtherProjects/asset-pipeline/knowledge/blender-reference-modeling.md>). At one fixed camera, compare the original, earlier model and current model for [Delta Tree](http://127.0.0.1:8953/hull-review.html?ship=delta-tree&round=175) or [Glacier Tongue](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue&round=175). Delta Tree gained supported branch platforms, a deeper hull and a repositioned observatory. Glacier Tongue gained stepped bow plates, a raised outer blade and an open illuminated port bay. Both remain exterior review models. [Delta Tree scene](../../../Assets/ShipConcepts/hull-review-r175/refined/delta-tree/delta-tree.blend) and [Glacier Tongue scene](../../../Assets/ShipConcepts/hull-review-r175/glacier-bow-pass/glacier-tongue/glacier-tongue.blend) · [Measurements](../../Testing/research-battleship-reference-refinement.json).

The owner chose to compare the revised models with the original art before changing the game. Each local comparison pairs the selected painting with a rotatable exterior. **Plain gray** shows the geometry. **Gallery angle** restores the opening view. [Delta Tree](http://127.0.0.1:8953/hull-review.html?ship=delta-tree) · [Glacier Tongue](http://127.0.0.1:8953/hull-review.html?ship=glacier-tongue).

**Delta Tree.** The trunk branches around eleven sunken courtyards with inset terraces. A stepped observatory rises behind the fan. Twelve individual drive housings follow the supporting stern study, with five below, four in the middle and three above. The side doors open behind arched frames. [Editable Blender model](../../../Assets/ShipConcepts/hull-review-r174/delta-tree/delta-tree.blend).

**Glacier Tongue.** Separate silver shelves turn back above blue armor. The port hangar opens behind three enclosed crossings to an outboard blade. The far side closes around the keel. The gallery and near-side artwork show a shorter blade than the overhead guide. The model follows the gallery for its length. [Editable Blender model](../../../Assets/ShipConcepts/hull-review-r174/glacier-tongue/glacier-tongue.blend).

Both exports reopened at 1,280 m. The gallery camera angles are estimated. Guide artwork supplies some upper and side paint. The branch walls, deck floors, armor lips, hangar walls and crossings are modeled. These exteriors await a likeness decision for each ship. Gameplay still uses the existing boarding models. [Geometry and export checks](../../Testing/research-battleship-hull-review.json).

### Hull proportions and reference views

Each model study had one three-quarter concept as its design input. The other views were rendered from the modeled geometry. Glacier Tongue’s original leaves a tall bay beneath the overlapping plates, with crossings to a separate side plate. The model compresses that opening into a slot. Delta Tree’s original builds up from the shallow bow to a tall bank of drives. The modeled hull has a more uniform side profile. [Originals, current models and straight side and bow views](http://127.0.0.1:8953/#i-aquila-direction-r148-hulls).

**Proposed for these two hulls.** Establish a straight side view and a bow view from the selected original. Mark length, width and hull height from keel to main deck. Mark the bay opening or drive stack separately from the mast. Use the same scale and reference lines for left, right, top, underside, bow and stern. Keep Glacier Tongue’s outboard plate on its original side. Where the art hides a surface, the new view proposes its shape. Compare a rough 3D hull with the agreed outlines before another material pass. The exact proportions and hidden surfaces remain open.

For each hull selected for modeling, the owner wants as many reference images as the design needs. The proposed brief starts with twelve views, then covers walkable decks, mechanisms and materials for each ship. [Modeling references and prompts for the first pair](ModelingReferences.md).

### Completed reference packs

Each pack includes 24 selected images, the original gallery concept, earlier studies, registered floor plans and an editable Blender proportion study. Start with Delta Tree’s stacked drives or Glacier Tongue’s deep docking bay. [Delta Tree reference pack](../../../Assets/ShipConcepts/modeling-references-r150/delta-tree-reference-pack.zip) · [Glacier Tongue reference pack](../../../Assets/ShipConcepts/modeling-references-r150/glacier-tongue-reference-pack.zip) · [browse the gallery](http://127.0.0.1:8953/#i-aquila-direction-r150-packs).

The side, top and bow studies informed the rough hulls. Six straight Blender projections per ship share one camera scale. Three proposed playable decks share a lift and stair. Physical dimensions, doorway clearance and the game’s final mounts still require a modeling pass. Higgsfield completed 54 images, including six earlier studies. The 55 matching debits and one refund total 40.5 credits. [Generation and archive checks](../../Testing/research-battleship-reference-images.json).

### Proposed first boarding test

Start with Glacier Tongue because its earlier hull flattened the docking bay. Set the scale of the existing Blender study using the shuttle and player. Keep the bay roof and enclosed crossings recognizable from the side. Build one route from the shuttle ramp through the airlock to the lift lobby. In a small Windows Unreal level, check the bay from the combat camera, then walk that route with collision enabled. After it fits, use the same scale to test Delta Tree’s shallow bow and stacked stern drives.

### Glacier Tongue boarding scale

**Approved design.** Glacier Tongue measures **1,280 m** bow to stern. Its landing pad measures **142 m wide by 180 m long**. The owner chose this hull in the boarding comparison and approved the corrected hangar. [Submitted choice](../../../reports/game-direction/inbox/20261124T235647Z.json).

The first Windows trial uses 6.4 metres for each Blender study unit. Bow to stern measures 640 m. Peregrine keeps its 68 m length, 64 m span and 16 m height. The standing player measures 1.80 m, with the eye 1.64 m above level ground. The ramp descends 2 m over an 11 m run. Both open airlock frames provide 2.8 m of width and 3 m of headroom.

The three covered crossings and the outboard plate come from the same 17-part Blender study shown in the reference pack. This trial parks the shuttle between the forward and middle crossings. The boarding deck, ramp, airlock and lift lobby share that hull’s coordinates. The lifts mark the end of the walk. [Controls](../../../native/Aquila/README.md#glacier-tongue-boarding-test) and [boarding checks](../../Testing/native-glacier-boarding.json).

**Playable size comparison.** The roof, outboard plate and three crossing spans scale uniformly around the dock datum. Peregrine, its ramp, the player and crew fittings keep their original dimensions. The structural connections extend from the pier and lift lobby to the larger hull. The test opens at the approved size. **B** switches to 640 m and back from the same spot on the ramp. [Packaged comparison](../../Testing/native-glacier-scale.json).

The landing pad belongs to Glacier Tongue. Its width and length double with the hull, from 71 × 90 m to 142 × 180 m. The platform grows outward from the crew entrance. Peregrine’s wing tips clear the side edges by 3.5 m or 39 m. The ramp still leads to the same airlock. [Windows landing pad checks](../../Testing/native-glacier-pad.json).

### Proposed Delta Tree boarding test

Bring Delta Tree’s existing Blender study into the Windows boarding test. At combat-camera distance, compare the shallow branching bow and tall bank of stern drives with the side reference. Park the same 68 m Peregrine at one round outer dock. Walk from the ramp through the airlock to the first branch junction. Check that the shuttle clears the dock and the player can walk the route without clipping. Use Glacier Tongue as the scale reference while choosing Delta Tree’s own length. [Side profile and dock section](http://127.0.0.1:8953/#i-aquila-direction-r158-shape).

### Delta Tree boarding scale

**Approved design.** Delta Tree measures **1,280 m** bow to stern. The owner chose to keep this length after boarding the ship and comparing it with Glacier Tongue. Use this scale for future hull and deck work. [Submitted choice](../../../reports/game-direction/inbox/20261125T012507Z.json).

The playable hull uses the reference pack’s Blender study. At the sampled sections, the forward hull measures about 99 m thick and the stern about 259 m. Three banks of drive rooms rise across the stern.

Peregrine parks at the outermost round island, **Arrivals 06**. From its ramp, walk through the rotunda, pass the two airlock frames and follow the passage to **Junction 01**. Research lies to the left. Command lies to the right. At the dock, **Tab** shows the whole hull, **V** frames the berth and **N** switches ships. [Boarding controls](../../../native/Aquila/README.md#delta-tree-boarding-test) and [Windows measurements](../../Testing/native-delta-boarding.json).

### Proposed Delta Tree bridge playtest

Turn right at Junction 01, ride the lift to the command deck and walk onto the bridge. The windows would look across the branching bow and round docks. The saved command deck plan places the circular bridge forward of the lift. [Command deck plan](../../../Assets/ShipConcepts/modeling-references-r150/delta-tree/15-command-deck.png) and [bridge view study](../../../Assets/ShipConcepts/modeling-references-r150/delta-tree/17-bridge-window.png).

The original proposal connected one working turret to a shielded practice drone outside the bridge. The owner later specified that the player watches the ship conduct the exercise. [Current bridge design](#delta-tree-command-bridge).

### Delta Tree command bridge

The owner selected the bridge playtest in round 161 and clarified the visitor’s role in round 164. A two-way lift connects the right branch to a circular command bridge 48 m above the arrivals deck. Its forward windows overlook the original hull and round docks. The ship fires a kinetic cannon at a shielded practice drone while the player walks freely inside. A depleted shield recharges for the next volley. The exercise belongs to the boarding visit. [Owner’s selection](../../../reports/game-direction/inbox/20261125T023749Z.json), [bridge visit](../../../native/Aquila/README.md#delta-tree-command-bridge) and [Windows checks](../../Testing/native-bridge-observer.json).

### Proposed bridge view for the simulator duel

Round 166 proposed fitting both ships and watching their duel from Delta Tree’s walkable bridge. The first check would follow an incoming round across a camera switch through impact. [Accepted bridge playtest](../../Testing/native-bridge-observer.json) and [simulator fittings](../../../native/Aquila/README.md#combat-simulator-on-windows).

### Delta Tree’s bridge in the simulator

The owner selected the duel in round 166. **Start duel aboard** places the visitor at the forward windows with both fitted ships fighting. The arena and the walkable command deck share the same rounds, shield impacts and hull damage. Switching views preserves the fight and the visitor’s place aboard. [Owner’s selection](../../../reports/game-direction/inbox/20261125T052555Z.json), [playtest controls](../../../native/Aquila/README.md#combat-simulator-on-windows) and [first bridge duel](../../Testing/native-bridge-duel.json).

In round 168 the owner spotted guns floating above the old exteriors. The duel and fitter now use the approved 1,280 m boarding studies. Delta Tree’s forward windows sit in its docking island above the bronze branches. Glacier Tongue keeps its curved roof, three crossings and outboard plate. Each hull has 30 numbered gun sockets. The base follows the slope at that port and the rounds leave the fitted barrels. The shield follows the hull geometry. [Windows hull and mount checks](../../Testing/native-duel-hulls.json).

In round 169 the owner asked to watch the same duel from either ship. Glacier Tongue’s walkable bridge sits on the forward observation crown. Its wide central pane looks across the curved roof toward Delta Tree. The paired consoles draw on the ship’s saved bridge references. **N** changes ships and **Tab** visits the exterior. Each bridge remembers the visitor’s footing and heading. [Windows bridge switch](../../Testing/native-two-bridges.json).

## Ships built in space

These battleships and most other large ships are built in space and spend their whole lives there. They never enter an atmosphere. Their outlines are an artistic choice. The planetary shuttle handles surface visits.

### Earlier proposals under the prototype names

**Resolute.** An inhabited crescent curves around a harbor open to the stars. Bring a shuttle through the gap and dock partway around the inner rim. From the window in your quarters, you can see the opposite arm of your own ship and watch shuttles crossing between them. Research labs and hangars open onto the same curved promenade. A bank of engines deep in the outer arc gives the ship a recognizable stern.

**Sentinel.** Three dark armor fins curl around a tall central tower, each ending at a different height. Looking down the tower, the fins form a three-point spiral. From the side, you can see starlight between them. A docking collar opens at the base. Ride a glass-sided lift to the bridge and the stars disappear behind each fin, then come back into view.

The space-only rule is settled. The crescent, spiral, boarding routes and materials are proposals. Each hull can place its fitted equipment around the shape that emerges from the discussion.

## Concept exploration and image budget

The original brief called for 50–60 concepts before choosing the first designs to model. The exploration guide called for a different architectural idea per prompt. It varied structure, proportions, symmetry and engine placement while keeping the whole ship in a square frame. Small shuttles conveyed scale. [Concept exploration guide](<C:/Users/jacob/Downloads/AI_Concept_Exploration_Generator.md>).

These are November 24 UTC quotes for one square, text-only image on the signed-in Higgsfield account. The cost check used a generic placeholder, without sending ship designs. Batch totals assume one request per image. Dollar value depends on the plan or pack. [Higgsfield cost checks](https://higgsfield.ai/blog/higgsfield-cli) and [credit packs](https://higgsfield.ai/creator-hub/help-center/credits/how-credit-packs-work).

| Model and route | Resolution and quality | Credits each | 50 images | 60 images |
| --- | --- | ---: | ---: | ---: |
| Z Image through Higgsfield | Square, provider sets resolution | 0.15 | 7.50 | 9 |
| Seedream 5.0 Flash through Higgsfield | 1K | 0.50 | 25 | 30 |
| GPT Image 2.6 Sunburst through Higgsfield | 1K, low | 0.75 | 37.50 | 45 |
| GPT Image 2.6 Sunburst through Higgsfield | 2K, high | 2 | 100 | 120 |
| Z-Image-Turbo on the local GPU, proposed | 1024 × 1024 | 0 provider credits | 0 | 0 |

Content Automation’s ComfyUI nodes call Higgsfield and use its credits. A local model uses GPU time and electricity. Its documented setup and Aquila execution are separate. The [local model guide](https://huggingface.co/Tongyi-MAI/Z-Image-Turbo) shows 1024 × 1024 generation with a seed. A working Aquila installation of that model is unverified. [Content Automation’s operator notes](<D:/OtherProjects/content-automation/docs/knowledge/COMFYUI_HIGGSFIELD_OPERATOR_NOTES.md>).

**Proposed comparison.** Try the same three radically different prompts on hosted Z Image and 1K low Sunburst. Possible structures include a hollow geode threaded with inhabited bridges, a city suspended beneath a crown of docks, and three habitats tied into a knot. Separate the designs by silhouette at thumbnail size. Preserve the prompt and design choices for each take. The two hosted models expose no image seed setting. Record a seed when available and keep each provider job ID.

The round 142 proposal priced six images at up to 2.70 credits, with one attempt per model and concept. Adding 57 new Z Image hulls would have brought the total to 60 distinct concepts for 11.25 credits. The owner subsequently selected Sunburst for the trial.

### Three Sunburst concepts

On November 24 UTC, Higgsfield completed three GPT Image 2.6 Sunburst jobs at 1K low quality. Each original is a 1024 × 1024 PNG from one text-only request. The three 0.75-credit quotes total 2.25 credits. The account lists three matching debits by model, amount and creation time. Its transaction rows omit job IDs. Exact prompts, job IDs, settings and original images are retained together under the game’s local asset folder. [Originals and generation records](../../../Assets/ShipConcepts/sunburst-r143/) and [local gallery](http://127.0.0.1:8953/#q-aquila-direction-r143-batch).

| Concept | Visible in the original |
| --- | --- |
| Bridge Geode | Three lit bridges cross a hollow, faceted hull. The opening in its far wall frames the stars. |
| Hanging City | Four terraced towers hang at different heights beneath an open docking ring. |
| Habitat Knot | Copper, ivory and blue habitat loops weave around each other. The shuttle approaches a bay between two loops. |

These were exterior concepts. Ship assignments and walkable rooms were open at the trial stage. The trial counted toward both proposed collection sizes. At the same price, 47 additional concepts would have cost 35.25 credits and 57 cost 42.75.

### The 60-ship collection

The owner submitted the 60-ship choice through the report at 05:46 UTC on November 24. It covers 57 additional GPT Image 2.6 Sunburst concepts at 1K low quality, up to 42.75 more credits. Each exact square, text-only request returned a 0.75-credit quote. The three originals keep numbers 01–03.

Higgsfield completed all 57 new jobs. Combined with the trials, the collection contains 60 original 1024 × 1024 PNGs, each with a distinct file hash and provider job ID. The additional prompts sampled eleven families of ship architecture while keeping the same framing, lighting and scale cues. Each ledger record links the exact submitted prompt to the original. The recorded design seed describes the prompt choices. The provider offered no image seed. Asset Ledger lists all 60 ships under the Aquila game.

The account recorded 57 matching 0.75-credit debits by model and creation time. Transaction rows omit job IDs. The additional cost was 42.75 credits and the combined cost was 45. The approved generation is complete.

The gallery provides a numbered overview, a larger view and the original PNG for each hull. The shape descriptions were checked against the generated pictures. [Browse all 60](http://127.0.0.1:8953/#aquila-research-battleship-roster), [numbered catalog](collection-60.json), [57 new originals and receipts](../../../Assets/ShipConcepts/sunburst-r144/) and [three trial originals](../../../Assets/ShipConcepts/sunburst-r143/).

### Research battleships

The owner kept all 60 hulls in round 145 and assigned the whole collection to the research battleship roster in round 146. This role combines long-range research, crew quarters, shuttle operations and capital-ship combat. A ship can carry whole neighborhoods and fulfill that role. [Established research battleship direction](../../../knowledge/game-direction.md#research-battleship-and-walkable-bridge-2026-11-15-utc).

The concept guide called for long-range research, exploration and heavy combat. All 60 submitted prompts specified an inhabited capital ship. The 57 expansion prompts explicitly asked for research, exploration and self-defense. The three trial prompts concentrated on hull architecture. The combat wording in the submitted prompts was lighter than the master brief. The pictures establish silhouettes and places the crew might inhabit. They leave weapons, shields and interior layouts to ship design.

Hanging City (02) offers one way to read that combination. Its towers could house crew and labs, while the docking ring could carry the ship’s batteries. That fitting is an example for discussion.

## Archived briefs for the prototype names

| Ship | Exterior brief |
| --- | --- |
| Resolute | A long research carrier with a split bow, two open hangar galleries, an offset bridge and six stern drives. Pewter plating, an ochre stripe and warm bay lights. |
| Sentinel | A blunt hammerhead bow, low cheek castings, an amber sensor slit and two rectangular drives around a square stern notch. Graphite metal and gray ceramic. |

Both requested 30 flush mounts at fixed coordinates, separate material finishes, three exterior views, a thumbnail silhouette check, an editable model and an Unreal export. Asset Pipeline built both first studies in Blender. Resolute’s bays have floors, frames and lights visible from outside. Neither study includes connected rooms that a player can enter. Both Unreal import checks are pending. [Resolute exterior review](http://127.0.0.1:8964/) and [Sentinel exterior review](http://127.0.0.1:8963/).

The two original prompts are preserved as the record of that build. [Resolute brief](Resolute.txt) and [Sentinel brief](Sentinel.txt).

## What belongs in the design discussion

For each ship, consider its distant outline alongside a cutaway showing where someone boards, where they can walk and what they see. Bridge windows and hangar openings should face the same way from inside and outside. Rooms can use separate human-scale spaces.

Each research battleship is a home, research vessel and shuttle carrier. A walk from the shuttle ramp can pass the research area and reach a bridge overlooking the approach. Engineering and personal quarters are established destinations. Each hull needs its own boarding route.

Keep room for up to 30 fitted turrets. Place each socket where that hull supports the whole gun base. The fitter and combat read the same socket positions.
