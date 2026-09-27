#!/usr/bin/env bash
# First-time local dev: deps, optional .env.local stub, stub WAVs.
set -euo pipefail
# shellcheck disable=SC1091
source "$(dirname "$0")/lib/common.sh"
cd_website

log "Installing npm dependencies"
npm ci

if [ ! -f .env.local ] && [ -f .env.example ]; then
  if [ ! -f .env.local ]; then
    cp .env.example .env.local
    warn "Created .env.local from .env.example — add Supabase keys for auth/saved kits."
  fi
fi

if [ -f ../sound-factory/scripts/generate_stub_kits.py ]; then
  log "Regenerating stub WAVs (optional)"
  python3 ../sound-factory/scripts/generate_stub_kits.py || warn "Stub kit script skipped"
fi

log "Dev setup done. Run: npm run dev"
log "Local smoke: npm run smoke:local"
