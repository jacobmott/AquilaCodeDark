# Applying the three ship research returns

November 25, 2026. All three Perplexity returns are version 1. The local Blender trial used the two 1,280 m review exteriors. The original gallery paintings govern likeness. The supporting drawings were made independently. [Source versions and attachment hashes](research-battleship-reference-matching-returns-v1.json) · [Ship measurements and files](../Testing/research-battleship-stern-and-berth-materials.json).

## Camera

Kept the same gallery camera, exposure and rear registration for both Delta Tree revisions. The bottom middle exhaust establishes translation. Bottom-row exhaust spacing establishes scale. The other four bottom centers land within 2.9 pixels of fresh marks in the 1,024-pixel rear study. The middle and upper rows sit about 20 and 26 pixels low. In the primary painting, the three visible near exhaust centers still meet their marks. Changing the camera to correct the upper tiers would move the lower bank too.

Use this registration for the lower shoulders. Keep the tier-height disagreement open. The camera report's focal-length and pose results came from a stand-in. This trial did not estimate a physical lens or rerun the synthetic benchmark. [Camera return and figures](../../Assets/Research/research-battleship-camera-registration/result-v0001.md).

## Hull

Changed the editable lower shoulder cross-section and tapered it into the old longitudinal stations. The exhausts, mast and window-deck heights stayed fixed. Four additional rows were marked on the rear study after freezing the mesh. Across both edges, their mean absolute gap fell from 33.75 to 1.97 m at the registered exhaust pitch. All 108 rays passed inside the twelve exhaust rims. The seam around the gallery-side door still needs work. The single rear outline leaves the hidden stern depth uncertain.

Use section fitting and reserved contours for visible edges. Use whole-assembly rays for openings. Keep learned geometry and differentiable fitting deferred until a specific missing region has stronger evidence. [Hull return](../../Assets/Research/research-battleship-hull-reconstruction/result-v0001.md) · [Editable recipe](<D:/OtherProjects/asset-pipeline/Tools/asset-pipeline/experimental/aquila_research_battleships/reference_hulls/stern_shoulders.py>).

## Materials

Kept Glacier Tongue’s mesh fixed. Compared four changes in order: wider emitting surfaces, lower diffuser power, patterned amber surfaces and muted emission. Compared the selected materials with the prior dock under the original lighting, a 65° key turn and a black world with every scene lamp off. Both exported panel maps are embedded and drive color and emission. The two native geometry digests match.

| Same studio view | Previous | Uniform warm panels | Selected patterned panels | Painting |
| --- | ---: | ---: | ---: | ---: |
| Whole opening, median relative luminance | 0.026 | 0.041 | 0.037 | 0.060 |
| Whole opening, 95th percentile | 0.206 | 0.561 | 0.506 | 0.555 |
| Inside wall, median | 0.156 | 0.547 | 0.424 | 0.319 |
| Inside wall, bright-region sRGB | 165, 123, 83 | 221, 194, 173 | 215, 190, 156 | 216, 186, 151 |

The uniform pass came closest on one whole-opening highlight statistic. Choose the patterned pass for the closer interior color and visible panel detail. Silencing the four berth finishes reduced the whole-opening highlight to 0.190. Exterior control highlights stayed at 0.064 to three decimals across previous and selected studio views. The doorway still needs a deeper rounded lip. These samples compare displayed appearances. The painting cannot identify physical emission power or roughness. [Material return](../../Assets/Research/research-battleship-material-matching/result-v0001.md) · [Reusable lighting test](<D:/OtherProjects/asset-pipeline/Tools/asset-pipeline/experimental/blender_reference_modeling/material_light_test.py>) · [Khronos emission specification](https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_emissive_strength/README.md).
