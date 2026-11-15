#!/usr/bin/env bash
# Disklordz VST UI agent bootstrap: Google Stitch CLI on Cloud and dev machines.
# Global npm to /usr often needs root; use ~/.npm-global and optional /usr/local/bin symlink.
set -euo pipefail

PREFIX="${STITCH_NPM_PREFIX:-$HOME/.npm-global}"
mkdir -p "$PREFIX"
export PATH="$PREFIX/bin:$PATH"
npm config set prefix "$PREFIX"

if ! command -v stitch >/dev/null 2>&1; then
  npm install -g @google/stitch
fi

if [[ -w /usr/local/bin ]] || command -v sudo >/dev/null 2>&1; then
  if [[ -w /usr/local/bin ]]; then
    ln -sf "$PREFIX/bin/stitch" /usr/local/bin/stitch
  elif sudo -n true 2>/dev/null; then
    sudo ln -sf "$PREFIX/bin/stitch" /usr/local/bin/stitch
  fi
fi

stitch --version