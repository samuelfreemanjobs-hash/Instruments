#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
export PYTHONUNBUFFERED=1
python3 tools/cull_bank_audio_diversity.py
python3 tools/verify_cross_category_gate.py
python3 tests/test_trap_lord_audio_qa.py
python3 tools/package_audition_artifacts.py
python3 tools/package_mpc_live_examples.py
python3 tools/split_mpc_to_artifacts.py
python3 -c "import json; b=json.load(open('assets/factory/preset_bank.json')); print('BANK',len(b['presets'])); print(json.dumps(b.get('achievedQuotas',{}),indent=2))"
