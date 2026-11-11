#!/usr/bin/env bash
# DPCS Phase 4 gate for {{PLUGIN_NAME}}: every verification command exits 0.
# Generated from templates/apc/verify-phase4.template.sh (SOP-VF-06). Add product-specific checks below.
set -euo pipefail
REPO="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$REPO"

step() { echo; echo "== $*"; }

step "APC gate (impl)"
bash tools/apc-gate.sh {{PLUGIN_NAME}} impl

step "Unit tests (JUCE-free factory-lib blocks used by {{PLUGIN_NAME}})"
# e.g. cmake -S vst/factory-lib/analysis/tests -B build/analysis-tests && cmake --build build/analysis-tests && ctest --test-dir build/analysis-tests --output-on-failure

step "Build"
bash tools/build-vst.sh {{PLUGIN_NAME}}

step "Factory QA"
python3 vst-testing-ops/run_business.py --profile factory --plugin {{PLUGIN_NAME}}

echo
echo "{{PLUGIN_NAME}} Phase 4 verify OK"
