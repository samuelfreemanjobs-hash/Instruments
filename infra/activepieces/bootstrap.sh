#!/usr/bin/env bash
# Generate .env secrets and start Activepieces (Docker Compose v2 required).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

if ! command -v docker >/dev/null 2>&1; then
  echo "Docker is not installed. Install Docker Desktop or engine, then re-run:"
  echo "  $ROOT/bootstrap.sh"
  exit 1
fi

if ! docker compose version >/dev/null 2>&1; then
  echo "docker compose (v2 plugin) is required."
  exit 1
fi

if [[ ! -f .env ]]; then
  cp .env.example .env
  if [[ "$(uname)" == Darwin ]]; then
    sed -i '' -e "s|^AP_API_KEY=.*|AP_API_KEY=$(openssl rand -hex 32)|" .env
    sed -i '' -e "s|^AP_ENCRYPTION_KEY=.*|AP_ENCRYPTION_KEY=$(openssl rand -hex 16)|" .env
    sed -i '' -e "s|^AP_JWT_SECRET=.*|AP_JWT_SECRET=$(openssl rand -hex 32)|" .env
    sed -i '' -e "s|^AP_POSTGRES_PASSWORD=.*|AP_POSTGRES_PASSWORD=$(openssl rand -hex 32)|" .env
  else
    sed -i -e "s|^AP_API_KEY=.*|AP_API_KEY=$(openssl rand -hex 32)|" .env
    sed -i -e "s|^AP_ENCRYPTION_KEY=.*|AP_ENCRYPTION_KEY=$(openssl rand -hex 16)|" .env
    sed -i -e "s|^AP_JWT_SECRET=.*|AP_JWT_SECRET=$(openssl rand -hex 32)|" .env
    sed -i -e "s|^AP_POSTGRES_PASSWORD=.*|AP_POSTGRES_PASSWORD=$(openssl rand -hex 32)|" .env
  fi
  echo "Created .env with generated secrets (not committed — see .gitignore)."
else
  echo "Using existing .env (skip generation)."
fi

docker compose pull
docker compose up -d

REPO="$(cd "$ROOT/../.." && pwd)"
if grep -qE '^DISKLORDZ_AP_ADMIN_EMAIL=.+$' .env 2>/dev/null && grep -qE '^DISKLORDZ_AP_ADMIN_PASSWORD=.+$' .env; then
  set -a
  # shellcheck disable=SC1091
  source .env
  set +a
  python3 "$REPO/scripts/provision_disklordz_activepieces.py" || true
fi

echo ""
echo "Activepieces UI: http://localhost:8080"
echo "Full automation: set DISKLORDZ_AP_ADMIN_* in .env, then run ./automate.sh"
echo "Webhook file:    infra/activepieces/.disklordz-webhook-url (used by render_kit.py)"
echo "Logs:            docker compose logs -f app"
