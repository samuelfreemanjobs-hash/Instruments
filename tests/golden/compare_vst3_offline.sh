#!/usr/bin/env bash
# Render one manifest row via OfflineRender (in-process) and Vst3OfflineRender (bundle), then SpectralDiff.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/OfflineRender"
VST3_RENDER="${ROOT}/build/Vst3OfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
BUNDLE="${ROOT}/build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3"
MANIFEST="${ROOT}/tests/golden/manifest.tsv"

for bin in "$RENDER" "$VST3_RENDER" "$DIFF"; do
  if [[ ! -x "$bin" ]]; then
    echo "Missing $bin — cmake --build build -j --target OfflineRender Vst3OfflineRender SpectralDiff JDUpgraded_VST3" >&2
    exit 1
  fi
done
if [[ ! -d "$BUNDLE" ]]; then
  echo "Missing VST3 bundle: $BUNDLE" >&2
  exit 1
fi

IFS=$'\t' read -r _filename program note velocity seconds sample_rate < <(grep -v '^#' "$MANIFEST" | head -1)
filename="${_filename# }"
out="${ROOT}/tests/golden/${filename}"
tmp="${ROOT}/build/vst3_parity_${filename}"
tmp_vst3="${ROOT}/build/vst3_parity_vst3_${filename}"

"$RENDER" "$tmp" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
"$VST3_RENDER" "$BUNDLE" "$tmp_vst3" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
"$DIFF" "$tmp" "$tmp_vst3" --max-rms-db -35 --max-spectral-db 2.0
echo "VST3 bundle parity OK for ${filename}"
