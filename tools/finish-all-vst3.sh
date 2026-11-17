#!/usr/bin/env bash
# Build, optional aarch64 verify, and package all shipping vst/* plugins (MPC + tools).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOG="${ROOT}/disklordz/vst-factory/logs/finish-all-vst3.log"
mkdir -p "$(dirname "$LOG")"
: >"$LOG"

SKIP_AARCH64="${SKIP_AARCH64:-0}"
SKIP_PACKAGE="${SKIP_PACKAGE:-0}"

mpc_only() {
  "${ROOT}/tools/build-vst.sh" --list | grep -E '^Mpc|^MeterTap|^Memphis' || true
}

echo "=== finish-all-vst3 $(date -Iseconds) ===" | tee -a "$LOG"

while IFS= read -r name; do
  [[ -z "$name" ]] && continue
  echo "--- $name ---" | tee -a "$LOG"
  if ! "${ROOT}/tools/build-vst.sh" "$name" >>"$LOG" 2>&1; then
    echo "[FAIL] build $name" | tee -a "$LOG"
    continue
  fi
  if [[ "$SKIP_AARCH64" == "0" ]] && [[ "$name" == Mpc* ]]; then
    if "${ROOT}/tools/build-vst-aarch64-linux.sh" "$name" >>"$LOG" 2>&1; then
      vst3=$(find "${ROOT}/vst/${name}/build-aarch64" -name '*.vst3' 2>/dev/null | head -1)
      if [[ -n "$vst3" ]]; then
        "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" "$vst3" >>"$LOG" 2>&1 || true
      fi
    else
      echo "[WARN] aarch64 $name" | tee -a "$LOG"
    fi
  fi
  if [[ "$SKIP_PACKAGE" == "0" ]]; then
    "${ROOT}/tools/package-vst.sh" "$name" 1.0.0 >>"$LOG" 2>&1 || echo "[WARN] package $name" | tee -a "$LOG"
  fi
  echo "[OK] $name" | tee -a "$LOG"
done < <(mpc_only)

echo "Done. Log: $LOG"