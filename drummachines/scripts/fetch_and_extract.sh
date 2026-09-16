#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CACHE="${ROOT}/_cache"
SAMPLES="${ROOT}/samples"
ITEM_ID="ultimate-vintage-drum-machines-sample-pack"
FILE_NAME="Ultimate Vintage Drum Machines Sample Pack by Onibaku.7z"
URL="https://archive.org/download/${ITEM_ID}/Ultimate%20Vintage%20Drum%20Machines%20Sample%20Pack%20by%20Onibaku.7z"
ARCHIVE="${CACHE}/${FILE_NAME}"

mkdir -p "${CACHE}" "${SAMPLES}"

if [[ ! -f "${ARCHIVE}" ]]; then
  echo "Downloading (~5.5 GB) to ${ARCHIVE}"
  echo "URL: ${URL}"
  curl -fL --retry 5 --retry-delay 5 -C - -o "${ARCHIVE}" "${URL}"
else
  echo "Using existing archive: ${ARCHIVE}"
fi

if ! python3 -c "import py7zr" 2>/dev/null; then
  echo "Installing py7zr for user..."
  python3 -m pip install --user py7zr
fi

echo "Extracting to ${SAMPLES} ..."
python3 "${ROOT}/scripts/extract_7z.py" "${ARCHIVE}" "${SAMPLES}"

echo "Done. Contents:"
find "${SAMPLES}" -maxdepth 2 -type d | head -40
