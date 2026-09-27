#!/usr/bin/env bash
# Source production go-live env from .env.go-live (gitignored). Safe to source in other scripts.
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
load_go_live_env
