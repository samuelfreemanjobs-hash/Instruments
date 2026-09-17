#!/usr/bin/env bash
# One-time VM setup: PostgreSQL 16 + DocumentDB extension + FerretDB v2.7 (GitHub downloads only).
# Prisma ORM still needs real MongoDB for writes; this stack helps with driver-level tests and db push experiments.
set -euo pipefail

FERRETDB_VERSION="${FERRETDB_VERSION:-2.7.0}"
DOCDB_TAG="${DOCDB_TAG:-v0.107.0-ferretdb-2.7.0}"
FERRETDB_BIN="${FERRETDB_BIN:-/tmp/ferretdb}"

if [[ $EUID -ne 0 ]]; then
  echo "Run with sudo: sudo $0"
  exit 1
fi

export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq curl postgresql-16 postgresql-16-cron postgresql-16-pgvector \
  postgresql-16-postgis-3 postgresql-16-postgis-3-scripts postgresql-16-rum

if ! dpkg -s postgresql-16-documentdb >/dev/null 2>&1; then
  tmpdeb="/tmp/documentdb.deb"
  curl -fsSL -o "$tmpdeb" \
    "https://github.com/FerretDB/documentdb/releases/download/${DOCDB_TAG}/ubuntu24.04-postgresql-16-documentdb_0.107.0.ferretdb.2.7.0_amd64.deb"
  dpkg -i "$tmpdeb"
fi

# FerretDB internal libpq pool needs trust on localhost (dev only).
if ! grep -q 'host all all 127.0.0.1/32 trust' /etc/postgresql/16/main/pg_hba.conf; then
  sed -i '1i host all all 127.0.0.1/32 trust' /etc/postgresql/16/main/pg_hba.conf
  sed -i '1i host all all ::1/128 trust' /etc/postgresql/16/main/pg_hba.conf
fi

if ! grep -q "pg_documentdb" /var/lib/postgresql/16/main/postgresql.auto.conf 2>/dev/null; then
  sudo -u postgres psql -d postgres -c "ALTER SYSTEM SET shared_preload_libraries = 'pg_cron,pg_documentdb_core,pg_documentdb';"
fi

service postgresql restart
sudo -u postgres psql -d postgres -c "CREATE EXTENSION IF NOT EXISTS documentdb CASCADE;"
sudo -u postgres psql -c "DO \$\$ BEGIN CREATE USER ferret WITH PASSWORD 'ferret' SUPERUSER; EXCEPTION WHEN duplicate_object THEN NULL; END \$\$;"
sudo -u postgres psql -c "ALTER USER ferret WITH PASSWORD 'ferret';"

if [[ ! -x "$FERRETDB_BIN" ]]; then
  curl -fsSL -o "$FERRETDB_BIN" \
    "https://github.com/FerretDB/FerretDB/releases/download/v${FERRETDB_VERSION}/ferretdb-linux-amd64"
  chmod +x "$FERRETDB_BIN"
fi

echo "DocumentDB + FerretDB ready. Start with:"
echo "  $FERRETDB_BIN --postgresql-url=postgres://ferret:ferret@127.0.0.1:5432/postgres"
