#!/usr/bin/env bash
# DPCS Phase 1 gate for {{PLUGIN_NAME}}: behaviour docs exist and status.json agrees.
# Generated from templates/apc/verify-phase1.template.sh (SOP-VF-01).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
req=(
  "$ROOT/DPCS_PHASE0_CONSTITUTION.md"
  "$ROOT/.ideas/creative-brief.md"
  "$ROOT/.ideas/parameter-spec.md"
  "$ROOT/status.json"
)
for f in "${req[@]}"; do
  [[ -f "$f" ]] || { echo "missing: $f" >&2; exit 1; }
done
if grep -n '<!--' "$ROOT/.ideas/creative-brief.md" >/dev/null; then
  echo "creative-brief.md still has template comments to fill in" >&2
  exit 1
fi
python3 - "$ROOT/status.json" <<'PY'
import json, sys
s = json.load(open(sys.argv[1], encoding="utf-8"))
assert s.get("dpcs_phase", 0) >= 1, "dpcs_phase must be >= 1"
v = s.get("validation", {})
assert v.get("creative_brief_exists") is True, "validation.creative_brief_exists"
assert v.get("parameter_spec_exists") is True, "validation.parameter_spec_exists"
print("{{PLUGIN_NAME}} Phase 1 PRD OK")
PY
