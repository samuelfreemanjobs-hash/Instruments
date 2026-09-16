#!/usr/bin/env bash
# Create a new VST/VSTi project from vst/template.
#
# Usage: ./tools/new-vst.sh MyPluginName
#
# Plugin name: PascalCase, letters and digits only (e.g. Phonk808, JunoBass).

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${1:-}"

if [[ -z "${NAME}" ]]; then
  echo "Usage: $0 <PluginName>" >&2
  echo "Example: $0 Phonk808" >&2
  exit 1
fi

if [[ ! "${NAME}" =~ ^[A-Za-z][A-Za-z0-9]*$ ]]; then
  echo "Plugin name must be PascalCase alphanumeric (e.g. MySynth)." >&2
  exit 1
fi

DEST="${ROOT}/vst/${NAME}"
if [[ -e "${DEST}" ]]; then
  echo "Already exists: ${DEST}" >&2
  exit 1
fi

# 4-char plugin code from name (JUCE requirement)
CODE="$(echo "${NAME}" | tr '[:lower:]' '[:upper:]' | grep -o '[A-Z0-9]' | head -4 | tr -d '\n')"
while [[ ${#CODE} -lt 4 ]]; do
  CODE="${CODE}X"
done
CODE="${CODE:0:4}"

cp -a "${ROOT}/vst/template" "${DEST}"
if [[ -f "${DEST}/params.json" ]]; then
  sed -i "s/TemplateSynth/${NAME}/g" "${DEST}/params.json"
fi

# CMake / JUCE identifiers
sed -i "s/TemplateSynth/${NAME}/g" "${DEST}/CMakeLists.txt"
sed -i "s/Template Synth/${NAME}/g" "${DEST}/CMakeLists.txt"
sed -i "s/Tmpl/${CODE}/g" "${DEST}/CMakeLists.txt"
sed -i "s/templatesynth/$(echo "${NAME}" | tr '[:upper:]' '[:lower:]')/g" "${DEST}/CMakeLists.txt"

for f in "${DEST}/Source/"*.h "${DEST}/Source/"*.cpp; do
  sed -i "s/TemplateSynth/${NAME}/g" "${f}"
done

cat > "${DEST}/README.md" <<EOF
# ${NAME}

Created from \`vst/template\` via \`./tools/new-vst.sh ${NAME}\`.

## Build

\`\`\`bash
git submodule update --init vst/JUCE   # from repo root, once
cd vst/${NAME}
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
\`\`\`

## Parameters

- **Gain** — output level
- **Wave** — sine vs saw mix

Edit \`Source/PluginProcessor.*\` for DSP.
EOF

echo "Created ${DEST}"
echo "Next: edit PLUGIN_MANUFACTURER_CODE in CMakeLists.txt if needed, then build."
