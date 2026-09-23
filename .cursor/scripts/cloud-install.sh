#!/usr/bin/env bash
# Cloud Agent VM bootstrap — idempotent. Invoked from .cursor/environment.json install.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "${REPO_ROOT}"

echo "==> apt packages (JUCE + Python multisample lane)"
sudo apt-get update -qq
sudo apt-get install -y -qq \
  build-essential g++-12 gcc-12 cmake git python3-pip \
  libstdc++-12-dev libasound2-dev libfreetype6-dev libfontconfig1-dev \
  libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev \
  libxext-dev libcurl4-openssl-dev libxcomposite-dev freeglut3-dev pkg-config

echo "==> git submodules (gearmulator fork + mpc-agent)"
git submodule sync --recursive
git submodule update --init --recursive gearmulator-lane/gearmulator mpc-agent 2>/dev/null || \
  git submodule update --init --recursive gearmulator-lane/gearmulator

echo "==> CMake: JD Upgraded + Wave909 + Vst3OfflineRender"
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j"$(nproc)" --target Vst3OfflineRender Wave909_VST3 2>/dev/null || \
  cmake --build build -j"$(nproc)" --target Vst3OfflineRender

if [[ -f gearmulator-lane/requirements-dawdreamer.txt ]]; then
  echo "==> DawDreamer (Python VST host for multisample capture)"
  pip install -q -r gearmulator-lane/requirements-dawdreamer.txt
fi

echo "==> Cloud install complete"
