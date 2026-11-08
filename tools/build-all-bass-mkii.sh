#!/usr/bin/env bash
# Scaffold (if missing) and build all mkII bass oscillators.
#
#   export LOGUE_SDK=/path/to/logue-sdk
#   ./tools/build-all-bass-mkii.sh [--scaffold-only]

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=tools/mkii/lib.sh
source "${ROOT}/tools/mkii/lib.sh"

SCAFFOLD_ONLY=0
BOOTSTRAP=0
NO_SCAFFOLD=0
BASS_ONLY=0
for arg in "$@"; do
  case "${arg}" in
    --scaffold-only) SCAFFOLD_ONLY=1 ;;
    --bootstrap) BOOTSTRAP=1 ;;
    --no-scaffold) NO_SCAFFOLD=1 ;;
    --bass-only) BASS_ONLY=1 ;;
  esac
done

if [[ "${BOOTSTRAP}" -eq 1 ]]; then
  sdk="$(mkii_ensure_logue_sdk_clone)"
  mkii_init_submodules "${sdk}"
  mkii_ensure_arm_gcc "${sdk}"
  export LOGUE_SDK="${sdk}"
elif [[ -z "${LOGUE_SDK:-}" ]]; then
  if sdk="$(mkii_resolve_logue_sdk 2>/dev/null)"; then
    export LOGUE_SDK="${sdk}"
  fi
fi

if [[ "${NO_SCAFFOLD}" -eq 0 ]]; then
  mkii_scaffold_all --force
fi

if [[ "${SCAFFOLD_ONLY}" -eq 1 ]]; then
  echo "Scaffold complete under src/mkii/oscillators/"
  exit 0
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  echo "LOGUE_SDK not set — run: ./tools/mkii-automate.sh bootstrap" >&2
  exit 1
fi

BASS_SLUGS=(
  tr808_kick_phonk
  west_coast_moog_funk
  juno_106_rnb_bass
  dilla_type_bass
  moog_sub_phatty_bass
  prophet5_funk_rnb_bass
  dx7_lately_bass
  sh101_babyface_bass
  moog_voyager_se1_squelch
  cardo_got_wings_gfunk
  larry_june_bay_cruise_bass
  khalifa_haze_stoner_bass
  juicy_j_three6_memphis_bass
  ensoniq_eps1_memphis_bass
)

build_slugs=()
if [[ "${BASS_ONLY}" -eq 1 ]]; then
  build_slugs=("${BASS_SLUGS[@]}")
else
  for meta in "${ROOT}"/src/mkii/oscillators/*/mkii.meta.json; do
    build_slugs+=("$(basename "$(dirname "${meta}")")")
  done
fi

for slug in "${build_slugs[@]}"; do
  echo "==> Building mkII ${slug}"
  "${ROOT}/tools/build-mkii.sh" "${slug}"
done

echo "All mkII bass units built."
