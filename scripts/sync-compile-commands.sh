#!/usr/bin/env bash
# Symlink build/compile_commands.json to repo root for clangd (run after cmake configure).
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
build="${root}/build/compile_commands.json"
if [[ ! -f "$build" ]]; then
  echo "Missing $build — run: cmake -B build ..." >&2
  exit 1
fi
ln -sf "$build" "${root}/compile_commands.json"
echo "Linked ${root}/compile_commands.json -> build/compile_commands.json"
