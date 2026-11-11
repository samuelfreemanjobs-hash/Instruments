# Legacy Vite PWA — archived

**Status: archived** (Phase 8). Production and CI use **Next.js only**.

The Vite app under `src/` remains in the repo for historical reference. Do not ship new features there.

| Need | Command |
|------|---------|
| **Development** | `npm run dev` → Next :3000 + API :3847 |
| **Legacy regression only** | `npm run dev:legacy` → Vite :5173 (unmaintained) |

Legacy build output is **not** part of GitHub Actions `deal-hunter.yml`.
