#!/usr/bin/env bash
# Serves build/web/ over HTTP (WASM won't load over file:// due to CORS).
# Usage: scripts/serve.sh [PORT]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PORT="${1:-8000}"

cd "${ROOT_DIR}/build/web"

# force rebuild everytime
bash "${ROOT_DIR}/scripts/build_web.sh"

echo "Serving ${ROOT_DIR}/build/web at http://localhost:${PORT}/index.html"
python3 -m http.server "${PORT}"
