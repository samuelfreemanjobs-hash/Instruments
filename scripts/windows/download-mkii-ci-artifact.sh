#!/usr/bin/env bash
# Same as download-mkii-ci-artifact.ps1 — for WSL when PowerShell/gh fails.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REPO="${GH_REPO:-samuelfreemanjobs-hash/Instruments}"
ARTIFACT="nts1-mkii-units"
OUT="${ROOT}/release/nts1-custom-oscillators"
WF="nts1-mkii-build.yml"

command -v gh >/dev/null || { echo "Install gh and run: gh auth login"; exit 1; }
gh auth status >/dev/null

run_id="${1:-}"
if [[ -z "${run_id}" ]]; then
  run_id="$(gh run list -R "${REPO}" --workflow "${WF}" --limit 30 --json databaseId,conclusion,headBranch \
    | python3 -c "import json,sys; r=json.load(sys.stdin); \
m=[x for x in r if x.get('conclusion')=='success' and x.get('headBranch')=='main']; \
print((m or [x for x in r if x.get('conclusion')=='success'])[0]['databaseId'])")"
fi

staging="$(mktemp -d)"
trap 'rm -rf "${staging}"' EXIT
for attempt in 1 2 3 4; do
  if gh run download "${run_id}" -R "${REPO}" --name "${ARTIFACT}" --dir "${staging}"; then
    break
  fi
  sleep $((attempt * 2))
done

mkdir -p "${OUT}"
cp -a "${staging}/"* "${OUT}/"
echo "Installed under ${OUT}"
echo "Windows: .\\scripts\\windows\\prepare-mkii-test.ps1"