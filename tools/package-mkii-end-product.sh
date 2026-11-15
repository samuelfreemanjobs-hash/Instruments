#!/usr/bin/env bash
# End product: versioned zip of all mkII genre oscillators + INSTALL for Windows/Kontrol Editor.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "${ROOT}"

"${ROOT}/tools/package-nts1.sh"

OUT="${ROOT}/release/nts1-custom-oscillators"
MKII="${OUT}/mkII-nts1mkiiunit"
ZIP="${ROOT}/release/nts1-mkii-genre-oscillators.zip"
VERSION="$(date -u +%Y%m%d)"

cat > "${OUT}/README-mkII.txt" <<EOF
NTS-1 mkII — DiskLordz genre oscillator pack
Built: ${VERSION} UTC
Firmware: NTS-1 mkII >= 1.0.0

Load: KORG KONTROL Editor -> NTS-1 mkII -> Oscillator -> Load -> Write
Folder: mkII-nts1mkiiunit/*.nts1mkiiunit

Musical units (14): tr808_kick_phonk, west_coast_moog_funk, juno_106_rnb_bass,
dilla_type_bass, moog_sub_phatty_bass, prophet5_funk_rnb_bass, dx7_lately_bass,
sh101_babyface_bass, moog_voyager_se1_squelch, cardo_got_wings_gfunk,
larry_june_bay_cruise_bass, khalifa_haze_stoner_bass, juicy_j_three6_memphis_bass,
ensoniq_eps1_memphis_bass

Quick ref: docs/NTS1_MKII_WINDOWS_QUICKREF.md (in repo)
EOF

count="$(find "${MKII}" -maxdepth 1 -name '*.nts1mkiiunit' | wc -l)"
if [[ "${count}" -lt 14 ]]; then
  echo "Need at least 14 mkII units; found ${count}. Run: bash ./scripts/windows/build-mkii-wsl.sh" >&2
  exit 1
fi

rm -f "${ZIP}"
(
  cd "${OUT}"
  zip -qr "${ZIP}" INSTALL.txt README-mkII.txt mkII-nts1mkiiunit
)

echo "End product: ${ZIP} (${count} units)"
echo "Windows: unzip -> prepare-mkii-test.ps1 -> Kontrol Editor"