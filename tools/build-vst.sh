#!/usr/bin/env bash
# Build VST/VSTi projects under vst/ (excludes template folders).
#
# Usage:
#   ./tools/build-vst.sh <PluginName>
#   ./tools/build-vst.sh --list
#   ./tools/build-vst.sh --all
#
# Requires: git submodule update --init vst/JUCE

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
JUCE_DIR="${ROOT}/vst/JUCE"

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

ensure_juce() {
  if [[ ! -f "${JUCE_DIR}/CMakeLists.txt" ]]; then
    echo "JUCE missing. Run from repo root:" >&2
    echo "  git submodule update --init vst/JUCE" >&2
    exit 1
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
  echo "==> Building vst/${name}"
  cd "${vst}"
  cmake -B build -DCMAKE_BUILD_TYPE=Release
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
EOF
    exit 0
    ;;
  *)
    ensure_juce
    build_one "${cmd}"
    ;;
esac
