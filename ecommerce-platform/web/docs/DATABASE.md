# Database setup

Prisma on MongoDB **requires a replica-set deployment** (all writes use transactions). A plain single-node `mongod` must be started with `--replSet`; Atlas includes a replica set by default.

**FerretDB + DocumentDB** (Postgres-backed) can run `prisma db push` but **does not** support Prisma CRUD (`commitTransaction` is missing). Do not use FerretDB as the app database for this project.

## Option A — MongoDB Atlas (recommended for production and Cloud Agents)

1. Create a free cluster at https://www.mongodb.com/cloud/atlas
2. Add the connection string to environment secrets (Cloud Agent dashboard) **or** `web/.env.local`:

```env
DATABASE_URL="mongodb+srv://USER:PASS@cluster.mongodb.net/ecommerce?retryWrites=true&w=majority"
SESSION_SECRET="openssl rand -base64 32"
```

3. Apply schema and seed:

```bash
cd web
npm run db:push
npm run db:seed
npm run dev
```

Seed creates `admin@example.com` / `admin123`, categories, products, coupon `WELCOME10`.

Cloud Agent VMs often block `fastdl.mongodb.org` / `downloads.mongodb.com`; use Atlas and allow egress to `*.mongodb.net` in the environment network policy.

## Option B — In-memory Mongo (local dev / CI when CDN is reachable)

```bash
npm run dev:db
```

Uses `mongodb-memory-server` (downloads MongoDB from the official CDN), writes `.env.local`, runs `db push` + seed, then Next.js.

If download fails with TLS/`ECONNRESET`, use Atlas (Option A) or fix egress (see `web/.cursor/environment.json`).

## Option C — Automation scripts

| Script | Purpose |
|--------|---------|
| `npm run db:local` | `bootstrap-local-db.ts` — probe Mongo, push, seed |
| `npm run e2e:quick` | Headless checkout + fulfill + email dev log |
| `./scripts/ensure-local-db.sh` | Same as `db:local` |
| `sudo ./scripts/setup-documentdb-ferretdb.sh` | Optional Postgres DocumentDB + FerretDB (advanced; not for Prisma writes) |
| `./scripts/start-ferretdb.sh` | Start FerretDB if you use Option C for experiments |

`scripts/lib/ensure-mongodb.ts` picks, in order: `DATABASE_URL` (with write probe) → mongodb-memory-server → failure with actionable message.
