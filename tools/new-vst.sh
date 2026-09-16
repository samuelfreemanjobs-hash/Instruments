#!/usr/bin/env bash
# Create a new VST/VSTi project from vst/template or vst/template-effect.
#
# Usage:
#   ./tools/new-vst.sh MyPluginName [--type synth|effect]
#
# Plugin name: PascalCase, letters and digits only (e.g. Phonk808, VinylNoise).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME=""
TYPE="synth"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --type)
      TYPE="${2:-}"
      shift 2
      ;;
    -*)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
    *)
      if [[ -z "${NAME}" ]]; then
        NAME="$1"
      else
        echo "Unexpected argument: $1" >&2
        exit 1
      fi
      shift
      ;;
  esac
done

if [[ -z "${NAME}" ]]; then
  echo "Usage: $0 <PluginName> [--type synth|effect]" >&2
  echo "Example: $0 Phonk808" >&2
  echo "Example: $0 VinylNoise --type effect" >&2
  exit 1
fi

if [[ ! "${NAME}" =~ ^[A-Za-z][A-Za-z0-9]*$ ]]; then
  echo "Plugin name must be PascalCase alphanumeric (e.g. MySynth)." >&2
  exit 1
fi

case "${TYPE}" in
  synth) SRC_TEMPLATE="template" ;;
  effect) SRC_TEMPLATE="template-effect" ;;
  *)
    echo "Unknown --type: ${TYPE} (use synth or effect)" >&2
    exit 1
    ;;
esac

DEST="${ROOT}/vst/${NAME}"
if [[ -e "${DEST}" ]]; then
  echo "Already exists: ${DEST}" >&2
  exit 1
fi

CODE="$(echo "${NAME}" | tr '[:lower:]' '[:upper:]' | grep -o '[A-Z0-9]' | head -4 | tr -d '\n')"
while [[ ${#CODE} -lt 4 ]]; do
  CODE="${CODE}X"
done
CODE="${CODE:0:4}"

cp -a "${ROOT}/vst/${SRC_TEMPLATE}" "${DEST}"

if [[ "${TYPE}" == "effect" ]]; then
  BASE_CLASS="TemplateEffect"
  sed -i "s/TemplateEffect/${NAME}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/Template Effect/${NAME}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/Tefx/${CODE}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/templateeffect/$(echo "${NAME}" | tr '[:upper:]' '[:lower:]')/g" "${DEST}/CMakeLists.txt"
  for f in "${DEST}/Source/"*.h "${DEST}/Source/"*.cpp; do
    sed -i "s/TemplateEffect/${NAME}/g" "${f}"
  done
else
  sed -i "s/TemplateSynth/${NAME}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/Template Synth/${NAME}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/Tmpl/${CODE}/g" "${DEST}/CMakeLists.txt"
  sed -i "s/templatesynth/$(echo "${NAME}" | tr '[:upper:]' '[:lower:]')/g" "${DEST}/CMakeLists.txt"
  for f in "${DEST}/Source/"*.h "${DEST}/Source/"*.cpp; do
    sed -i "s/TemplateSynth/${NAME}/g" "${f}"
  done
fi

if [[ -f "${DEST}/params.json" ]]; then
  sed -i "s/TemplateEffect/${NAME}/g" "${DEST}/params.json" 2>/dev/null || true
  sed -i "s/TemplateSynth/${NAME}/g" "${DEST}/params.json" 2>/dev/null || true
fi

KIND="${TYPE}"
cat > "${DEST}/README.md" <<EOF
# ${NAME}

Created from \`vst/${SRC_TEMPLATE}\` via \`./tools/new-vst.sh ${NAME} --type ${TYPE}\`.

## Build

\`\`\`bash
./tools/build-vst.sh ${NAME}
\`\`\`

Kind: **${KIND}** — see \`params.json\` and \`Source/PluginProcessor.*\`.
EOF

echo "Created ${DEST} (${TYPE})"
echo "Build: ./tools/build-vst.sh ${NAME}"
