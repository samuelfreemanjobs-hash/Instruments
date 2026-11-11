#!/usr/bin/env bash
# DPCS Phase 4-style gate for Phase 1 PRD completeness.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
req=(
  "$ROOT/DPCS_PHASE0_CONSTITUTION.md"
  "$ROOT/DPCS_PHASE1_PRD.md"
  "$ROOT/.ideas/parameter-spec.md"
  "$ROOT/.ideas/creative-brief.md"
  "$ROOT/status.json"
)
for f in "${req[@]}"; do
  if [[ ! -f "$f" ]]; then
    echo "missing: $f" >&2
    exit 1
  fi
done
python3 - <<'PY' "$ROOT/status.json"
import json, sys
p = sys.argv[1]
with open(p, encoding="utf-8") as f:
    s = json.load(f)
assert s.get("dpcs_phase", 0) >= 1, "dpcs_phase must be >= 1"
assert s.get("validation", {}).get("creative_brief_exists") is True
assert s.get("validation", {}).get("parameter_spec_exists") is True
print("MeterTap Phase 1 PRD OK")
PY
exit 0
