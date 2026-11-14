#!/usr/bin/env bash
# Build every logue SDK v1 oscillator/effect in this repo for one or more platforms,
# then optionally all NTS-1 mkII oscillators.
#
# Usage:
#   export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
#   ./tools/build-all-units.sh                    # v1: nutekt-digital only
#   ./tools/build-all-units.sh --all-platforms    # nutekt-digital, minilogue-xd, prologue
#   ./tools/build-all-units.sh --mkii             # also build mkII catalog
#   ./tools/build-all-units.sh --all-platforms --mkii --package
#
# Writes a build summary to release/logue-build-status.tsv (and stdout).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=tools/mkii/lib.sh
source "${ROOT}/tools/mkii/lib.sh"

ALL_PLATFORMS=0
DO_MKII=0
DO_PACKAGE=0
PLATFORMS=(nutekt-digital)

for arg in "$@"; do
  case "${arg}" in
    --all-platforms) ALL_PLATFORMS=1 ;;
    --mkii) DO_MKII=1 ;;
    --package) DO_PACKAGE=1 ;;
    -h|--help)
      sed -n '3,14p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown option: ${arg}" >&2
      exit 1
      ;;
  esac
done

if [[ "${ALL_PLATFORMS}" -eq 1 ]]; then
  PLATFORMS=(nutekt-digital minilogue-xd prologue)
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  if sdk="$(mkii_resolve_logue_sdk 2>/dev/null)"; then
    export LOGUE_SDK="${sdk}"
  else
    echo "LOGUE_SDK is not set. Run: ./tools/mkii-automate.sh bootstrap" >&2
    exit 1
  fi
fi

V1_UNITS=(
  oscillators/tr808-kick
  oscillators/west-coast-moog
  oscillators/juno-rnb
  oscillators/dilla-bass
  oscillators/sub-phatty
  oscillators/prophet-funk
  oscillators/dx7-lately
  oscillators/sh101-babyface
  oscillators/moog-voyager-se
  oscillators/cardo-gfunk
  oscillators/larry-june
  oscillators/khalifa-haze
  oscillators/memphis-juicy
  oscillators/ensoniq-eps1
  oscillators/template
  oscillators/wavetable
  effects/modfx
  effects/delfx
  effects/revfx
)

STATUS_DIR="${ROOT}/release"
mkdir -p "${STATUS_DIR}"
STATUS_FILE="${STATUS_DIR}/logue-build-status.tsv"
echo -e "unit\tplatform\tresult\tnote" >"${STATUS_FILE}"

record_status() {
  local unit="$1" platform="$2" result="$3" note="${4:-}"
  echo -e "${unit}\t${platform}\t${result}\t${note}" >>"${STATUS_FILE}"
  printf '[%s] %s @ %s: %s %s\n' "${result}" "${unit}" "${platform}" "${note}"
}

failures=0
for platform in "${PLATFORMS[@]}"; do
  for unit in "${V1_UNITS[@]}"; do
    echo "==> v1 ${unit} (${platform})"
    set +e
    log="$("${ROOT}/tools/build.sh" "${unit}" "${platform}" 2>&1)"
    rc=$?
    set -e
    if [[ "${rc}" -eq 0 ]]; then
      record_status "${unit}" "${platform}" "OK" ""
    else
      failures=$((failures + 1))
      note="$(echo "${log}" | tail -3 | tr '\n' ' ')"
      record_status "${unit}" "${platform}" "FAIL" "${note}"
    fi
  done
done

if [[ "${DO_MKII}" -eq 1 ]]; then
  gcc_bin="$(mkii_gcc_mkii_bin "${LOGUE_SDK}")"
  if [[ ! -x "${gcc_bin}" ]] && ! command -v arm-none-eabi-gcc >/dev/null; then
    echo "No arm-none-eabi-gcc for mkII; skipping mkII builds." >&2
    record_status "mkii/*" "nts-1_mkii" "SKIP" "no toolchain"
  else
  for meta in "${ROOT}"/src/mkii/oscillators/*/mkii.meta.json; do
    slug="$(basename "$(dirname "${meta}")")"
    [[ "${slug}" == template_osc || "${slug}" == wavetable_osc ]] && continue
    echo "==> mkII ${slug}"
    set +e
    log="$("${ROOT}/tools/build-mkii.sh" "${slug}" 2>&1)"
    rc=$?
    set -e
    if [[ "${rc}" -eq 0 ]]; then
      record_status "mkii/${slug}" "nts-1_mkii" "OK" ""
    else
      failures=$((failures + 1))
      note="$(echo "${log}" | tail -3 | tr '\n' ' ')"
      record_status "mkii/${slug}" "nts-1_mkii" "FAIL" "${note}"
    fi
  done
  fi
fi

if [[ "${DO_PACKAGE}" -eq 1 ]]; then
  "${ROOT}/tools/package-nts1.sh"
fi

echo ""
echo "Summary: ${STATUS_FILE} (${failures} failure(s))"
exit $(( failures > 0 ? 1 : 0 ))
