#!/usr/bin/env bash
# Dispatch GitHub Actions workflow "logue NTS-1 units", optionally watch and download artifacts.
#
# Requires: gh (https://cli.github.com/) authenticated with repo + workflow scope.
# Optional: GH_WORKFLOW_TOKEN or GH_TOKEN (classic PAT with workflow scope for CI/automation).
#
# Usage:
#   ./scripts/trigger-logue-units-build.sh
#   ./scripts/trigger-logue-units-build.sh --ref main --watch --download
#   LOGUE_ARTIFACT_DIR=~/Downloads/logue-units ./scripts/trigger-logue-units-build.sh --download
#
# Local build fallback (no GitHub):
#   ./scripts/trigger-logue-units-build.sh --local

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REPO="${GITHUB_REPOSITORY:-samuelfreemanjobs-hash/Instruments}"
WORKFLOW_NAME="logue NTS-1 units"
WORKFLOW_FILE="logue-nts1-units.yml"
REF="${LOGUE_GHA_REF:-}"
WATCH=0
DOWNLOAD=0
LOCAL=0

usage() {
  sed -n '4,18p' "$0"
  exit "${1:-0}"
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --ref) REF="$2"; shift 2 ;;
    --watch) WATCH=1; shift ;;
    --download) DOWNLOAD=1; shift ;;
    --local) LOCAL=1; shift ;;
    -h|--help) usage 0 ;;
    *) echo "Unknown option: $1" >&2; usage 1 ;;
  esac
done

run_local_build() {
  echo "==> Local build (no GitHub Actions)"
  if [[ -z "${LOGUE_SDK:-}" ]]; then
    if [[ -d "${ROOT}/.deps/logue-sdk/platform/nutekt-digital" ]]; then
      export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
    else
      export MKII_SKIP_GCC=1
      "${ROOT}/tools/mkii-automate.sh" bootstrap
      export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
    fi
  fi
  "${ROOT}/tools/build-all-units.sh" --all-platforms --mkii --package
  echo "Output: ${ROOT}/release/nts1-custom-oscillators/"
}

if [[ "${LOCAL}" -eq 1 ]]; then
  run_local_build
  exit 0
fi

if [[ -z "${REF}" ]]; then
  if command -v gh >/dev/null 2>&1; then
    state="$(gh pr view 113 --repo "${REPO}" --json state -q .state 2>/dev/null || echo OPEN)"
    if [[ "${state}" == "MERGED" ]]; then
      REF="main"
    else
      REF="cursor/logue-osc-build-windows-13db"
    fi
  else
    REF="main"
  fi
fi

if [[ -n "${GH_WORKFLOW_TOKEN:-}" ]]; then
  export GH_TOKEN="${GH_WORKFLOW_TOKEN}"
elif [[ -n "${GITHUB_TOKEN:-}" && -z "${GH_TOKEN:-}" ]]; then
  export GH_TOKEN="${GITHUB_TOKEN}"
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "gh not found. Use --local for a full build on this machine." >&2
  exit 1
fi

if ! gh auth status -h github.com >/dev/null 2>&1; then
  echo "gh not authenticated. Run: gh auth login -s repo,workflow" >&2
  echo "Or: ./scripts/trigger-logue-units-build.sh --local" >&2
  exit 1
fi

echo "==> Dispatch '${WORKFLOW_NAME}' on ${REPO} @ ref=${REF}"

dispatch_ok=0
if gh workflow run "${WORKFLOW_NAME}" --repo "${REPO}" --ref "${REF}" 2>/dev/null; then
  dispatch_ok=1
else
  echo "workflow_dispatch failed; trying repository_dispatch (needs workflow on default branch)..." >&2
  if gh api "repos/${REPO}/dispatches" \
    -f event_type=build-logue-units \
    -f "client_payload[ref]=${REF}" 2>/dev/null; then
    dispatch_ok=1
  fi
fi

if [[ "${dispatch_ok}" -eq 0 ]]; then
  echo "Remote dispatch failed. Falling back to local build..." >&2
  run_local_build
  exit 0
fi

echo "Triggered. Open: https://github.com/${REPO}/actions/workflows/${WORKFLOW_FILE}"

if [[ "${WATCH}" -eq 1 ]] || [[ "${DOWNLOAD}" -eq 1 ]]; then
  sleep 12
  run_id="$(gh run list --repo "${REPO}" --workflow="${WORKFLOW_FILE}" --limit 5 --json databaseId,status,createdAt -q 'sort_by(.createdAt) | reverse | .[0].databaseId')"
fi

if [[ "${WATCH}" -eq 1 ]]; then
  if [[ -z "${run_id}" || "${run_id}" == "null" ]]; then
    echo "No workflow run found yet; skipping watch." >&2
  else
    gh run watch "${run_id}" --repo "${REPO}" --exit-status
  fi
fi

if [[ "${DOWNLOAD}" -eq 1 ]]; then
  if [[ -z "${run_id:-}" || "${run_id}" == "null" ]]; then
    run_id="$(gh run list --repo "${REPO}" --workflow="${WORKFLOW_FILE}" --limit 1 --json databaseId -q '.[0].databaseId')"
  fi
  if [[ -z "${run_id}" || "${run_id}" == "null" ]]; then
    echo "Could not resolve latest run id" >&2
    exit 1
  fi
  out="${LOGUE_ARTIFACT_DIR:-${ROOT}/release/from-gha}"
  mkdir -p "${out}"
  gh run download "${run_id}" --repo "${REPO}" -D "${out}"
  echo "Downloaded artifacts → ${out}"
fi
