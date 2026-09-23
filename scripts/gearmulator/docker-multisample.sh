#!/usr/bin/env bash
# Build and run the multisample Docker image (local machine / CI with Docker).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="${MULTISAMPLE_IMAGE:-instruments-multisample:latest}"
DOCKERFILE="${REPO_ROOT}/gearmulator-lane/docker/Dockerfile"

if ! command -v docker >/dev/null 2>&1; then
  echo "docker not found. Install Docker Desktop / engine on your host, or use the VM scripts directly:" >&2
  echo "  cmake --build build -j --target Vst3OfflineRender" >&2
  echo "  ./scripts/gearmulator/render_multisample_grid.sh ..." >&2
  exit 1
fi

cmd="${1:-build}"
shift || true

case "${cmd}" in
  build)
    docker build -f "${DOCKERFILE}" -t "${IMAGE}" "${REPO_ROOT}"
    ;;
  run)
    PLUGINS_DIR="${PLUGINS_DIR:-${REPO_ROOT}/gearmulator-lane/docker/volumes/plugins}"
    ROM_DIR="${ROM_DIR:-${REPO_ROOT}/gearmulator-lane/docker/volumes/rom}"
    OUT_DIR="${OUT_DIR:-${REPO_ROOT}/gearmulator-lane/multisamples/out}"
    mkdir -p "${PLUGINS_DIR}" "${ROM_DIR}" "${OUT_DIR}"
    docker run --rm \
      -e GEARMULATOR_ROM_DIR=/rom \
      -v "${PLUGINS_DIR}:/plugins:ro" \
      -v "${ROM_DIR}:/rom:ro" \
      -v "${OUT_DIR}:/out" \
      "${IMAGE}" "$@"
    ;;
  compose)
    export PLUGINS_DIR="${PLUGINS_DIR:-${REPO_ROOT}/gearmulator-lane/docker/volumes/plugins}"
    export ROM_DIR="${ROM_DIR:-${REPO_ROOT}/gearmulator-lane/docker/volumes/rom}"
    export OUT_DIR="${OUT_DIR:-${REPO_ROOT}/gearmulator-lane/multisamples/out}"
    docker compose -f "${REPO_ROOT}/gearmulator-lane/docker/docker-compose.yml" run --rm multisample "$@"
    ;;
  *)
    echo "Usage: $0 build | run [--plugin /plugins/X.vst3 ...] | compose [grid args...]" >&2
    exit 1
    ;;
esac
