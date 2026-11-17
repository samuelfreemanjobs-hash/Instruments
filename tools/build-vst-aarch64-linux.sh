#!/usr/bin/env bash
# Cross-build a JUCE VST3 under vst/<Name> for Linux aarch64 (Akai MPC Sample / Melodee).
#
# Usage:
#   ./tools/build-vst-aarch64-linux.sh <PluginName>
#   ./tools/build-vst-aarch64-linux.sh --list
#
# Host deps (Debian/Ubuntu):
#   sudo apt-get install -y build-essential cmake \
#     gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
#     libasound2-dev:arm64 libfreetype6-dev:arm64 \
#     libx11-dev:arm64 libxrandr-dev:arm64 libxinerama-dev:arm64 \
#     libxcursor-dev:arm64 libgtk-3-dev:arm64
#
# The resulting .vst3 places the .so under Contents/aarch64-linux/ (not x86_64-linux).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOOLCHAIN="${ROOT}/cmake/toolchains/linux-aarch64-gnu.cmake"
JUCE_DIR="${ROOT}/vst/JUCE"
BUILD_DIR_NAME="${MPC_VST_BUILD_DIR:-build-aarch64}"

is_template_dir() {
  case "$(basename "$1")" in
    template|template-effect|JUCE|factory-lib) return 0 ;;
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
    echo "  # or: git clone --branch 7.0.12 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE" >&2
    exit 1
  fi
}

ensure_cross_compiler() {
  local cxx="${CXX:-aarch64-linux-gnu-g++}"
  if ! command -v "${cxx}" >/dev/null 2>&1; then
    echo "Cross compiler not found: ${cxx}" >&2
    echo "Install: sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu" >&2
    exit 1
  fi
}

ensure_host_cxx_for_juceaide() {
  # JUCE bootstraps juceaide with the host `c++` (not the cross toolchain). Fail early with a clear message.
  local probe="/tmp/juce_host_cxx_probe"
  if ! /usr/bin/c++ -x c++ - -o "${probe}" 2>/dev/null <<< 'int main() { return 0; }'; then
    if command -v g++-13 >/dev/null 2>&1; then
      echo "Host /usr/bin/c++ cannot link C++ (often clang++ without libstdc++)." >&2
      echo "Fix: sudo update-alternatives --set c++ /usr/bin/g++" >&2
      echo "     or export HOST_CXX=g++-13 and ensure 'c++' invokes a working GNU g++." >&2
      exit 1
    fi
    echo "Host C++ toolchain cannot link a test program; install build-essential / g++." >&2
    exit 1
  fi
  rm -f "${probe}"
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

  echo "==> Cross-building vst/${name} (aarch64-linux)"
  ensure_host_cxx_for_juceaide
  cd "${vst}"

  cmake_args=(
    -B "${BUILD_DIR_NAME}"
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}"
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_CXX_FLAGS="-march=armv8-a"
    -DCMAKE_C_FLAGS="-march=armv8-a"
    -DCMAKE_FIND_ROOT_PATH="/usr/aarch64-linux-gnu"
    -DCOPY_PLUGIN_AFTER_BUILD=FALSE
  )

  # Lighter link on embedded targets (optional override per plugin in CMakeLists).
  if [[ "${MPC_HEADLESS:-}" == "1" ]]; then
    cmake_args+=(-DMPC_VST3_HEADLESS=ON)
  fi

  cmake "${cmake_args[@]}"
  local vst3_target="${name}_VST3"
  if ! cmake --build "${BUILD_DIR_NAME}" --config Release -j"$(nproc 2>/dev/null || echo 4)" --target "${vst3_target}"; then
    echo "Hint: install arm64 GUI/audio dev libs (multiarch) — see docs/MPC_SAMPLE_VST3.md" >&2
    return 1
  fi

  local artefact
  artefact="$(find "${BUILD_DIR_NAME}" -name '*.vst3' -type d 2>/dev/null | head -1 || true)"
  if [[ -n "${artefact}" ]]; then
    echo "VST3: ${vst}/${artefact}"
    if [[ -x "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" ]]; then
      bash "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" "${vst}/${artefact}" || true
    fi
  else
    echo "Build finished; locate bundle: find ${vst}/${BUILD_DIR_NAME} -name '*.vst3'" >&2
    return 1
  fi
}

cmd="${1:-}"
case "${cmd}" in
  --list|-l)
    list_plugins
    exit 0
    ;;
  -h|--help|help|"")
    cat <<EOF
Usage: $0 <PluginName> | --list

Environment:
  MPC_VST_BUILD_DIR   CMake build dir (default: build-aarch64)
  MPC_HEADLESS=1      Pass -DMPC_VST3_HEADLESS=ON when plugin supports it
  CXX                 Cross g++ (default: aarch64-linux-gnu-g++)
EOF
    exit 0
    ;;
  *)
    ensure_juce
    ensure_cross_compiler
    build_one "${cmd}"
    ;;
esac