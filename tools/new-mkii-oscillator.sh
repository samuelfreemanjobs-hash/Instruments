#!/usr/bin/env bash
# Scaffold a new NTS-1 mkII oscillator from template_osc (mkII-only lane).
#
# Usage:
#   ./tools/new-mkii-oscillator.sh <slug> ["Display Name"]
#
# Example:
#   ./tools/new-mkii-oscillator.sh memphis_dust_sub "Dust Sub"

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SLUG="${1:-}"
DISPLAY="${2:-$SLUG}"

if [[ -z "${SLUG}" ]] || [[ ! "${SLUG}" =~ ^[a-z][a-z0-9_]*$ ]]; then
  echo "Usage: $0 <slug> [display_name]" >&2
  echo "  slug: snake_case, e.g. memphis_dust_sub" >&2
  exit 1
fi

TEMPLATE="${ROOT}/src/mkii/oscillators/template_osc"
DEST="${ROOT}/src/mkii/oscillators/${SLUG}"

if [[ -e "${DEST}" ]]; then
  echo "Refusing to overwrite existing ${DEST}" >&2
  exit 1
fi

cp -a "${TEMPLATE}" "${DEST}"
rm -f "${DEST}/.agent/status.json.example"

mkdir -p "${DEST}/.agent"
cat >"${DEST}/.agent/status.json" <<EOF
{
  "slug": "${SLUG}",
  "current_phase": "scaffold",
  "setup_completed": false,
  "phases_completed": []
}
EOF

cat >"${DEST}/config.mk" <<EOF
##############################################################################
# Configuration for Makefile — ${SLUG}
#

PROJECT := ${SLUG}
PROJECT_TYPE := osc

UCSRC = header.c
UCXXSRC = unit.cc

UINCDIR =
ULIBDIR =
ULIBS = -lm
UDEFS =
EOF

# Short display name for hardware (max 15 chars in header)
SHORT="${DISPLAY:0:15}"
python3 - <<PY
from pathlib import Path
p = Path("${DEST}/header.c")
text = p.read_text(encoding="utf-8")
text = text.replace('name = "template"', 'name = "${SHORT}"')
p.write_text(text, encoding="utf-8")
PY

echo "Created mkII oscillator: ${DEST}"
echo "Next: edit osc.h (mark PORT_COMPLETE), header.c params, .agent/brief.md"
echo "Eval:  ./tools/osc-eval-mkii.sh ${SLUG} --static-only"
