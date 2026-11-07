#!/usr/bin/env bash
# Collect built NTS-1 mkI (.ntkdigunit) and mkII (.nts1mkiiunit) into release/ for install.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/release/nts1-custom-oscillators"
MKI="${OUT}/mkI-ntkdigunit"
MKII="${OUT}/mkII-nts1mkiiunit"
mkdir -p "${MKI}" "${MKII}"

shopt -s nullglob
count_mki=0
for pkg in "${ROOT}"/src/oscillators/*/*.ntkdigunit; do
  cp -a "${pkg}" "${MKI}/"
  count_mki=$((count_mki + 1))
done

count_mkii=0
for pkg in "${ROOT}"/src/mkii/oscillators/*/*.nts1mkiiunit \
           "${ROOT}"/src/mkii/oscillators/*/build/*.nts1mkiiunit; do
  [[ -f "${pkg}" ]] || continue
  cp -a "${pkg}" "${MKII}/"
  count_mkii=$((count_mkii + 1))
done

cat > "${OUT}/INSTALL.txt" <<'EOF'
Korg NTS-1 custom oscillators — install (same day)

## Which file type?

- NTS-1 original (mkI): *.ntkdigunit in mkI-ntkdigunit/
- NTS-1 mkII (firmware 1.0+): *.nts1mkiiunit in mkII-nts1mkiiunit/

## mkI (nutekt-digital)

1. Install KORG KONTROL Editor (or legacy logue SDK Librarian).
2. Connect NTS-1 via USB.
3. Open the Oscillator slot → Load → pick a .ntkdigunit from mkI-ntkdigunit/.
4. Write to the unit. Only one custom osc is active at a time — swap per track.

Preset knob values: see each unit's src/oscillators/<name>/manifest.json → _agent.presets.

## mkII

1. KORG KONTROL Editor or loguecli (see logue-sdk platform/nts-1_mkii/README.md).
2. Load .nts1mkiiunit from mkII-nts1mkiiunit/ into the oscillator slot.

## Rebuild locally

  ./tools/mkii-automate.sh bootstrap   # once: clones logue-sdk
  ./tools/build-all-bass.sh            # mkI catalog
  ./tools/build-all-bass-mkii.sh       # mkII (after DSP ports complete)
  ./tools/package-nts1.sh              # refresh this folder
EOF

echo "Packaged ${count_mki} mkI and ${count_mkii} mkII units → ${OUT}"
