# Modeling references for research battleships

Prepare references when a ship is selected for modeling. Delta Tree and Glacier Tongue are the first pair. Keep the chosen illustration with every set. Start with a single turnaround sheet. Add sections and close-ups where the modeler still needs to resolve a joint, material or playable space.

## First sheet for a selected ship

Asset Pipeline owns the reusable long and upright asset recipe, 4K Max preset and request builder. Use this workflow for a future ship or another asset. [Character and asset sheets](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md>).

Use the original concept as the sole image input to one GPT Image 2.6 Sunburst generation. Ask for one 4K landscape PNG per ship. Put port and starboard on the first row, top and underside on the second, bow and stern on the third. Keep a common scale, level orthographic cameras, visible outlines and narrow margins. The 4K setting applies to the entire sheet.

Check the original silhouette and the features that connect adjacent views. Count visible engine tiers and outlets, trace the same mast and dock along the hull, and verify that asymmetric structures stay on their physical side. A hidden feature inferred on the sheet becomes a proposed design choice. Use the checked turnaround to guide exterior modeling. Keep the earlier close-ups, cutaways and interior plans available for their specific subjects. [Exact prompts, original paintings and two-sheet quotes](turnaround-sheets.json).

### Using the two completed sheets

Both sheets contain six views in 3504 × 2336 pixels. In each Blender reference library, the Image Editor opens on the full sheet. Its image selector holds the original painting and six lossless crops. Append the named turnaround collection to the model scene for port and top guides. Both guides span 1,280 m from bow to stern. Their vertical and lateral origins follow marked hull features. [Delta Tree sheet](../../../Assets/ShipConcepts/turnaround-sheets-r190/delta-tree.png) · [Delta Tree Blender references](../../../Assets/ShipConcepts/turnaround-sheets-r190/delta-tree/delta-tree-references.blend) · [Glacier Tongue sheet](../../../Assets/ShipConcepts/turnaround-sheets-r190/glacier-tongue.png) · [Glacier Tongue Blender references](../../../Assets/ShipConcepts/turnaround-sheets-r190/glacier-tongue/glacier-tongue-references.blend).

Delta’s stern shows two columns of three illuminated outlets. Four engine tips cross the top panel. The original confirms three vertical tiers on the visible flank. Glacier’s side views distinguish the open port bay from its plated starboard side. Its top and underside both place the blade below the hull with the bow left. Use the separate vertically flipped underside for orientation. Its seven pictured rear outlets remain a proposal. Both ships’ bow panels enlarge the beam. At equal hull length, compare the deck perimeter and spacing of the outer fittings against the original before reshaping the models. [Marked panel boundaries](turnaround-view-marks.json) · [Measured artwork and model comparisons](../../Testing/research-battleship-turnarounds.json).

### Choosing how to rebuild the exteriors

Round 191 recommendation, carried out in round 192 below.

The owner liked both sheets. Start a separate, plain Glacier Tongue exterior study to test the slimmer body and nearly parallel blade. The existing top view measures 51% as wide as the model is long. The sheet measures 33% including the blade. Then try Delta Tree’s branching deck and docks together. Its existing width is 54% of length, against 41% on the sheet.

Compare each simple model against the original painting and calibrated port and top views. Keep the old scenes side by side. Reuse materials and individual modules after their shapes fit. Preserve 1,280 m scale and carry over the tested bridge, hangar and turret clearances as checks against the new skin. This is the proposed approach to the next model pass. [Source comparisons](../../Testing/research-battleship-turnarounds.json) · [Workflow checks and recommendation](../../Testing/research-battleship-turnaround-workflow.json).

### The first plain exteriors

Round 192 rebuilt both ships in fresh Blender scenes from the accepted sheets. Each model spans 1,280 m. Their neutral comparison boards show the original angle, port and overhead at matched scale. Glacier’s blade begins too far forward in the original-angle comparison. Delta’s branch roots crowd some deck wells. Refine these shapes before moving detail or finishes onto them. [Glacier comparison](../../../Assets/ShipConcepts/hull-blockouts-r192/glacier-tongue/comparison.webp) · [Delta comparison](../../../Assets/ShipConcepts/hull-blockouts-r192/delta-tree/comparison.webp) · [Native and exported checks](../../Testing/research-battleship-sheet-blockouts.json) · [Repeat either build in Asset Pipeline](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#replay-the-ship-example>).

### Blade and branching-deck comparison

Round 193 uses the painting for Glacier’s visible blade contour and the plan for lateral spacing. Both crossings meet the hull and blade. The revised middle crossing clears the center of the boarding doorway. Delta’s courtyard floors and smaller ribs sit between connected deck branches. The new scenes are exterior studies. [Glacier comparison](../../../Assets/ShipConcepts/hull-refinements-r193/glacier-tongue/comparison.webp) · [Delta comparison](../../../Assets/ShipConcepts/hull-refinements-r193/delta-tree/comparison.webp) · [Native and exported results](../../Testing/research-battleship-blade-and-courtyards.json) · [Asset Pipeline replay](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#fit-an-individual-part-against-the-painting>).

### First material comparison

Round 194 projects Glacier’s blade painting onto the exposed plating and glazing. It uses the overhead sheet for Delta’s rims and floors. Both boards show plain and fitted surfaces from the same camera, with a turned-light comparison. The ships remain 1,280 m exterior review models. [Glacier’s blade and cabin lighting](../../../Assets/ShipConcepts/hull-finishes-r194/glacier-tongue/selected/comparison.webp) · [Delta’s courtyard materials](../../../Assets/ShipConcepts/hull-finishes-r194/delta-tree/selected/comparison.webp) · [Color samples and exported maps](../../Testing/research-battleship-feature-finishes.json) · [Reusable Asset Pipeline workflow](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#fit-surface-materials-after-the-shape-matches>).

### Forward armor and bow observatory

Round 195 fits Glacier’s silver prow plates and lit windows to distinct painted edges. Delta’s curved glass front reveals cabin lights and joins a separate plated cabin. Compare the original artwork, earlier model and revised model at the same scale in each row. Delta’s side view puts all five glass marks on panes. Its roof still falls short in the painting view. [Glacier’s four comparisons](../../../Assets/ShipConcepts/bow-studies-r195/glacier-tongue/selected/comparison.webp) · [Delta’s four comparisons](../../../Assets/ShipConcepts/bow-studies-r195/delta-tree/selected/comparison.webp) · [Geometry, materials and checks](../../Testing/research-battleship-forward-armor-and-observatory.json) · [Blender workflow](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#fit-overlapping-plates-and-seat-a-glazed-cabin>).

### Forward landings and cabin footprint

Glacier’s landing decks share a recessed service wall beneath the port armor. Delta’s revised cabin keeps its front sill, widens the rear shoulders and seats a separate skirt on the deck. Both boards include new close renders from the original gallery angle. [Glacier’s port bay](../../../Assets/ShipConcepts/landing-studies-r196/glacier-tongue/service-bay/comparison.webp) · [Delta’s observatory](../../../Assets/ShipConcepts/landing-studies-r196/delta-tree/seated/comparison.webp) · [Geometry, sources and limits](../../Testing/research-battleship-landings-and-cabin.json) · [Repeat the Blender study](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#fit-a-landing-and-cabin-across-conflicting-views>).

### Docking hall and passenger glazing

Glacier’s forged entrance surrounds a full-depth room with a split sill. A glazed passage connects the collar to the aft crossing. Delta’s passenger rows follow both curved flanks, with separate warm panes, dark recesses and metal sills. The original paintings guided both features. Glacier’s doorway sits higher than its side drawing, and Delta’s denser glazing starts farther aft. [Glacier’s comparisons](../../../Assets/ShipConcepts/door-windows-r197/glacier-tongue/cast/comparison.webp) · [Delta’s comparisons](../../../Assets/ShipConcepts/door-windows-r197/delta-tree/fitted/comparison.webp) · [Measurements and source scenes](../../Testing/research-battleship-docking-and-windows.json) · [Blender recipes](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#recessed-entrances-and-rows-of-windows>).

### Lower armor and drive housings

Glacier’s close view follows the indigo lower armor, eight inset silver plates and the opening beneath them. Delta’s close view separates the pale collars, dark cases and amber rims. Compare its stern against its plan to see the different drive counts in the same sheet. [Glacier’s armor](../../../Assets/ShipConcepts/armor-drives-r198/glacier-tongue/refit/comparison.webp) · [Delta’s drives](../../../Assets/ShipConcepts/armor-drives-r198/delta-tree/tempered/comparison.webp) · [Model records](../../Testing/research-battleship-bay-armor-and-drives.json) · [Repeat in Blender](<D:/OtherProjects/asset-pipeline/knowledge/asset-turnaround-sheets.md#armor-overhangs-and-repeated-outlets>).

## Twelve views to establish the hull

| Views | Image request | What the modeler can check |
| --- | --- | --- |
| Six straight exterior views | Left, right, top, underside, bow and stern. One full ship per image, with a common scale and margin. | Hull thickness, asymmetry, drive spacing and which openings pass through the ship. |
| Two three-quarter views | A low view from the bow and a high view from the stern, on opposite sides. Choose angles that reveal surfaces hidden in the original. | Where plates overlap, where a bridge attaches, and how the underside meets the stern. |
| One lengthwise cutaway | Cut from bow to stern. Mark the cut on the top view. Expose deck heights, the main passage, the bay and the drive rooms it intersects. | How the ship gains height and where people or equipment fit inside. |
| Three cross-sections | Cut across the forward hull, through the main dock and through the aft drives. Mark each position and viewing direction on the side view. | The shape of each slice. Distinguish solid hull, walkable space and open vacuum. |

Use the side, bow and plan on the checked sheet to build a plain hull in Blender. Set length, beam and keel-to-deck height independently of antennas. Carry the original and agreed proportions into later detail studies. A matching render of the rough hull can guide a close-up. Compare outlines, deck levels and openings before detailing the mesh. Blender can display each reference on its matching axis. [Blender reference images](https://docs.blender.org/manual/en/5.0/modeling/empties.html)

Ask for a plain background, even lighting and clear gaps between parts. Use orthographic projection for the six straight views. Keep the same named landmarks and the same shuttle throughout. For close-ups, include a person beside an airlock. Add scale bars, section marks and dimensions from the agreed Blender scene. Keep a clean copy of each image beneath those annotations. A hidden surface first shown in new artwork is a design proposal.

## References for playing aboard the ship

| Reference | What to ask for |
| --- | --- |
| Playable deck plans | One plan per accessible deck. Connect the boarding door, labs, crew quarters, bridge and engineering. Align lifts, stairs and exterior openings across decks. |
| Eye-level views | Stand on the shuttle ramp looking into the dock. Stand at a bridge window looking out. Keep the door, deck and window heights consistent with the plans. |
| Mechanisms and fittings | Show a hangar door open and shut from the same angle. Show an empty turret port beside the fitted version. Map the 30 port positions onto the accepted hull in Blender. |
| Materials and lighting | Show adjacent samples of armor, exposed metal, glazing and lit windows under neutral light. Include one lights-off comparison. Use the samples when building the actual materials. |
| Hidden assemblies | When an attachment is unclear, show one plate lifted away with its supports exposed. Keep the rest of the ship in place. |

## First requests for Asset Pipeline

**Glacier Tongue.** Start with the docking-side profile. Then ask for the following detail after choosing the bay proportions.

> Using the attached Glacier Tongue concept and side profile, cut across the ship through one enclosed crossing. Show the underside of the blue armor, the open bay, the crossing and the separate outer plate. Put the same shuttle on the approach and a person at the boarding door. Keep the air around the crossing visible.

**Delta Tree.** Start with the side profile from the shallow branching bow to the stacked stern. A separate section through a round dock should show where its branch joins the crew passage. For the stern, ask for this view.

> Using the attached Delta Tree concept and side profile, show the ship from high astern. Keep the forward fan shallow. Reveal how the drive rooms stack above and below the inhabited rim. Keep the dock branches and the observation dish in their original positions. Use even light so the gaps between the drive housings remain visible.

The first Blender check is a plain hull seen from the original angle, the side, the bow and underneath. Overlay the straight references. Place the same shuttle in the bay and a player figure at the boarding door. Check that the door reaches a deck and the passages fit within the shell before committing to the detailed exterior.
