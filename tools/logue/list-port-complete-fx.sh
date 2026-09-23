#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
FX_ROOT="${ROOT}/src/mkii/fx"
shopt -s nullglob
for dir in "${FX_ROOT}"/*/; do
  fx="${dir}fx.h"
  [[ -f "${fx}" ]] || continue
  if rg -q 'PORT_COMPLETE' "${fx}" 2>/dev/null; then
    basename "${dir}"
  fi
done | sort -u
