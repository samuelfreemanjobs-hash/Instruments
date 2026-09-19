#!/usr/bin/env bash
# VST Plugin Factory OS — create plugins, list registry, build all targets.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REGISTRY="${ROOT}/plugins/registry.txt"
TEMPLATES="${ROOT}/templates"

usage() {
  cat <<'EOF'
VST Plugin Factory OS

Usage:
  factory.sh build [--target NAME]     Configure + build (default: all VST3 targets)
  factory.sh verify                    Ensure every registry plugin has a Release VST3
  factory.sh validate                pluginval on all factory VST3 bundles
  factory.sh ship [--install-dir DIR] Copy VST3 bundles to disk (default: ~/.vst3)
  factory.sh release [--new KIND NAME] Build + verify + validate + ship (zero-touch product)
  factory.sh list                      Show registered plugin directories
  factory.sh new effect <dir-name>     Scaffold a stereo effect from template
  factory.sh new synth <dir-name>      Scaffold a VSTi from template

Monorepo full QA + factory + ship:
  python3 vst-testing-ops/run_business.py --profile release

Examples:
  ./scripts/factory.sh release
  ./scripts/factory.sh release --new effect my-drive
  FACTORY_VST3_INSTALL_DIR=$HOME/.vst3 ./scripts/factory.sh ship
EOF
}

slug_ok() {
  [[ "$1" =~ ^[a-z][a-z0-9-]*$ ]]
}

require_cmake() {
  if ! command -v cmake >/dev/null; then
    echo "cmake not found on PATH." >&2
    exit 1
  fi
}

cmd_list() {
  echo "Registered plugins (plugins/registry.txt):"
  grep -v '^#' "$REGISTRY" | grep -v '^[[:space:]]*$' || true
}

cmd_build() {
  require_cmake
  local target=""
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --target) target="$2"; shift 2 ;;
      *) echo "Unknown build arg: $1" >&2; exit 1 ;;
    esac
  done

  cd "$ROOT"
  local generator=()
  if command -v ninja >/dev/null; then
    generator=(-G Ninja)
  fi

  local cxx=()
  if command -v g++-12 >/dev/null; then
    cxx=(-DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12)
  elif command -v g++ >/dev/null; then
    cxx=(-DCMAKE_CXX_COMPILER=g++)
  fi

  cmake -B build -DCMAKE_BUILD_TYPE=Release "${generator[@]}" "${cxx[@]}"
  if [[ -n "$target" ]]; then
    cmake --build build -j "$(nproc 2>/dev/null || echo 4)" --target "$target"
  else
    cmake --build build -j "$(nproc 2>/dev/null || echo 4)"
  fi

  echo ""
  echo "Artifacts: ${ROOT}/build/*_artefacts/Release/VST3/"
}

replace_in_file() {
  local file="$1"
  local from="$2"
  local to="$3"
  sed -i "s/${from}/${to}/g" "$file"
}

to_camel_case() {
  # my-drive -> MyDrive
  local input="$1"
  local out=""
  local part
  IFS='-' read -ra parts <<< "$input"
  for part in "${parts[@]}"; do
    out+="$(tr '[:lower:]' '[:upper:]' <<< "${part:0:1}")${part:1}"
  done
  echo "$out"
}

random_plugin_code() {
  # Four chars with at least one uppercase (JUCE requirement).
  local base
  base="$(tr -dc 'A-Za-z0-9' </dev/urandom | head -c 3)"
  echo "${base}X"
}

cmd_verify() {
  exec python3 "${ROOT}/scripts/factory_ops.py" verify-artefacts
}

cmd_validate() {
  exec python3 "${ROOT}/scripts/run_pluginval_factory.py"
}

cmd_ship() {
  local install_dir=""
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --install-dir) install_dir="$2"; shift 2 ;;
      *) echo "Unknown ship arg: $1" >&2; exit 1 ;;
    esac
  done
  if [[ -n "$install_dir" ]]; then
    exec python3 "${ROOT}/scripts/factory_ops.py" ship --install-dir "$install_dir"
  fi
  exec python3 "${ROOT}/scripts/factory_ops.py" ship
}

cmd_release() {
  local new_kind="" new_name=""
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --new)
        new_kind="${2:-}"; new_name="${3:-}"
        shift 3
        ;;
      *) echo "Unknown release arg: $1" >&2; exit 1 ;;
    esac
  done
  if [[ -n "$new_kind" || -n "$new_name" ]]; then
    if [[ -z "$new_kind" || -z "$new_name" ]]; then
      echo "--new requires KIND and DIR-NAME (effect|synth)" >&2
      exit 1
    fi
    cmd_new "$new_kind" "$new_name"
  fi
  cmd_build
  python3 "${ROOT}/scripts/factory_ops.py" verify-artefacts
  python3 "${ROOT}/scripts/run_pluginval_factory.py"
  python3 "${ROOT}/scripts/factory_ops.py" ship
}

cmd_new() {
  local kind="${1:-}"
  local dir_name="${2:-}"
  if [[ -z "$kind" || -z "$dir_name" ]]; then
    usage
    exit 1
  fi
  if ! slug_ok "$dir_name"; then
    echo "Directory name must be lowercase alphanumerics and hyphens (e.g. my-drive)." >&2
    exit 1
  fi

  local dest="${ROOT}/plugins/${dir_name}"
  if [[ -e "$dest" ]]; then
    echo "Already exists: $dest" >&2
    exit 1
  fi

  local camel target product plugin_code
  camel="$(to_camel_case "$dir_name")"
  target="${camel}"
  product="$camel"
  plugin_code="$(random_plugin_code)"

  case "$kind" in
    effect)
      mkdir -p "$dest"
      cp -R "${TEMPLATES}/effect/." "$dest/"
      ;;
    synth)
      mkdir -p "$dest"
      cp -R "${TEMPLATES}/synth/." "$dest/"
      ;;
    *)
      echo "Kind must be 'effect' or 'synth'." >&2
      exit 1
      ;;
  esac

  while IFS= read -r -d '' f; do
    replace_in_file "$f" "{{PLUGIN_CAMEL}}" "$camel"
    replace_in_file "$f" "{{PLUGIN_TARGET}}" "$target"
    replace_in_file "$f" "{{PLUGIN_PRODUCT_NAME}}" "$product"
    replace_in_file "$f" "{{PLUGIN_CODE}}" "$plugin_code"
    replace_in_file "$f" "{{PLUGIN_DIR_SLUG}}" "$dir_name"
  done < <(find "$dest" -type f -print0)

  echo "$dir_name" >> "$REGISTRY"
  echo "Created plugin at plugins/${dir_name}"
  echo "  CMake target: ${target}"
  echo "  PLUGIN_CODE:  ${plugin_code}"
  echo "Run: ./scripts/factory.sh build --target ${target}_VST3"
}

main() {
  local cmd="${1:-}"
  shift || true
  case "$cmd" in
    build) cmd_build "$@" ;;
    verify) cmd_verify ;;
    validate) cmd_validate ;;
    ship) cmd_ship "$@" ;;
    release) cmd_release "$@" ;;
    list) cmd_list ;;
    new) cmd_new "$@" ;;
    -h|--help|help|"") usage ;;
    *) echo "Unknown command: $cmd" >&2; usage; exit 1 ;;
  esac
}

main "$@"
