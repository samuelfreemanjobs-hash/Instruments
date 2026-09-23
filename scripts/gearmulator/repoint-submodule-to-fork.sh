#!/usr/bin/env bash
# Point gearmulator-lane/gearmulator submodule at your GitHub fork.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SUBMODULE_PATH="gearmulator-lane/gearmulator"
GITMODULES="${REPO_ROOT}/.gitmodules"

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <fork-git-url>" >&2
  echo "Example: $0 https://github.com/samuelfreemanjobs-hash/gearmulator.git" >&2
  exit 1
fi

FORK_URL="$1"
if [[ ! "${FORK_URL}" =~ ^https://github.com/[^/]+/gearmulator(\.git)?$ ]]; then
  echo "Expected a github.com fork URL ending in /gearmulator or /gearmulator.git" >&2
  exit 1
fi
FORK_URL="${FORK_URL%.git}.git"

if [[ ! -f "${GITMODULES}" ]]; then
  echo "Missing ${GITMODULES}" >&2
  exit 1
fi

git -C "${REPO_ROOT}" submodule sync -- "${SUBMODULE_PATH}" 2>/dev/null || true

# Update recorded URL in .gitmodules
git config -f "${GITMODULES}" "submodule.${SUBMODULE_PATH}.url" "${FORK_URL}"

git -C "${REPO_ROOT}" submodule sync "${SUBMODULE_PATH}"

if [[ -d "${REPO_ROOT}/${SUBMODULE_PATH}/.git" ]]; then
  git -C "${REPO_ROOT}/${SUBMODULE_PATH}" remote set-url origin "${FORK_URL}"
  echo "Submodule origin → ${FORK_URL}"
else
  echo "Submodule not checked out yet. After init, origin will be ${FORK_URL}"
fi

echo "Updated .gitmodules. Verify with: git config -f .gitmodules --get submodule.${SUBMODULE_PATH}.url"
