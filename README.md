# reentry-minimalism

NYU Game Design, Week 1 "Minimalism" assignment (group 7 — Lucy Zheng & Steven Li).

A one-button spacecraft re-entry sim: press-and-release timing on a single
button adjusts angle of attack. Fly too steep and you burn up; too shallow
and you lose energy/range control and overshoot or undershoot the landing
target. Win by arriving at the target with the right remaining energy.

Constraints from the assignment: one-button input only, simple geometric
shapes only (circles/squares/triangles), must run in a browser.

## Tech stack

C + [raylib](https://www.raylib.com/) compiled to WebAssembly via
[emscripten](https://emscripten.org/), so it runs directly in the browser
with no plugin.

## Project layout

```
src/main.c          game source
assets/             textures/images bundled into the build (see assets/README.md)
raylib/             raylib source, git submodule pinned to release 6.0
emsdk/               emscripten SDK (gitignored, installed locally — see below)
build/web/          build output: index.html + .js + .wasm (gitignored)
scripts/
  setup_emsdk.sh    clone + install + activate emsdk (run once)
  build_web.sh      build raylib for web (if needed) + compile src/main.c
  serve.sh          serve build/web/ over HTTP for local testing
```

## Setup (once per machine)

```bash
git clone --recurse-submodules <this repo>
cd reentry-minimalism
scripts/setup_emsdk.sh   # downloads and activates the emscripten toolchain
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

## Build & run

```bash
scripts/build_web.sh     # -> build/web/index.html
scripts/serve.sh         # serves on http://localhost:8000 (WASM needs http://, not file://)
```

Then open http://localhost:8000/index.html in a browser.

Pass `--rebuild-raylib` to `build_web.sh` to force a clean rebuild of the
raylib web library (only needed after changing raylib source/version).

## CI: auto-release to itch.io

`.github/workflows/itch-release.yml` builds the web target and pushes it to
[stevenli-phoenix-work.itch.io/reentry-minimalism](https://stevenli-phoenix-work.itch.io/reentry-minimalism)
via [butler](https://itch.io/docs/butler/) on every push to `main`.

Already configured on this repo (`BUTLER_API_KEY` secret, `ITCH_TARGET`
repo variable = `stevenli-phoenix-work/reentry-minimalism`). To point this
at a different itch.io project or key:

1. Get an API key from https://itch.io/user/settings/api-keys.
2. `gh secret set BUTLER_API_KEY --repo <owner>/<repo>` (paste the key, or
   pipe it in — never put it in a tracked file; `.env` is gitignored for
   local use only).
3. `gh variable set ITCH_TARGET --repo <owner>/<repo> --body "<itch-username>/<game-slug>"`
4. Optional: `gh variable set ITCH_CHANNEL --repo <owner>/<repo> --body "<channel>"`
   (defaults to `web` if unset).

One remaining manual step: on itch.io, open the project's Edit page once
the first build has been pushed and check "This file will be played in
the browser" on the uploaded file — butler doesn't set that flag for you.

The workflow fails fast with a clear error if `ITCH_TARGET` isn't set.

## Notes

- Textures/images go in `assets/` (see `assets/README.md`) — `build_web.sh`
  auto-detects it and adds `--preload-file` when it has content, so no
  build changes are needed as art gets added.
- If the game loop ever needs a blocking wait, keep `-s ASYNCIFY` in
  `scripts/build_web.sh` (already enabled) — browsers are single-threaded
  and can't truly block.
