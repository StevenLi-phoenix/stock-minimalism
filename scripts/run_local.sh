#!/usr/bin/env bash
# Builds (if needed) and runs the native desktop binary directly — the fast
# loop for local development, no browser/emscripten round-trip required.
# Usage: scripts/run_local.sh [--rebuild-raylib]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN_PATH="${ROOT_DIR}/build/native/stock-minimalism"

bash "${ROOT_DIR}/scripts/build_native.sh" "${1:-}"

echo
echo "Launching ${BIN_PATH} ..."
exec "${BIN_PATH}"
