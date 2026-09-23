#!/usr/bin/env bash
set -euo pipefail

export VST3_OFFLINE_RENDER_BIN="${VST3_OFFLINE_RENDER_BIN:-/usr/local/bin/Vst3OfflineRender}"

if [[ $# -eq 0 ]] || [[ "${1:-}" == "--help" ]] || [[ "${1:-}" == "-h" ]]; then
  cat <<'EOF'
instruments-multisample container

Mount volumes:
  /plugins  — folder containing .vst3 bundles (read-only)
  /rom      — optional Gearmulator ROM / SysEx dumps (read-only)
  /out      — WAV output + manifest.tsv

Environment:
  GEARMULATOR_ROM_DIR  default /rom (upstream RomLoader search paths)

Example:
  docker run --rm \
    -v "$HOME/plugins:/plugins:ro" \
    -v "$HOME/rom:/rom:ro" \
    -v "$PWD/out:/out" \
    instruments-multisample \
    --plugin /plugins/JE-8086.vst3 --name jp001 --program 0

Passes remaining args to render_multisample_grid.sh (--out-dir defaults to /out).
EOF
  exit 0
fi

has_out_dir=false
for arg in "$@"; do
  if [[ "${arg}" == "--out-dir" ]]; then
    has_out_dir=true
    break
  fi
done

mkdir -p /out
if [[ "${has_out_dir}" == false ]]; then
  set -- --out-dir /out "$@"
fi

exec /usr/local/bin/render_multisample_grid.sh "$@"
