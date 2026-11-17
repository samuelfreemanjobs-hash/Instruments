#!/usr/bin/env bash
# Collect built logue SDK user units into release/ for install / handoff.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/release/nts1-custom-oscillators"
MKI_NTS="${OUT}/mkI-ntkdigunit"
MKI_XD="${OUT}/minilogue-xd-mnlgxdunit"
MKI_PRO="${OUT}/prologue-prlgunit"
MKII="${OUT}/mkII-nts1mkiiunit"
mkdir -p "${MKI_NTS}" "${MKI_XD}" "${MKI_PRO}" "${MKII}"

shopt -s nullglob
copy_glob() {
  local dest="$1"
  shift
  local n=0
  for pkg in "$@"; do
    [[ -f "${pkg}" ]] || continue
    cp -a "${pkg}" "${dest}/"
    n=$((n + 1))
  done
  echo "${n}"
}

count_mki_nts="$(copy_glob "${MKI_NTS}" \
  "${ROOT}"/src/oscillators/*/*.ntkdigunit \
  "${ROOT}"/src/oscillators/*/build/*.ntkdigunit \
  "${ROOT}"/src/effects/*/*.ntkdigunit \
  "${ROOT}"/src/effects/*/build/*.ntkdigunit)"

count_mki_xd="$(copy_glob "${MKI_XD}" \
  "${ROOT}"/src/oscillators/*/*.mnlgxdunit \
  "${ROOT}"/src/oscillators/*/build/*.mnlgxdunit \
  "${ROOT}"/src/effects/*/*.mnlgxdunit \
  "${ROOT}"/src/effects/*/build/*.mnlgxdunit)"

count_mki_pro="$(copy_glob "${MKI_PRO}" \
  "${ROOT}"/src/oscillators/*/*.prlgunit \
  "${ROOT}"/src/oscillators/*/build/*.prlgunit \
  "${ROOT}"/src/effects/*/*.prlgunit \
  "${ROOT}"/src/effects/*/build/*.prlgunit)"

count_mkii="$(copy_glob "${MKII}" \
  "${ROOT}"/src/mkii/oscillators/*/*.nts1mkiiunit \
  "${ROOT}"/src/mkii/oscillators/*/build/*.nts1mkiiunit)"

cat > "${OUT}/INSTALL.txt" <<'EOF'
Korg logue custom oscillators — install

## Which file type?

| Hardware | Extension | Folder |
|----------|-----------|--------|
| NTS-1 (mkI) | *.ntkdigunit | mkI-ntkdigunit/ |
| minilogue xd | *.mnlgxdunit | minilogue-xd-mnlgxdunit/ |
| prologue | *.prlgunit | prologue-prlgunit/ |
| NTS-1 mkII | *.nts1mkiiunit | mkII-nts1mkiiunit/ |

## Load onto hardware

1. Install KORG KONTROL Editor or logue-cli (see logue-sdk docs).
2. Connect the synth via USB.
3. Open the Oscillator (or Effect) slot → Load → pick the unit file.
4. Only one custom osc is active at a time on NTS-1 — swap per track.

Preset knob values: `src/oscillators/<name>/manifest.json` → `_agent.presets`.

## Rebuild locally

  ./tools/mkii-automate.sh bootstrap
  export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
  ./tools/build-all-units.sh --all-platforms --mkii --package

Windows: see docs/BUILD_WINDOWS_LOGUE.md

**Not hardware-tested in CI** — verify on your unit before a live set.
EOF

echo "Packaged mkI NTS=${count_mki_nts} XD=${count_mki_xd} Pro=${count_mki_pro} mkII=${count_mkii} → ${OUT}"
