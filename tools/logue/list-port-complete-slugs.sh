#!/usr/bin/env bash
# Print mkII oscillator slugs whose osc.h contains PORT_COMPLETE.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OSC_ROOT="${ROOT}/src/mkii/oscillators"

shopt -s nullglob
for dir in "${OSC_ROOT}"/*/; do
  osc="${dir}osc.h"
  [[ -f "${osc}" ]] || continue
  if rg -q 'PORT_COMPLETE' "${osc}" 2>/dev/null; then
    basename "${dir}"
  fi
done | sort -u
