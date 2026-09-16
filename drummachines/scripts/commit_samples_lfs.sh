#!/usr/bin/env bash
# Stage extracted samples for Git LFS after fetch_and_extract.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/.."
git lfs install
git add drummachines/samples .gitattributes
echo "Staged samples for LFS. Review with: git lfs ls-files | head"
echo "Then commit and push (expect multi-GB upload; ensure GitHub LFS quota)."
