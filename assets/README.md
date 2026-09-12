# assets/

Texture/image files for the game go here (e.g. `assets/spacecraft.png`).

`scripts/build_web.sh` automatically adds `--preload-file assets` to the
emcc command when this directory has files in it, so anything placed here
is bundled into the WASM build and loadable at runtime via raylib's normal
`LoadTexture("assets/whatever.png")` — no other build changes needed.

Keep files small: everything preloaded here is fetched up front before the
game can start, on every page load.
