# Deal Hunter — architecture

## Purpose

Progressive Web App for couples or households hunting seasonal deals with **low cognitive load** UI. Supports Black Friday, Cyber Monday, holiday gifting, then grocery/household couponing. Users filter by price, shipping, condition (new/used/refurb), and working coupons; only **trusted retailer URLs** surface. Shared list uses checkboxes + share codes; browser notifications fire on steals and price drops.

## Build & run

```bash
cd tools/deal-hunter
npm ci
npm run dev          # Vite on :5173, API on :3847 (proxied /api)
npm run build && npm start   # production: API serves dist/
```

## Data flow

1. **Search**: Browser calls `GET /api/deals/search` with query filters. Server filters the in-repo catalog, applies simulated live-price jitter, appends rows to SQLite `price_snapshots`, and returns steal score, discount %, and short price history.
2. **Lists**: Items live in `localStorage` on each device. **Create share code** posts to `POST /api/lists`; partner **Join** loads `GET /api/lists/:code`. **Sync** pushes `PUT` so both stay aligned.
3. **Alerts**: Thresholds and watchlist in `localStorage`. On each search refresh (~5 min), the client evaluates steal score and price-drop rules and fires `Notification` when permitted.

## Threading / realtime

Single-threaded Node API; no audio/realtime constraints. Price refresh is polling-based (client interval).

## Key modules

| Path | Role |
|------|------|
| `server/catalog.ts` | Sample deal inventory (replace with retailer APIs) |
| `server/index.ts` | REST API, search, shared lists |
| `server/db.ts` | SQLite for lists + price history |
| `src/App.tsx` | Main UI: search, list, alerts |
| `src/hooks/useAlerts.ts` | Notification logic |
| `vite.config.ts` | PWA manifest + service worker |

## Extension points

- Swap `CATALOG` for Amazon PA-API, affilate feeds, or a Firecrawl scraper pipeline (respect robots/ToS).
- Add Web Push with a small backend for alerts when the app is closed.
- Pin-protect list updates for multi-user write access.

## Related docs

- Root [ARCHITECTURE.md](../../ARCHITECTURE.md) monorepo index
- [README.md](./README.md) user-facing overview
