#!/usr/bin/env bash
# WO-SAAS-018: Full local daily pipeline (regression + build + optional Cloud Agent).
# Usage:
#   ./run-factory-daily.sh                    # regression + build + lint; launch agent if CURSOR_API_KEY set
#   ./run-factory-daily.sh --regression-only
#   ./run-factory-daily.sh --dry-run          # print agent prompt only
#   ./run-factory-daily.sh --launch-only      # skip npm (agent only)
#   ./run-factory-daily.sh --theme loops_patterns
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
WEB="$ROOT/disklordz/website"
TRIGGER="$ROOT/disklordz/automation/scripts/trigger_cursor_factory_daily_agent.sh"

REGRESSION_ONLY=0
DRY_RUN=0
LAUNCH_ONLY=0
SKIP_AGENT=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --regression-only) REGRESSION_ONLY=1; shift ;;
    --dry-run) DRY_RUN=1; shift ;;
    --launch-only) LAUNCH_ONLY=1; shift ;;
    --skip-agent) SKIP_AGENT=1; shift ;;
    --theme)
      export FACTORY_DAILY_THEME="$2"
      shift 2
      ;;
    -h | --help)
      sed -n '2,12p' "$0"
      exit 0
      ;;
    *)
      echo "unknown option: $1" >&2
      exit 1
      ;;
  esac
done

if [[ "$DRY_RUN" -eq 1 ]]; then
  exec bash "$TRIGGER" --dry-run ${FACTORY_DAILY_THEME:+--theme "$FACTORY_DAILY_THEME"}
fi

if [[ "$LAUNCH_ONLY" -eq 0 ]]; then
  echo "==> Factory daily: npm ci + regression + build + lint"
  cd "$WEB"
  npm ci
  npm run factory:dsp-regression
  if [[ "$REGRESSION_ONLY" -eq 1 ]]; then
    echo "==> regression-only: done"
    exit 0
  fi
  npm run build
  npm run lint
  echo "==> Factory daily: website checks passed"
fi

if [[ "$SKIP_AGENT" -eq 1 ]]; then
  exit 0
fi

echo "==> Factory daily: Cloud Agent launch (optional)"
agent_args=()
[[ -n "${FACTORY_DAILY_THEME:-}" ]] && agent_args+=(--theme "$FACTORY_DAILY_THEME")
bash "$TRIGGER" "${agent_args[@]}"
