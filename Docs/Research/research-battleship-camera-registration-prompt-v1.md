# Matching a Blender camera to screenshots and conflicting concept views

Research a practical way to decide whether an apparent shape error comes from the mesh, the camera, or a disagreement between drawings. The deliverable is a repeatable camera-registration procedure for a Blender artist, plus an independent check before the artist edits geometry.

## Project and available evidence

We have editable exterior meshes of two fictional 1,280-metre spacecraft. Each has a primary three-quarter painting, separate supporting illustrations and saved Blender cameras. We can mark feature centers, lines, outlines, and a few known distances. Delta Tree has repeated exhausts and an offset dish. Glacier Tongue has sweeping curved plating, a projecting blade and a dock. The primary painting governs design disagreements. We can render fresh screenshots from a known camera for a controlled test. We cannot assume camera metadata, vanishing points, focal length, or true orthographic projection for the artwork.

## Questions

1. Compare multiple screenshots of the same unchanged mesh, a single uncalibrated picture, and independently drawn views of one design. Which camera quantities can be estimated in each? Cover unknown cropping, lens shift, field of view, apparent scale and optional distortion only when observable. Explain how to recognize an almost orthographic image. Describe ambiguity between focal length, distance and hull depth.
2. Compare a small manual Blender baseline with one robust numerical alternative. Consider vanishing-point alignment where directions are known, and 2D-to-3D correspondences against trusted parts of an existing mesh where they are not. Include the failure cases for largely curved hulls, symmetric repeated outlets, nearly coplanar anchors and questionable model geometry. Explain which camera parameters to hold fixed and how to keep fitting an inaccurate mesh from reinforcing its errors.
3. Give a worked diagnostic. A stern bank lines up in the gallery view, but its apparent width or location disagrees in a second drawing. Show step by step how to mark landmarks spanning depth, reserve unused marks, compare residual directions, check sensitivity to landmark placements, and choose among changing a camera, editing the hull or recording an artistic contradiction. Use the 1,280 m length as a scale anchor only where warranted.
4. Specify a small, blind benchmark that uses Blender renders of a known mesh. Hide the generating camera from the fit, inject realistic annotation/cropping uncertainty, and describe how to assess pose and projected feature error independently. Explain what benchmark thresholds transfer to drawings and what should remain an artist's judgment.

Return a ranked method, a short procedure for Blender or Python, one repeatable experiment and clear stop conditions. Prefer official software documentation, original papers, maintained code and verified examples. Link directly to key claims. Give versions, license, cost or hardware requirements for tools you propose, and label unsupported assumptions. This report covers camera and image alignment. Mesh construction and material matching have separate briefs.

Return a Markdown report. Start it with this exact routing header, followed by the report title:

---
research_console_project: aquila-code-dark
research_console_key: research-battleship-camera-registration-v1:run-1
---
