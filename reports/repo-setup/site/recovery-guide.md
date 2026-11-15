# Development and recovery

## Existing commands

Run in the repository root. Preserve `package-lock.json`; use `npm ci` on a fresh checkout.
Dependencies already existed during setup; no application package was upgraded or reinstalled.
Record the tested Node/npm versions in the setup receipt; a successful build does not establish version support.

| Purpose | Command |
| --- | --- |
| Browser development server | `npm run serve:dev` |
| Angular and Electron development build | `npm run build:dev` |
| Angular and Electron production build | `npm run build:prod` |
| Electron development app | `npm run electron:dev` |
| Electron production app | `npm run electron:prod` |
| Desktop packaging | `npm run package` or `npm run make` |

Build output is `dist/aquila-code-dark/`; it is not source and stays ignored.
Browser and desktop runtime checks are separate from TypeScript/Angular compilation. Packaging is not part of repository setup.
The existing package has no npm test script. Setup safety tests run with:

```powershell
.\.venv-tools\Scripts\python.exe -B -m unittest discover -s scripts/tests -v
py -B scripts/check-startup-context.py --handoff Docs/Handoffs/HANDOFF_2026-11-14_setup.md
```

## Recover a checkout

1. Obtain source from `git@github.com:jacobmott/AquilaCodeDark.git` at the intended revision.
2. Read the shared core and task index. Create the isolated storage-helper environment per [storage](storage.md).
3. Restore an approved manifest into a new empty recovery directory. Check every hash.
4. Compare against the checkout and place only approved missing asset directories at their recorded relative paths.
   A fresh checkout needs `src/assets`; it also keeps original art in `AllAssets` and authoring data in `Tiled`.
5. Restore deployment configuration separately from an authorized private source. Never put tokens or credentials in Git or the asset bucket.
6. Install locked JavaScript dependencies, then build. Verify the game in a browser and Electron before calling runtime recovery complete.
7. Reinstall the local Git hook with `git config core.hooksPath .githooks`; config does not travel in a clone.
   Configure account memory and shared-service paths deliberately on another machine; loopback bindings are machine-local.

## Deployment boundary

The live site is `https://aquilacode.io`, using its own S3 website bucket through CloudFront.
Private asset backup is separate. Do not run upload, CloudFront invalidation, DNS, certificate or hosting-policy changes on a build request.
Deployment needs explicit authorization, review of the build output and exact website destination.
Legacy detailed instructions are retained in the setup history, not the current operating route.
