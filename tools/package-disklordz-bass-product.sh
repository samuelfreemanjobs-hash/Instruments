#!/usr/bin/env bash
# Build + package DiskLordz sellable custom bass oscillator product (14 oscs × 4 platforms).
#
# Usage:
#   export LOGUE_SDK="$(pwd)/.deps/logue-sdk"   # optional if already built
#   ./tools/package-disklordz-bass-product.sh
#   ./tools/package-disklordz-bass-product.sh --skip-build   # copy only
#
# Output:
#   release/products/disklordz-custom-bass-oscillators-<version>/
#   release/products/disklordz-custom-bass-oscillators-<version>.zip

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CATALOG="${ROOT}/disklordz/products/custom-bass-oscillators/catalog.json"
SKIP_BUILD=0

for arg in "$@"; do
  case "$arg" in
    --skip-build) SKIP_BUILD=1 ;;
    -h|--help)
      sed -n '3,16p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown option: $arg" >&2
      exit 1
      ;;
  esac
done

if [[ ! -f "${CATALOG}" ]]; then
  echo "Missing catalog: ${CATALOG}" >&2
  exit 1
fi

VERSION="$(python3 -c "import json; print(json.load(open('${CATALOG}'))['version'])")"
BUNDLE_NAME="disklordz-custom-bass-oscillators-${VERSION}"
OUT="${ROOT}/release/products/${BUNDLE_NAME}"
ZIP="${ROOT}/release/products/${BUNDLE_NAME}.zip"

find_unit_file() {
  local dir="$1" base="$2"
  for path in "${dir}/${base}" "${dir}/build/${base}"; do
  if [[ -f "${path}" ]]; then
      echo "${path}"
      return 0
    fi
  done
  return 1
}

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

rm -rf "${OUT}"
mkdir -p "${OUT}/units/nts-1" "${OUT}/units/minilogue-xd" "${OUT}/units/prologue" "${OUT}/units/nts-1-mkii"

python3 - "${CATALOG}" "${ROOT}" "${OUT}" <<'PY'
import json, shutil, sys
from pathlib import Path

catalog_path, root, out = Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3])
catalog = json.loads(catalog_path.read_text())
missing = []

def find(src_dir: Path, name: str) -> Path | None:
    for p in (src_dir / name, src_dir / "build" / name):
        if p.is_file():
            return p
    return None

for u in catalog["units"]:
    v1 = root / "src" / u["v1_dir"]
    proj = u["project"]
    for ext, sub in (
        ("ntkdigunit", "nts-1"),
        ("mnlgxdunit", "minilogue-xd"),
        ("prlgunit", "prologue"),
    ):
        src = find(v1, f"{proj}.{ext}")
        if src is None:
            missing.append(f"{proj}.{ext}")
        else:
            shutil.copy2(src, out / "units" / sub / src.name)
    mkii = root / "src/mkii/oscillators" / u["mkii_slug"]
    mkii_name = f"{u['mkii_slug']}.nts1mkiiunit"
    src = find(mkii, mkii_name)
    if src is None:
        missing.append(mkii_name)
    else:
        shutil.copy2(src, out / "units/nts-1-mkii" / src.name)

if missing:
    print("Missing unit files (run build first):", file=sys.stderr)
    for m in missing:
        print(f"  - {m}", file=sys.stderr)
    raise SystemExit(1)

inv = {
    "product_id": catalog["product_id"],
    "version": catalog["version"],
    "generated_utc": __import__("datetime").datetime.now(__import__("datetime").timezone.utc).isoformat(),
    "files": [],
}
for f in sorted(out.rglob("*")):
    if f.is_file() and "units/" in str(f.relative_to(out)):
        inv["files"].append(str(f.relative_to(out)))
(out / "MANIFEST.json").write_text(json.dumps(inv, indent=2) + "\n", encoding="utf-8")
print(f"Copied {len(inv['files'])} unit files")
PY

python3 "${ROOT}/tools/products/generate_bass_osc_product_docs.py" "${OUT}"

mkdir -p "${ROOT}/release/products"
rm -f "${ZIP}"
(
  cd "${ROOT}/release/products"
  zip -rq "${BUNDLE_NAME}.zip" "${BUNDLE_NAME}"
)

echo ""
echo "Bundle directory: ${OUT}"
echo "Bundle zip:       ${ZIP}"
echo "Units:            $(find "${OUT}/units" -type f | wc -l) files"
du -sh "${OUT}" "${ZIP}"
