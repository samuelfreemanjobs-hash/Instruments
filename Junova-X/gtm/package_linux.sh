#!/usr/bin/env bash
# Pack Release Junova-X binaries into dist/Junova-X-<version>-linux-x64.zip
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GTM="$(cd "$(dirname "$0")" && pwd)"
ART="${ROOT}/build/Junova-X/JunovaX_artefacts/Release"
VERSION="${JUNOVA_VERSION:-0.9.0-rc1}"
STAGING="$(mktemp -d /tmp/junova-pack-XXXXXX)"
DIST="${GTM}/dist"
ZIP_NAME="Junova-X-${VERSION}-linux-x64.zip"

for path in \
  "${ART}/VST3/Junova-X.vst3" \
  "${ART}/CLAP/Junova-X.clap" \
  "${ART}/Standalone/Junova-X"; do
  if [[ ! -e "$path" ]]; then
    echo "Missing artefact: $path — build with:" >&2
    echo "  cmake --build build -j --target JunovaX_VST3 JunovaX_CLAP JunovaX_Standalone" >&2
    exit 1
  fi
done

mkdir -p "${STAGING}/Junova-X" "${STAGING}/Junova-X/bin"
cp -a "${ART}/VST3/Junova-X.vst3" "${STAGING}/Junova-X/"
cp -a "${ART}/CLAP/Junova-X.clap" "${STAGING}/Junova-X/"
cp -a "${ART}/Standalone/Junova-X" "${STAGING}/Junova-X/bin/"
cp "${GTM}/INSTALL.txt" "${STAGING}/Junova-X/"
echo "${VERSION}" > "${STAGING}/Junova-X/VERSION"

mkdir -p "$DIST"
rm -f "${DIST}/${ZIP_NAME}"
(
  cd "$STAGING"
  zip -rq "${DIST}/${ZIP_NAME}" Junova-X
)

echo "Wrote ${DIST}/${ZIP_NAME}"
rm -rf "$STAGING"
