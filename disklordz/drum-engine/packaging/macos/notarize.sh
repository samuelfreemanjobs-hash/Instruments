#!/usr/bin/env bash
# Notarize a pkg on macOS. On Linux this exits 2 so a release agent stops instead of pretending.
set -euo pipefail

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "notarize: this host is $(uname -s). Run this script on macOS with Xcode notarytool." >&2
  echo "Required on that Mac: Developer ID Application, Developer ID Installer, and an app-specific password in the keychain." >&2
  exit 2
fi

PKG="${1:-}"
if [[ -z "${PKG}" || ! -f "${PKG}" ]]; then
  echo "usage: notarize.sh path/to/Product.pkg" >&2
  exit 2
fi

: "${APPLE_ID:?set APPLE_ID}"
: "${APPLE_TEAM_ID:?set APPLE_TEAM_ID}"
: "${APPLE_APP_PASSWORD:?set APPLE_APP_PASSWORD}"

xcrun notarytool submit "${PKG}" \
  --apple-id "${APPLE_ID}" \
  --team-id "${APPLE_TEAM_ID}" \
  --password "${APPLE_APP_PASSWORD}" \
  --wait
xcrun stapler staple "${PKG}"
spctl --assess --type install "${PKG}"
echo "stapled ${PKG}"
