#!/usr/bin/env bash
# One entry point for NTS-1 mkII scaffold + build automation.
#
# Usage:
#   ./tools/mkii-automate.sh bootstrap          # clone SDK, submodules, ARM gcc
#   ./tools/mkii-automate.sh scaffold           # regenerate mkII trees (keeps PORT_COMPLETE osc.h)
#   ./tools/mkii-automate.sh build <slug>       # build one .nts1mkiiunit
#   ./tools/mkii-automate.sh build-all          # scaffold + build every unit
#   ./tools/mkii-automate.sh docker <slug>      # symlink into logue-sdk for Docker build
#   ./tools/mkii-automate.sh all                # bootstrap + scaffold + build-all
#
# Environment:
#   LOGUE_SDK  — optional; default: <repo>/.deps/logue-sdk

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=tools/mkii/lib.sh
source "${ROOT}/tools/mkii/lib.sh"

CMD="${1:-}"
shift || true

usage() {
  sed -n '3,12p' "$0"
  exit "${1:-0}"
}

cmd_bootstrap() {
  local sdk gcc_ok=0
  sdk="$(mkii_ensure_logue_sdk_clone)"
  mkii_init_submodules "${sdk}"
  if mkii_ensure_arm_gcc "${sdk}"; then
    gcc_ok=1
  fi
  export LOGUE_SDK="${sdk}"
  echo ""
  echo "Bootstrap complete (LOGUE_SDK=${sdk})."
  if [[ "${gcc_ok}" -eq 0 ]]; then
    echo "Toolchain not installed — set MKII_SKIP_GCC=1 to continue, or install gcc under tools/gcc/."
  else
    echo "  export LOGUE_SDK=${sdk}"
  fi
}

cmd_scaffold() {
  # Always refresh generated files; completed ports keep osc.h (PORT_COMPLETE).
  mkii_scaffold_all --force
}

cmd_build() {
  local slug="${1:-}"
  if [[ -z "${slug}" ]]; then
    echo "Usage: $0 build <mkii-slug>" >&2
    exit 1
  fi
  local sdk
  sdk="$(mkii_resolve_logue_sdk)" || {
    echo "Run: $0 bootstrap" >&2
    exit 1
  }
  export LOGUE_SDK="${sdk}"
  "${ROOT}/tools/build-mkii.sh" "${slug}"
}

cmd_build_all() {
  local sdk gcc_bin
  sdk="$(mkii_resolve_logue_sdk)" || sdk="$(mkii_ensure_logue_sdk_clone)"
  mkii_init_submodules "${sdk}"
  mkii_ensure_arm_gcc "${sdk}" || true
  export LOGUE_SDK="${sdk}"
  mkii_scaffold_all --force
  gcc_bin="$(mkii_gcc_mkii_bin "${sdk}")"
  if [[ ! -x "${gcc_bin}" ]]; then
    echo "No mkII gcc at ${gcc_bin}; skipping compile. Run bootstrap on a machine with ARM gcc, or use docker." >&2
    exit 0
  fi
  "${ROOT}/tools/build-all-bass-mkii.sh"
}

cmd_docker() {
  local slug="${1:-tr808_kick_phonk}"
  mkii_docker_link "${slug}"
}

cmd_all() {
  cmd_bootstrap
  cmd_build_all
}

case "${CMD}" in
  bootstrap) cmd_bootstrap ;;
  scaffold) cmd_scaffold "$@" ;;
  build) cmd_build "$@" ;;
  build-all) cmd_build_all ;;
  docker) cmd_docker "$@" ;;
  all) cmd_all ;;
  -h|--help|help|"") usage 0 ;;
  *)
    echo "Unknown command: ${CMD}" >&2
    usage 1
    ;;
esac
