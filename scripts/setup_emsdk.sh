#!/usr/bin/env bash
# Clones and activates the Emscripten SDK into ./emsdk (gitignored — not committed).
# Idempotent: safe to re-run.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EMSDK_DIR="${ROOT_DIR}/emsdk"

if [ ! -d "${EMSDK_DIR}" ]; then
  echo "Cloning emsdk into ${EMSDK_DIR} ..."
  git clone https://github.com/emscripten-core/emsdk.git "${EMSDK_DIR}"
else
  echo "emsdk already present at ${EMSDK_DIR}, pulling latest ..."
  git -C "${EMSDK_DIR}" pull
fi

cd "${EMSDK_DIR}"
./emsdk install latest
./emsdk activate latest

echo
echo "emsdk installed and activated."
echo "Run 'source ${EMSDK_DIR}/emsdk_env.sh' in your shell before building,"
echo "or just use scripts/build_web.sh which sources it automatically."
