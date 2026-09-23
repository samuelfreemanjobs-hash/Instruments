#!/usr/bin/env bash
# Batch-render a note/velocity grid from any Linux VST3 (Gearmulator, Wave909 smoke test, etc.)
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER_BIN="${REPO_ROOT}/build/Vst3OfflineRender"
PLUGIN=""
OUT_DIR="${REPO_ROOT}/gearmulator-lane/multisamples/out"
PROGRAM=0
NOTE_START=36
NOTE_END=84
NOTE_STEP=3
VELOCITY=100
SECONDS=3.0
SAMPLE_RATE=48000
NAME="capture"

usage() {
  echo "Usage: $0 --plugin <path/to/Plugin.vst3> [options]" >&2
  echo "  --out-dir DIR   (default: gearmulator-lane/multisamples/out)" >&2
  echo "  --program N     host program/preset index" >&2
  echo "  --note-start N  --note-end N  --note-step N" >&2
  echo "  --velocity N    --seconds F  --sample-rate N  --name LABEL" >&2
  exit 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --plugin) PLUGIN="$2"; shift 2 ;;
    --out-dir) OUT_DIR="$2"; shift 2 ;;
    --program) PROGRAM="$2"; shift 2 ;;
    --note-start) NOTE_START="$2"; shift 2 ;;
    --note-end) NOTE_END="$2"; shift 2 ;;
    --note-step) NOTE_STEP="$2"; shift 2 ;;
    --velocity) VELOCITY="$2"; shift 2 ;;
    --seconds) SECONDS="$2"; shift 2 ;;
    --sample-rate) SAMPLE_RATE="$2"; shift 2 ;;
    --name) NAME="$2"; shift 2 ;;
    -h|--help) usage ;;
    *) echo "Unknown arg: $1" >&2; usage ;;
  esac
done

[[ -n "${PLUGIN}" ]] || usage
[[ -x "${RENDER_BIN}" ]] || {
  echo "Build Vst3OfflineRender first: cmake --build build -j --target Vst3OfflineRender" >&2
  exit 1
}

SESSION="${NAME}_p${PROGRAM}_v${VELOCITY}"
DEST="${OUT_DIR}/${SESSION}"
mkdir -p "${DEST}"

MANIFEST="${DEST}/manifest.tsv"
echo -e "program\tnote\tvelocity\tseconds\tsample_rate\twav" > "${MANIFEST}"

for ((note=NOTE_START; note<=NOTE_END; note+=NOTE_STEP)); do
  wav="${DEST}/note_$(printf '%03d' "${note}")_vel_$(printf '%03d' "${VELOCITY}").wav"
  "${RENDER_BIN}" \
    --plugin "${PLUGIN}" \
    --out "${wav}" \
    --program "${PROGRAM}" \
    --note "${note}" \
    --velocity "${VELOCITY}" \
    --seconds "${SECONDS}" \
    --sampleRate "${SAMPLE_RATE}"
  echo -e "${PROGRAM}\t${note}\t${VELOCITY}\t${SECONDS}\t${SAMPLE_RATE}\t${wav}" >> "${MANIFEST}"
done

echo "Wrote grid under ${DEST}"
echo "Manifest: ${MANIFEST}"
