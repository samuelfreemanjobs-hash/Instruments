#!/usr/bin/env bash
# Shared helpers for NTS-1 mkII scaffold/build scripts.
set -euo pipefail

mkii_repo_root() {
  cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd
}

mkii_default_logue_sdk() {
  local root
  root="$(mkii_repo_root)"
  if [[ -n "${LOGUE_SDK:-}" ]]; then
    echo "${LOGUE_SDK}"
  else
    echo "${root}/.deps/logue-sdk"
  fi
}

mkii_resolve_logue_sdk() {
  local sdk
  sdk="$(mkii_default_logue_sdk)"
  if [[ ! -d "${sdk}/platform/nts-1_mkii" ]]; then
    return 1
  fi
  echo "${sdk}"
}

mkii_ensure_logue_sdk_clone() {
  local sdk
  sdk="$(mkii_default_logue_sdk)"
  if [[ -d "${sdk}/.git" ]]; then
    printf '%s\n' "${sdk}"
    return 0
  fi
  echo "Cloning logue-sdk into ${sdk} ..." >&2
  mkdir -p "$(dirname "${sdk}")"
  git clone --depth 1 https://github.com/korginc/logue-sdk.git "${sdk}"
  printf '%s\n' "${sdk}"
}

mkii_init_submodules() {
  local sdk="$1"
  echo "Initializing logue-sdk submodules (CMSIS ext) ..."
  git -C "${sdk}" submodule update --init platform/ext
}

mkii_gcc_mkii_bin() {
  local sdk="$1"
  echo "${sdk}/tools/gcc/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc"
}

mkii_ensure_arm_gcc() {
  local sdk="$1"
  local gcc_bin installer
  gcc_bin="$(mkii_gcc_mkii_bin "${sdk}")"
  if [[ -x "${gcc_bin}" ]]; then
    echo "ARM gcc OK: ${gcc_bin}"
    return 0
  fi
  if [[ "${MKII_SKIP_GCC:-0}" == "1" ]]; then
    echo "MKII_SKIP_GCC=1 — skipping toolchain install (use Docker: ./tools/mkii-automate.sh docker <slug>)" >&2
    return 0
  fi
  echo "Installing gcc-arm-none-eabi 10.3-2021.10 for mkII ..."
  installer="${sdk}/tools/gcc/get_gcc_10_3-2021_10_linux.sh"
  if [[ ! -f "${installer}" ]]; then
    echo "Missing ${installer}" >&2
    return 1
  fi
  if ! (cd "${sdk}/tools/gcc" && bash "./get_gcc_10_3-2021_10_linux.sh"); then
    echo "" >&2
    echo "Could not download Korg-bundled gcc (network or ARM developer site)." >&2
    echo "Options:" >&2
    echo "  1) Run locally: cd \$LOGUE_SDK/tools/gcc && ./get_gcc_10_3-2021_10_linux.sh" >&2
    echo "  2) Use Docker: ./tools/mkii-automate.sh docker <slug>" >&2
    echo "  3) Scaffold only: MKII_SKIP_GCC=1 ./tools/mkii-automate.sh bootstrap" >&2
    return 1
  fi
  if [[ ! -x "${gcc_bin}" ]]; then
    echo "gcc install failed; expected ${gcc_bin}" >&2
    return 1
  fi
}

mkii_scaffold_all() {
  local root sdk force_flag=()
  root="$(mkii_repo_root)"
  if ! sdk="$(mkii_resolve_logue_sdk 2>/dev/null)"; then
    sdk="$(mkii_ensure_logue_sdk_clone)"
  fi
  mkii_init_submodules "${sdk}"
  if [[ "${1:-}" == "--force" ]]; then
    force_flag=(--force)
  fi
  export LOGUE_SDK="${sdk}"
  python3 "${root}/tools/mkii/scaffold-mkii.py" --all-bass "${force_flag[@]}"
}

mkii_docker_link() {
  local root slug sdk unit_dir link_name target
  root="$(mkii_repo_root)"
  slug="$1"
  sdk="$(mkii_resolve_logue_sdk)" || {
    echo "LOGUE_SDK not ready" >&2
    return 1
  }
  unit_dir="${root}/src/mkii/oscillators/${slug}"
  if [[ ! -d "${unit_dir}" ]]; then
    echo "Unit not found: ${unit_dir}" >&2
    return 1
  fi
  link_name="inst-${slug}"
  target="${sdk}/platform/nts-1_mkii/${link_name}"
  ln -sfn "${unit_dir}" "${target}"
  echo "Docker: start ${sdk}/docker/run_interactive.sh"
  echo "Docker: build nts-1_mkii/${link_name}"
}
