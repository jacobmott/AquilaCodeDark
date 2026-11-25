# Rebuilding editable hard-surface hulls from silhouettes, sections and openings

Find the most practical techniques for revising an existing, editable Blender mesh to match screenshots or concept art in three dimensions. Focus on thickness, section shape, overlapping parts and genuine openings. Assume a registered camera, saved scale and a record of which views disagree. The desired output is an editable hard-surface asset with inspectable structure.

## Project and two concrete failures

Each of two 1,280-metre fictional spacecraft has a primary three-quarter painting, additional independently drawn elevations and sections, and a textured Blender model. The primary painting guides the design. Our workflow already measures marked outlines and feature centers, checks a separate angle, and casts finite rays through meshes. A second input scenario is a set of actual screenshots of one unchanged 3D asset, where correspondences can be trusted more strongly.

Delta Tree's engine bank can line up while its lower stern shoulders still curve inward too early. Glacier Tongue's dock hangs beneath a hull and connects to a separate blade through three enclosed crossings. From one view the bay can look right while solid geometry fills the opening, or a crossing misses a support. The side and primary drawings place the outer blade differently.

## Questions

1. Compare a simple artist-led method using cross-sections, silhouette traces and lofts with one suitable constrained fitting or volume-based method. Show how to express fixed features, positive and negative space, occlusion order, known scale, real support contacts and possible asymmetry. Explain what silhouettes can establish about concave or concealed surfaces. Describe how to author a plausible hidden side and label its uncertainty.
2. Give two short Blender experiments an artist can run. For Delta, change a shoulder section while keeping the exhaust pitch and the main gallery alignment. For Glacier, distinguish an opening through the ship from a recess backed by a wall, keep all crossing endpoints attached and preserve the outboard blade. Identify which image measurements guide edits and which withheld measurements or angles can disprove each interpretation.
3. Evaluate whether a current open, local image-to-3D method, differentiable renderer, photogrammetry pipeline or similar technique earns a place in this workflow. Choose at most two advanced candidates on their demonstrated merits. Distinguish consistent screenshots from independently drawn views. Explain whether the output is usable topology, a visual proxy or a starting mesh, what manual repair it needs and how an artist could reject it. Give version, license including relevant weights, approximate hardware requirements where documented, and any published limits.
4. Explain how to check the result by overlaying silhouettes at a known scale, comparing cross-sections and isolating obscuring parts. Include physical tests for openings and joints, and inspection from an angle held out from fitting. Explain where these tests can still disagree with visual likeness, and what one additional reference would resolve the greatest uncertainty.

Recommend what to try first, what to skip for these assets and why. Cite original papers, official Blender/tool documentation or maintained implementations. Distinguish measured evidence from proposed artist practice. Camera solving and matching materials are covered by separate requests.

Return a Markdown report. Start it with this exact routing header, followed by the report title:

---
research_console_project: aquila-code-dark
research_console_key: research-battleship-hull-reconstruction-v1:run-1
---
