#!/usr/bin/env bash
# Collect WAVE-909 release artefacts from a Release build tree.
set -euo pipefail

W909_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPO="$(cd "${W909_ROOT}/.." && pwd)"
BUILD="${REPO}/build"
OUT="${W909_ROOT}/Release"
ART="${BUILD}/Wave909/Wave909_artefacts/Release"

mkdir -p "$OUT"

cp -a "${ART}/VST3/WAVE-909.vst3" "$OUT/" 2>/dev/null || cp -a "${ART}/VST3/"*.vst3 "$OUT/" 
cp -a "${ART}/CLAP/WAVE-909.clap" "$OUT/" 2>/dev/null || true
cp -a "${ART}/Standalone/WAVE-909" "$OUT/" 2>/dev/null || true

cp "${W909_ROOT}/README.md" "$OUT/"
cp "${W909_ROOT}/CHANGELOG.md" "$OUT/" 2>/dev/null || true
cp "${W909_ROOT}/Docs/USER_MANUAL.md" "$OUT/" 2>/dev/null || true

( cd "$OUT" && find . -type f -print0 | sort -z | xargs -0 sha256sum ) > "${OUT}/SHA256SUMS"

echo "Release staged in ${OUT}"
