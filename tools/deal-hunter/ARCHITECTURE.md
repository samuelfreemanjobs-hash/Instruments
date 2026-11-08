# Savvy HQ — architecture

Year-round shopping intelligence PWA. **Black Friday** and **Cyber Monday** are peak calendar modes; the engine runs every day on authorized retailer feeds (not scrape-everything).

## Build & run

```bash
cd tools/deal-hunter
npm ci
npm run dev
npm test && npm run build
```

## Data flow

1. **Catalog** — demo curated in `server/catalog.ts`; production swaps affiliate/PA-API/partner files per `server/integrations/data-sources.ts`.
2. **Real savings** — `intelligence/engine.ts` compares live price to 30-day SQLite snapshots; UI shows advertised % vs real % on `RealSavingsStrip`.
3. **Autopilot** — `agents/autonomous-fleet.ts` runs Value Scout, Deal Stalker, Local Scout, Community Curator, Season Planner; n8n hits `/api/autopilot/run`.
4. **Discover** — community tips (trusted URLs), local partner circulars, recommendations.
5. **Notify** — browser notifications + n8n email/web-push relay (`integrations/notify.ts`).

## Hosting

- **Vercel**: static PWA (`vercel.json`, see `DEPLOY.md`).
- **API**: long-lived Node + SQLite (`better-sqlite3`).

## Key modules

| Path | Role |
|------|------|
| `server/season.ts` | Calendar: `everyday` default + BF/CM peaks |
| `server/intelligence/engine.ts` | Buy/wait, value verdicts, category trends |
| `server/agents/autonomous-fleet.ts` | Autonomous shopping agents |
| `server/integrations/n8n.ts` | Webhook automation |
| `src/pages/AutopilotPage.tsx` | Agent digest UI |
| `src/pages/DiscoverPage.tsx` | Community + local + recs |

## Related

- Root [ARCHITECTURE.md](../../ARCHITECTURE.md)
- [DEPLOY.md](./DEPLOY.md)
