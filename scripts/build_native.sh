#!/usr/bin/env bash
# Builds the game as a native desktop binary (macOS/Linux) for fast local
# iteration — no browser, no emscripten. Output: build/native/stock-minimalism
# Usage: scripts/build_native.sh [--rebuild-raylib]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RAYLIB_SRC="${ROOT_DIR}/raylib/src"
BUILD_DIR="${ROOT_DIR}/build/native"
BIN_NAME="stock-minimalism"

mkdir -p "${BUILD_DIR}"

RAYLIB_NATIVE_LIB="${RAYLIB_SRC}/libraylib.a"
if [ ! -f "${RAYLIB_NATIVE_LIB}" ] || [ "${1:-}" = "--rebuild-raylib" ]; then
  echo "Building raylib for the native platform ..."
  make -C "${RAYLIB_SRC}" PLATFORM=PLATFORM_DESKTOP -B
else
  echo "Reusing existing ${RAYLIB_NATIVE_LIB} (pass --rebuild-raylib to force)."
fi

# macOS needs these frameworks linked; Linux needs the X11/GL/pthread/dl/m stack.
LINK_FLAGS=()
case "$(uname -s)" in
  Darwin)
    LINK_FLAGS=(-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL)
    ;;
  Linux)
    LINK_FLAGS=(-lGL -lm -lpthread -ldl -lrt -lX11)
    ;;
  *)
    echo "Unrecognized platform $(uname -s); attempting a plain link." >&2
    ;;
esac

echo "Compiling src/*.c -> ${BUILD_DIR}/${BIN_NAME} ..."
SRC_FILES=("${ROOT_DIR}"/src/*.c)
cc "${SRC_FILES[@]}" \
  -o "${BUILD_DIR}/${BIN_NAME}" \
  -O0 -g -Wall \
  -I"${RAYLIB_SRC}" \
  "${RAYLIB_NATIVE_LIB}" \
  "${LINK_FLAGS[@]}"

echo
echo "Build complete: ${BUILD_DIR}/${BIN_NAME}"
echo "Run it with: scripts/run_local.sh"
