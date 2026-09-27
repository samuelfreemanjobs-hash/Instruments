# shellcheck shell=bash
# Shared helpers for Disklordz SaaS scripts.
set -euo pipefail

WEBSITE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

log() { printf '==> %s\n' "$*"; }
warn() { printf 'WARN: %s\n' "$*" >&2; }
die() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "Missing command: $1"
}

require_env() {
  local name="$1"
  local hint="${2:-}"
  if [ -z "${!name:-}" ]; then
    if [ -n "$hint" ]; then
      die "Set $name — $hint"
    fi
    die "Set $name"
  fi
}

optional_env() {
  local name="$1"
  if [ -n "${!name:-}" ]; then
    echo "  OK   $name"
  else
    echo "  —    $name (optional)"
  fi
}

load_go_live_env() {
  local file="${DISKLORDZ_ENV_FILE:-$WEBSITE_ROOT/.env.go-live}"
  if [ -f "$file" ]; then
    log "Loading env from $file"
    set -a
    # shellcheck disable=SC1090
    source "$file"
    set +a
  fi
}

cd_website() {
  cd "$WEBSITE_ROOT"
}
