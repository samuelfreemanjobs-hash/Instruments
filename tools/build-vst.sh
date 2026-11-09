#!/usr/bin/env bash
# Build VST/VSTi projects under vst/ (excludes template folders).
#
# Usage:
#   ./tools/build-vst.sh <PluginName>
#   ./tools/build-vst.sh --list
#   ./tools/build-vst.sh --all
#
# JUCE: clones the tag in /JUCE_VERSION into vst/JUCE when missing (not a git submodule).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
JUCE_DIR="${ROOT}/vst/JUCE"
JUCE_VERSION_FILE="${ROOT}/JUCE_VERSION"

is_template_dir() {
  case "$(basename "$1")" in
    template|template-effect) return 0 ;;
    *) return 1 ;;
  esac
}

list_plugins() {
  for d in "${ROOT}"/vst/*/; do
    [[ -d "$d" ]] || continue
    is_template_dir "$d" && continue
    [[ -f "${d}CMakeLists.txt" ]] || continue
    basename "$d"
  done
}

read_juce_pin() {
  if [[ ! -f "${JUCE_VERSION_FILE}" ]]; then
    echo "Missing ${JUCE_VERSION_FILE} — cannot resolve JUCE pin." >&2
    exit 1
  fi
  tr -d '[:space:]' < "${JUCE_VERSION_FILE}"
}

local_juce_tag() {
  # Exact tag at HEAD, else empty (detached/wrong/non-git trees).
  git -C "${JUCE_DIR}" describe --tags --exact-match HEAD 2>/dev/null || true
}

ensure_juce() {
  local tag have
  tag="$(read_juce_pin)"
  if [[ -z "${tag}" ]]; then
    echo "JUCE_VERSION is empty." >&2
    exit 1
  fi
  if [[ -f "${JUCE_DIR}/CMakeLists.txt" ]]; then
    have="$(local_juce_tag)"
    if [[ "${have}" == "${tag}" ]]; then
      return 0
    fi
    echo "vst/JUCE is at '${have:-unknown}', want pin ${tag} — replacing checkout" >&2
    rm -rf "${JUCE_DIR}"
  fi
  echo "Cloning JUCE pin ${tag} into vst/JUCE" >&2
  git clone --depth 1 --branch "${tag}" \
    https://github.com/juce-framework/JUCE.git "${JUCE_DIR}"
}

prefer_compilers() {
  # Match AGENTS.md / CI: prefer g++-12 when present (default c++ may lack libstdc++).
  if [[ -z "${CXX:-}" ]] && command -v g++-12 >/dev/null 2>&1; then
    export CXX=g++-12
  fi
  if [[ -z "${CC:-}" ]] && command -v gcc-12 >/dev/null 2>&1; then
    export CC=gcc-12
  fi
}

build_one() {
  local name="$1"
  local vst="${ROOT}/vst/${name}"
  if [[ ! -d "${vst}" ]]; then
    echo "Not found: ${vst}" >&2
    return 1
  fi
  if is_template_dir "${vst}"; then
    echo "Skip template: ${name}" >&2
    return 1
  fi
  prefer_compilers
  echo "==> Building vst/${name} (CC=${CC:-cc} CXX=${CXX:-c++})"
  cd "${vst}"
  cmake -B build -DCMAKE_BUILD_TYPE=Release \
    ${CC:+-DCMAKE_C_COMPILER="${CC}"} \
    ${CXX:+-DCMAKE_CXX_COMPILER="${CXX}"}
  cmake --build build --config Release -j"$(nproc 2>/dev/null || echo 4)"
  local artefact
  artefact="$(find build -name '*.vst3' 2>/dev/null | head -1 || true)"
  if [[ -n "${artefact}" ]]; then
    echo "VST3: ${vst}/${artefact}"
  else
    echo "Build finished (search: find ${vst}/build -name '*.vst3')"
  fi
}

cmd="${1:-}"
case "${cmd}" in
  --list|-l)
    list_plugins
    exit 0
    ;;
  --all|-a)
    ensure_juce
    failed=0
    while IFS= read -r name; do
      build_one "${name}" || failed=1
    done < <(list_plugins)
    exit "${failed}"
    ;;
  -h|--help|help|"")
    cat <<EOF
Usage: $0 <PluginName> | --list | --all

JUCE is resolved from /JUCE_VERSION (cloned into vst/JUCE when missing).
EOF
    exit 0
    ;;
  *)
    ensure_juce
    build_one "${cmd}"
    ;;
esac
