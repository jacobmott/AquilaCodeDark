# Matching a textured Blender spaceship to illustrated concept art

Recommend a small, repeatable way to tell **which change will improve the likeness** when comparing a game model with concept artwork. We especially need to distinguish base paint, surface relief, roughness, metallic response, and emitted or reflected light. An artist should be able to use your recommendations on an ordinary Windows Blender workstation.

## What we already have

Two fictional, 1,280-metre research battleships have editable textured Blender meshes and glTF exports. Each has one primary three-quarter painting and independently illustrated supporting views, including orthographic-style elevations, sections, lit and unlit finish samples, and a lifted armor plate. The images are not calibrated captures of one physical model. When views disagree, the primary painting directs the design. We already lock the gallery camera, measure landmarks, compare silhouettes and seams, reserve unused shape marks, view gray geometry, and probe openings on the real mesh.

Delta Tree combines warm ivory armor, bronze trim and dark channels. Its main painted upper deck exports color, one roughness value and no normal texture on that finish. Glacier Tongue combines worn blue enamel, satin silver armor and a sheltered amber dock. In the artwork the dock has a bright center and a soft light falloff around its opening. The model looks more evenly lit. Some armor color was sampled from the lit painting. Our question is how to measure and diagnose differences like these without accidentally putting a shadow or a highlight into the paint.

## What to find

1. Find two or three genuinely useful techniques beyond a side-by-side beauty render. Give one simple artist-led baseline, then at most two stronger alternatives. Consider matched material samples under controlled lights, material masks, decomposed shader passes, inverse rendering and perceptual texture metrics only when justified. For each, state inputs, output, practical effort, and how a result guides a concrete edit.
2. Explain what a single painted view and independently generated support art can establish about shape, color, weathering, reflection and emission. Which quantities remain ambiguous without controlled observations? How can we compare the rendered appearance when the artwork has unknown lighting, tone curves, bloom or exposure? Include a way to reject a tempting but misleading match.
3. Give one small Blender experiment for Delta’s ivory and bronze, and one for Glacier’s dock. Describe the camera, lighting change, useful passes, regions to sample, and a separate view or lighting setup to hold back for verification. Include at least one observation that distinguishes scale of wear or relief from roughness, and one that distinguishes emission from reflected spill or bloom.
4. Identify any missing reference worth obtaining after exhausting our existing views. Describe the smallest useful brief, same-view or same-specimen constraints and what a synthetic image would still leave uncertain. Rank any advanced technique against the simple baseline. Say when to skip it.

Prioritize official documentation, original papers with public project pages or code, and concrete production demonstrations. Link directly to primary sources for important claims. Mark measured findings, research demonstrations, and proposed artist workflows separately. Give Blender or glTF version assumptions. For downloadable tools, identify licensing and local hardware requirements if known. Do not infer exact material constants from our descriptions. A concise ranked answer with testable steps is more useful than a list of products.

Return a Markdown report titled “Matching painted spacecraft materials in Blender.” Start the report with this exact routing header:

---
research_console_project: aquila-code-dark
research_console_key: research-battleship-material-matching-v1:run-1
---
