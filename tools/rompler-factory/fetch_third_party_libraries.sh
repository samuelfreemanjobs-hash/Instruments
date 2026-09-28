#!/usr/bin/env bash
# Fetch CC0 / CC-BY SFZ libraries for ASR-class v0 (local only; large downloads).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="${ROOT}/content/asr-class-v0/third_party"
mkdir -p "${DEST}"

echo "Target: ${DEST}"
echo "See docs/ASR_CLASS_CONTENT_V0.md and content/asr-class-v0/ATTRIBUTION.md"
echo ""

if [[ "${DOWNLOAD_THIRD_PARTY:-0}" != "1" ]]; then
  cat <<'EOF'
Dry run (default). Set DOWNLOAD_THIRD_PARTY=1 to attempt downloads.

Manual sources:
  VCSL (CC0):     https://github.com/sgossner/VCSL/releases — SFZ pack
  VSCO 2 CE (CC0): https://versilian-studios.com/vsco-community/ — Vanilla SFZ
  Salamander (CC-BY 3.0): https://github.com/sfzinstruments/SalamanderGrandPiano
                          + https://freepats.zenvoid.org/Piano/acoustic-grand-piano.html

After extract:
  third_party/vcsl/
  third_party/vsco2ce/
  third_party/salamander/

Curate subsets into HISE; do not commit third_party/ (gitignored).
EOF
  exit 0
fi

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || { echo "Missing: $1"; exit 1; }
}

need_cmd curl
need_cmd unzip

fetch_github_release_asset() {
  local repo="$1"
  local dest_dir="$2"
  local pattern="$3"
  mkdir -p "${dest_dir}"
  echo "==> Latest release assets for ${repo} matching ${pattern}"
  local api="https://api.github.com/repos/${repo}/releases/latest"
  local url
  url="$(curl -sL "${api}" | python3 -c "
import json,sys
r=json.load(sys.stdin)
pat=sys.argv[1]
for a in r.get('assets',[]):
    if pat in a['name'].lower():
        print(a['browser_download_url'])
        break
" "${pattern}")"
  if [[ -z "${url}" ]]; then
    echo "No asset matched; download manually from https://github.com/${repo}/releases"
    return 1
  fi
  local tmp
  tmp="$(mktemp -d)"
  curl -sL "${url}" -o "${tmp}/asset.zip"
  unzip -q "${tmp}/asset.zip" -d "${dest_dir}" || { echo "Unzip failed; leave zip in ${dest_dir}"; mv "${tmp}/asset.zip" "${dest_dir}/"; }
  rm -rf "${tmp}"
  echo "Extracted under ${dest_dir}"
}

fetch_github_release_asset "sgossner/VCSL" "${DEST}/vcsl" "sfz" || true

echo ""
echo "VSCO 2 CE: no stable single GitHub asset — download Vanilla SFZ from versilian-studios.com into ${DEST}/vsco2ce/"
echo "Salamander: clone or download SFZ+samples per sfzinstruments/SalamanderGrandPiano into ${DEST}/salamander/"
echo "Done."
