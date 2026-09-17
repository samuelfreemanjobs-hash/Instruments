#!/usr/bin/env bash
# Disklordz JUCE factory sandbox — deps, build, CI smoke, optional pluginval.
# Safe to run repeatedly (idempotent). Used by Cloud Agent install and local/CI.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

PLUGINVAL_VERSION="${PLUGINVAL_VERSION:-1.0.4}"
PLUGINVAL_URL="https://github.com/Tracktion/pluginval/releases/download/v${PLUGINVAL_VERSION}/pluginval_Linux.zip"
TOOL_DIR="${ROOT}/.sandbox/tools"
PLUGINVAL_BIN="${TOOL_DIR}/pluginval"

log() { echo "[sandbox] $*"; }
die() { echo "[sandbox] error: $*" >&2; exit 1; }

apt_packages() {
  export DEBIAN_FRONTEND=noninteractive
  sudo apt-get update -qq
  sudo apt-get install -y -qq \
    -o Dpkg::Options::="--force-confdef" \
    -o Dpkg::Options::="--force-confold" \
    build-essential g++-12 libstdc++-12-dev \
    libasound2-dev libfreetype6-dev libfontconfig1-dev \
    libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev \
    libxinerama-dev libxext-dev libcurl4-openssl-dev \
    libgtk-3-dev libwebkit2gtk-4.1-dev \
    curl unzip ca-certificates jq
}

cmake_configure() {
  cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
}

cmake_build() {
  cmake --build build -j"$(nproc)"
}

ci_smoke() {
  log "CI smoke (matches .github/workflows/build.yml) ..."
  test -d "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3"
  test -f "build/JDUpgraded_artefacts/Release/CLAP/JD Upgraded.clap"
  test -x "build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded"
  ./build/OfflineRender /tmp/jdupg-a.wav 0 60 100 0.5 44100
  ./build/OfflineRender /tmp/jdupg-b.wav 0 60 100 0.5 44100
  test -s /tmp/jdupg-a.wav
  ./build/SpectralDiff /tmp/jdupg-a.wav /tmp/jdupg-b.wav --max-rms-db -80 --max-spectral-db 0.25
  ./tests/golden/verify_golden.sh
  log "CI smoke passed."
}

fetch_pluginval() {
  mkdir -p "$TOOL_DIR"
  if [[ -x "$PLUGINVAL_BIN" ]]; then
    log "pluginval already present at $PLUGINVAL_BIN"
    return 0
  fi
  log "Downloading pluginval v${PLUGINVAL_VERSION} ..."
  local zip="${TOOL_DIR}/pluginval.zip"
  curl -fsSL "$PLUGINVAL_URL" -o "$zip"
  unzip -qo "$zip" -d "$TOOL_DIR"
  rm -f "$zip"
  if [[ ! -x "$PLUGINVAL_BIN" ]]; then
    # Some zips ship flat binary in TOOL_DIR
    local found
    found="$(find "$TOOL_DIR" -maxdepth 2 -name pluginval -type f | head -1)"
    [[ -n "$found" ]] || die "pluginval binary not found after unzip"
    chmod +x "$found"
    PLUGINVAL_BIN="$found"
  fi
  log "pluginval ready: $PLUGINVAL_BIN"
}

run_pluginval() {
  fetch_pluginval
  local strict="${PLUGINVAL_STRICTNESS:-5}"
  local timeout="${PLUGINVAL_TIMEOUT_MS:-120000}"
  log "pluginval strictness=${strict} on JD Upgraded VST3 ..."
  "$PLUGINVAL_BIN" --validate-in-process --strictness-level "$strict" --timeout-ms "$timeout" \
    --file "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3"
  log "pluginval VST3 passed."
}

build_myfirstplugin() {
  log "Building MyFirstPlugin (subproject) ..."
  cmake -B MyFirstPlugin/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
  cmake --build MyFirstPlugin/build -j"$(nproc)" --target MyFirstPlugin_VST3 MyFirstSynth_VST3
  if [[ "${SANDBOX_PLUGINVAL:-0}" == "1" ]]; then
    fetch_pluginval
    "$PLUGINVAL_BIN" --validate-in-process --strictness-level 5 --timeout-ms 120000 \
      --file "MyFirstPlugin/build/MyFirstPlugin_artefacts/Release/VST3/MyFirstPlugin.vst3"
    "$PLUGINVAL_BIN" --validate-in-process --strictness-level 5 --timeout-ms 120000 \
      --file "MyFirstPlugin/build/MyFirstPlugin_artefacts/Release/VST3/MyFirstSynth.vst3"
  fi
  log "MyFirstPlugin build passed."
}

usage() {
  cat <<'EOF'
Usage: scripts/disklordz-sandbox.sh [options]

Default (no flags): install Linux deps, configure, build JD Upgraded factory, run CI smoke.

Options:
  --deps-only       Apt packages only (no cmake)
  --configure-only  deps + cmake configure
  --ci              deps + configure + build + CI smoke (Cloud Agent install target)
  --pluginval       Also download pluginval and validate JD Upgraded VST3
  --myfirstplugin   Also build MyFirstPlugin VST3 targets
  --full            --ci + --pluginval + --myfirstplugin
  -h, --help        This help

Environment:
  PLUGINVAL_VERSION   (default 1.0.4)
  SANDBOX_PLUGINVAL=1 When set with --myfirstplugin, run pluginval on template plugins

Examples:
  ./scripts/disklordz-sandbox.sh --ci
  ./scripts/disklordz-sandbox.sh --full
EOF
}

main() {
  local do_deps=0 do_configure=0 do_build=0 do_smoke=0 do_pluginval=0 do_mfp=0

  if [[ $# -eq 0 ]]; then
    do_deps=1 do_configure=1 do_build=1 do_smoke=1
  else
    while [[ $# -gt 0 ]]; do
      case "$1" in
        --deps-only) do_deps=1 ;;
        --configure-only) do_deps=1 do_configure=1 ;;
        --ci) do_deps=1 do_configure=1 do_build=1 do_smoke=1 ;;
        --pluginval) do_pluginval=1 ;;
        --myfirstplugin) do_mfp=1 ;;
        --full) do_deps=1 do_configure=1 do_build=1 do_smoke=1 do_pluginval=1 do_mfp=1 ;;
        -h | --help) usage; exit 0 ;;
        *) die "unknown option: $1" ;;
      esac
      shift
    done
    # If only --pluginval/--myfirstplugin, assume build already exists
    if [[ $do_deps -eq 0 && $do_configure -eq 0 && $do_build -eq 0 && $do_smoke -eq 0 ]]; then
      do_build=0
    fi
  fi

  if [[ $do_deps -eq 1 ]]; then
    if ! apt_packages; then
      log "warn: apt reported errors (often fuse3 prompts on reused VMs); continuing if g++-12 is available"
      command -v g++-12 >/dev/null 2>&1 || die "g++-12 not found; fix apt or install build-essential"
    fi
  fi
  [[ $do_configure -eq 1 ]] && cmake_configure
  [[ $do_build -eq 1 ]] && cmake_build
  [[ $do_smoke -eq 1 ]] && ci_smoke
  [[ $do_pluginval -eq 1 ]] && run_pluginval
  [[ $do_mfp -eq 1 ]] && build_myfirstplugin

  log "Done."
}

main "$@"
