#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
HUMAN=0
[[ "${1:-}" == "--human" ]] && HUMAN=1

ok() { [[ "$HUMAN" -eq 1 ]] && echo "  OK: $1" || true; }
warn() { echo "  WARN: $1"; }
fail() { echo "  FAIL: $1"; ERR=1; }

ERR=0
[[ "$HUMAN" -eq 1 ]] && echo "APC system check"

command -v git >/dev/null && ok git || fail "git missing"
command -v cmake >/dev/null && ok cmake || warn "cmake missing (install for VST builds)"
command -v make >/dev/null && ok make || warn "make missing"
command -v python3 >/dev/null && ok python3 || warn "python3 missing"
command -v jq >/dev/null && ok jq || warn "jq missing (phase state updates)"

if [[ -d "$ROOT/vst/JUCE" ]]; then
  ok "vst/JUCE present"
else
  warn "vst/JUCE missing — run: git submodule update --init vst/JUCE"
fi

[[ -f "$ROOT/apc.config.json" ]] && ok apc.config.json || warn "run: bin/apc setup"

exit "$ERR"
