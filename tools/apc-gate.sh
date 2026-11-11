#!/usr/bin/env bash
# Verify APC phase prerequisites before allowing impl/ship.
# Usage: tools/apc-gate.sh <PluginName> <phase>
#   phase: plan | design | impl | ship

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=scripts/lib/apc-paths.sh
source "$ROOT/scripts/lib/apc-paths.sh"

NAME="${1:-}"
PHASE="${2:-}"
[[ -n "$NAME" && -n "$PHASE" ]] || {
  echo "Usage: $0 <PluginName> <plan|design|impl|ship>" >&2
  exit 1
}

apc_load_paths
PLUGIN="$(apc_plugin_path "$NAME")"
STATUS="$PLUGIN/status.json"

die() { echo "APC gate FAILED ($PHASE): $*" >&2; exit 1; }
ok() { echo "APC gate OK ($PHASE) for $NAME"; }

[[ -d "$PLUGIN" ]] || die "missing plugins/$NAME — run /apc-dream $NAME"
[[ -f "$STATUS" ]] || die "missing status.json"

check_file() {
  [[ -f "$PLUGIN/$1" ]] || die "missing $1 (complete prior phase)"
}

case "$PHASE" in
  plan)
    check_file ".ideas/creative-brief.md"
    check_file ".ideas/parameter-spec.md"
    ok
    ;;
  design)
    bash "$ROOT/tools/apc-gate.sh" "$NAME" plan
    check_file ".ideas/architecture.md"
    ok
    ;;
  impl)
    bash "$ROOT/tools/apc-gate.sh" "$NAME" design
    check_file ".ideas/design-spec.md"
    ok
    ;;
  ship)
    bash "$ROOT/tools/apc-gate.sh" "$NAME" impl
    vst="$(apc_vst_path "$NAME")"
    [[ -d "$vst" ]] || die "missing vst/$NAME — run /apc-impl"
    find "$vst/build" -name '*.vst3' 2>/dev/null | grep -q . || die "no .vst3 — run /apc-test"
    ok
    ;;
  *)
    die "unknown phase $PHASE"
    ;;
esac
