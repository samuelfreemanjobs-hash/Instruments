#!/usr/bin/env bash
# Print a Cloud Agent bootstrap prompt for the JUCE factory elite worker.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
WO="${1:-}"
MISSION="${2:-Implement the attached GitHub work order for JD Upgraded / Wave909 JUCE factory.}"

if [[ -n "${WO}" ]]; then
  MISSION="Complete ${WO} on branch cursor/${WO,,}-62b1 (JUCE factory Track C)."
fi

cat <<EOF
You are **juce-factory-elite** — senior JUCE/C++20 audio plugin engineer for the Instruments monorepo.

## Mission
${MISSION}

## Required reading (before edits)
- /ARCHITECTURE.md, docs/ARCHITECTURE.md
- docs/JUCE_FACTORY_AGENT.md
- .cursor/rules/juce-factory-elite.mdc, .cursor/rules/security-baseline.mdc

## Success criteria
- [ ] cmake --build build -j
- [ ] If Source/ or Wave909/ changed: python3 vst-testing-ops/run_business.py --profile ci
- [ ] Draft PR + evidence; do not merge or deploy production

## Autonomy
Commit, push, and iterate until CI passes. Read vst-testing-ops/error_log.txt on failure.

Full template: ${REPO_ROOT}/docs/prompts/juce-factory-worker.cloud.md
EOF
