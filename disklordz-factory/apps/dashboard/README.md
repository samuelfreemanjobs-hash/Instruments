# Factory dashboard (planned)

React/TypeScript UI matching the operator mock:

- Active projects / today counters
- Catalog and revenue summaries (from analytics DB)
- Factory queue progress by `BatchStage`
- **Approval queue** — approve/reject wired to `POST /approval-queue/{asset_id}/approve`

Until the app exists, use Factory API OpenAPI at `/docs`.
