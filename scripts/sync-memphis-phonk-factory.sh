#!/usr/bin/env bash
# Regenerate factory-facing hints after MemphisPhonkFactory export/manifest changes.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
echo "Memphis Phonk Factory → vst-factory SKU MemphisPhonkFactory (see product-library.json)"
test -f "${ROOT}/vst/MemphisPhonkFactory/README.md"
echo "Build: ${ROOT}/tools/build-vst.sh MemphisPhonkFactory"
