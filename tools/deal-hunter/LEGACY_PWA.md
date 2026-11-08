# Legacy Vite PWA (deprecated)

The **default product UI** is **Next.js** in `web/` (`npm run dev`).

`npm run dev:legacy` (Vite on :5173) remains for parity checks until the production cutover is signed off. New features land in Next only.

| Use | Command |
|-----|---------|
| **Primary** | `npm run dev` — Next :3000 + API :3847 |
| **Legacy** | `npm run dev:legacy` — Vite :5173 + API :3847 |

Production hosting: Vercel root **`tools/deal-hunter/web`** per [DEPLOY.md](./DEPLOY.md).
