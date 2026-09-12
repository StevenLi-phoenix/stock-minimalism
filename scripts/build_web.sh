#!/usr/bin/env bash
# Builds the game for the browser: raylib (web target) + src/*.c -> build/web/index.html
# Usage: scripts/build_web.sh [--rebuild-raylib]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EMSDK_DIR="${ROOT_DIR}/emsdk"
RAYLIB_SRC="${ROOT_DIR}/raylib/src"
BUILD_DIR="${ROOT_DIR}/build/web"

if command -v emcc >/dev/null 2>&1; then

  echo "Using emcc already on PATH: $(command -v emcc)"
elif [ -f "${EMSDK_DIR}/emsdk_env.sh" ]; then
  source "${EMSDK_DIR}/emsdk_env.sh"
else
  echo "emcc not found and no local emsdk at ${EMSDK_DIR}. Run scripts/setup_emsdk.sh first." >&2
  exit 1
fi

mkdir -p "${BUILD_DIR}"

RAYLIB_WEB_LIB="${RAYLIB_SRC}/libraylib.web.a"
if [ ! -f "${RAYLIB_WEB_LIB}" ] || [ "${1:-}" = "--rebuild-raylib" ]; then
  echo "Building raylib for PLATFORM_WEB ..."
  make -C "${RAYLIB_SRC}" PLATFORM=PLATFORM_WEB -B
else
  echo "Reusing existing ${RAYLIB_WEB_LIB} (pass --rebuild-raylib to force)."
fi

# Bundle assets/ into the WASM build if it has anything in it (e.g. textures),
# so main.c can LoadTexture("assets/whatever.png") without further build changes.
ASSET_ARGS=()
ASSETS_DIR="${ROOT_DIR}/assets"
if [ -d "${ASSETS_DIR}" ] && [ -n "$(find "${ASSETS_DIR}" -type f ! -name 'README.md' -print -quit)" ]; then
  echo "Bundling ${ASSETS_DIR} into the build (--preload-file) ..."
  ASSET_ARGS=(--preload-file "${ASSETS_DIR}@assets")
fi

echo "Compiling src/*.c -> ${BUILD_DIR}/index.html ..."
SRC_FILES=("${ROOT_DIR}"/src/*.c)
emcc "${SRC_FILES[@]}" \
  -o "${BUILD_DIR}/index.html" \
  -Os -Wall \
  -I"${RAYLIB_SRC}" \
  "${RAYLIB_WEB_LIB}" \
  -DPLATFORM_WEB \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s ALLOW_MEMORY_GROWTH=1 \
  --shell-file "${ROOT_DIR}/web/shell.html" \
  "${ASSET_ARGS[@]+"${ASSET_ARGS[@]}"}"

echo
echo "Build complete: ${BUILD_DIR}/index.html"
echo "Serve it with: scripts/serve.sh"
