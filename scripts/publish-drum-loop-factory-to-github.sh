#!/usr/bin/env bash
# Publish the Drum Loop Factory tree to GitHub (Memphis remote or drum-loop-factory).
# Run on your PC from the Instruments repo root, or set DRUM_LOOP_FACTORY_SRC to your local project.
set -euo pipefail

REMOTE_URL="${DRUM_LOOP_FACTORY_REMOTE:-https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory.git}"
TOKEN="${GITHUB_PUSH_TOKEN:-${MEMPHIS_DRUM_LOOP_FACTORY_PUSH_TOKEN:-}}"

if [[ -n "${DRUM_LOOP_FACTORY_SRC:-}" ]]; then
  SRC="$DRUM_LOOP_FACTORY_SRC"
elif [[ -f contracts/loop_types.py ]]; then
  SRC="$(pwd)"
elif [[ -d drum-loop-factory ]]; then
  SRC="$(pwd)/drum-loop-factory"
else
  echo "Set DRUM_LOOP_FACTORY_SRC or run from Instruments root / factory project root." >&2
  exit 1
fi

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

git clone --depth 1 "$REMOTE_URL" "$WORKDIR/remote" 2>/dev/null || {
  mkdir -p "$WORKDIR/remote"
  git -C "$WORKDIR/remote" init -b main
}

cp -a "$SRC/." "$WORKDIR/remote/"
cd "$WORKDIR/remote"

if [[ ! -f .gitignore ]]; then
  echo "Warning: no .gitignore in source" >&2
fi

git add -A
if git diff --staged --quiet; then
  echo "Nothing to publish."
  exit 0
fi

git commit -m "chore: publish drum-loop-factory tree"

if [[ -n "$TOKEN" ]]; then
  host_path="${REMOTE_URL#https://}"
  git push "https://x-access-token:${TOKEN}@${host_path}" HEAD:main
else
  if ! git remote get-url origin &>/dev/null; then
    git remote add origin "$REMOTE_URL"
  fi
  git push -u origin HEAD:main
fi

echo "Published to $REMOTE_URL (main)"
