# stock-minimalism

NYU Game Design, Week 1 "Minimalism" assignment (group 7 — Lucy Zheng & Steven Li).

## Concept

short press for purchase, long hold for sold, and the layout could be drawed boxed representing the stack going up & down
Colored with red & green

![Concept art](assets/mockup.png)

## Constrains

- one-button input only
- simple geometric shapes only (circles/squares/triangles)
- must run in a browser
- Zero AI

## Tech stack

C + [raylib](https://www.raylib.com/) compiled to WebAssembly via [emscripten](https://emscripten.org/)

## Project layout

```
src/main.c          game source
assets/             textures/images bundled into the build (see assets/README.md)
raylib/             raylib source, git submodule pinned to release 6.0
emsdk/              emscripten SDK (gitignored, installed locally — see below)
build/web/          build output: index.html + .js + .wasm (gitignored)
scripts/
  setup_emsdk.sh    clone + install + activate emsdk (run once)
  build_web.sh      build raylib for web (if needed) + compile src/main.c
  serve.sh          serve build/web/ over HTTP for local testing
```

## Setup

```bash
git clone --recurse-submodules https://github.com/StevenLi-phoenix/stock-minimalism
cd stock-minimalism
scripts/setup_emsdk.sh   # downloads and activates the emscripten toolchain
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
[stevenli-phoenix-work.itch.io/stock-minimalism](https://stevenli-phoenix-work.itch.io/stock-minimalism)
via [butler](https://itch.io/docs/butler/) on every push to `main`.

## Notes

- Textures/images go in `assets/` (see `assets/README.md`) — `build_web.sh`
  auto-detects it and adds `--preload-file` when it has content, so no
  build changes are needed as art gets added.
- If the game loop ever needs a blocking wait, keep `-s ASYNCIFY` in
  `scripts/build_web.sh` (already enabled) — browsers are single-threaded
  and can't truly block.
