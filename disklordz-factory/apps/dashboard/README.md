# Factory dashboard

React + Vite operator UI for the Autonomous Music Factory.

## Run (with API)

From repo root:

```bash
chmod +x disklordz-factory/scripts/run-factory-dev.sh
./disklordz-factory/scripts/run-factory-dev.sh
```

Open **http://127.0.0.1:5173** — Vite proxies `/api` → Factory API on port **8787**.

## Features

- Active projects / today counters (`GET /factory/dashboard`)
- Catalog stats and queue progress bars
- **Run night shift** → `POST /night-shift/run` then refresh approval queue
- **Approve / reject** pending assets (human gate at 07:00)

Manual dev:

```bash
cd disklordz-factory/apps/dashboard && npm install && npm run dev
```
