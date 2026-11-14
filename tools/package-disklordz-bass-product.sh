#!/usr/bin/env bash
# Build + package DiskLordz custom bass oscillators:
#   - Full bundle (14 oscs × 4 platforms)
#   - 14 single-SKU zips (one osc each × 4 platforms)
#
# Usage:
#   export LOGUE_SDK="$(pwd)/.deps/logue-sdk"   # optional if already built
#   ./tools/package-disklordz-bass-product.sh
#   ./tools/package-disklordz-bass-product.sh --skip-build
#
# Output (under release/products/):
#   diskLordz-custom-bass-oscillators-<version>.zip
#   singles/disklordz-<slug>-<version>.zip  (×14)
#   SKU_TABLE-<version>.md
#   PRODUCT_INDEX-<version>.json

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CATALOG="${ROOT}/disklordz/products/custom-bass-oscillators/catalog.json"
SKIP_BUILD=0

for arg in "$@"; do
  case "${arg}" in
    --skip-build) SKIP_BUILD=1 ;;
    -h|--help)
      sed -n '3,20p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown option: ${arg}" >&2
      exit 1
      ;;
  esac
done

if [[ ! -f "${CATALOG}" ]]; then
  echo "Missing catalog: ${CATALOG}" >&2
  exit 1
fi

if [[ "${SKIP_BUILD}" -eq 0 ]]; then
  if [[ -z "${LOGUE_SDK:-}" ]]; then
    if [[ -d "${ROOT}/.deps/logue-sdk/platform/nutekt-digital" ]]; then
      export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
    else
      export MKII_SKIP_GCC=1
      "${ROOT}/tools/mkii-automate.sh" bootstrap
      export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
    fi
  fi
  if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    echo "Install arm-none-eabi-gcc or pass --skip-build if binaries already exist." >&2
    exit 1
  fi
  export GCC_BIN_PATH="$(dirname "$(command -v arm-none-eabi-gcc)")/"
  for p in nutekt-digital minilogue-xd prologue; do
    "${ROOT}/tools/build-all-bass.sh" "${p}"
  done
  "${ROOT}/tools/build-all-bass-mkii.sh"
fi

python3 "${ROOT}/tools/products/package_bass_catalog.py"

VERSION="$(python3 -c "import json; print(json.load(open('${CATALOG}'))['version'])")"
PRODUCTS="${ROOT}/release/products"
echo ""
echo "=== Packaging complete (v${VERSION}) ==="
echo "Bundle zip:  ${PRODUCTS}/disklordz-custom-bass-oscillators-${VERSION}.zip"
echo "Singles dir: ${PRODUCTS}/singles/"
echo "SKU table:   ${PRODUCTS}/SKU_TABLE-${VERSION}.md"
du -sh "${PRODUCTS}/disklordz-custom-bass-oscillators-${VERSION}.zip" \
  "${PRODUCTS}/singles/"*".zip" 2>/dev/null | head -5
echo "..."
ls "${PRODUCTS}/singles/"*.zip 2>/dev/null | wc -l | xargs -I{} echo "Single zips: {}"
