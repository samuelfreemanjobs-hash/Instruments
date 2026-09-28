#!/usr/bin/env bash
# Create Wine win32 prefix for 32-bit VST2 reference plugins.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
export WINEARCH=win32
export WINEPREFIX="${WINEPREFIX:-$ROOT/wineprefix}"
export WINEDLLOVERRIDES="${WINEDLLOVERRIDES:-mscoree,mshtml=}"

mkdir -p "$ROOT/host" "$ROOT/plugins" "$ROOT/midi" "$ROOT/renders"

need() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "Missing $1. Install: sudo apt-get install -y wine wine32:i386 winetricks xvfb firejail" >&2
    exit 1
  fi
}

need wine
need xvfb-run

if [[ ! -d "$WINEPREFIX/drive_c" ]]; then
  echo "Initializing Wine prefix at $WINEPREFIX (win32)..."
  xvfb-run -a wineboot --init
fi

if command -v winetricks >/dev/null 2>&1; then
  echo "Installing common VST runtimes (vcrun2010, corefonts) — may take a few minutes..."
  xvfb-run -a winetricks -q vcrun2010 corefonts 2>/dev/null || \
    echo "winetricks optional step failed; some old VSTs may still need manual DLLs."
fi

# Symlink plugin folder into Wine VST path
VST_DIR="$WINEPREFIX/drive_c/Program Files/Steinberg/VstPlugins"
mkdir -p "$VST_DIR"
if [[ ! -L "$VST_DIR/junova-ref" ]]; then
  ln -sfn "$ROOT/plugins" "$VST_DIR/junova-ref"
fi

echo "Wine prefix ready: $WINEPREFIX"
echo "Drop 32-bit VST2 DLLs into: $ROOT/plugins/"
echo "Drop VSTHost.exe into: $ROOT/host/VSTHost.exe"
echo "Then: $ROOT/run_vsthost.sh"
