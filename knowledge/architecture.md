# Existing application

Source inspection, 2026-11-14. This is an existing game/app, not a new product scaffold.
The package declares Angular 18, Phaser 3, Rapier 2D, Spine/Phaser and Electron 32.
Exact resolved dependencies belong to `package-lock.json`; do not upgrade as an incidental setup action.

- `src/app/`: Angular UI, routes and ship views. SVG files can be Angular source templates.
- `src/game/`: Phaser scenes and gameplay. Asset preload uses `src/assets/` through the Angular build.
- `electron/`: desktop entry point, compiled after the Angular build by the npm build commands.
- `src/assets/`, `AllAssets/`, `Tiled/`: ignored runtime media, source art and map-authoring data.
- `public/`: static build inputs. The pre-existing favicon is a narrow hash-pinned Git exception.
- `configs/`: legacy configuration helpers; its older README references a separate AquilaCode application.
  Do not run those helpers until their actual target paths and inputs are reviewed for this repo.

Deployment uses the separate website bucket and CloudFront distribution for `aquilacode.io`.
Private backup setup does not deploy or alter game behavior.
