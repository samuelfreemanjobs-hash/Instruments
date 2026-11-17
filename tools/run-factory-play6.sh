#!/usr/bin/env bash
# Wrapper — see disklordz/vst-factory/scripts/run-factory-play6.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec "${ROOT}/disklordz/vst-factory/scripts/run-factory-play6.sh" "$@"