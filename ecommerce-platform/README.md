# eCommerce Platform (2025 Stack)

Step-by-step system to build a production-style shop with **Next.js 15**, **Prisma + MongoDB**, **Tailwind**, **Stripe/RazorPay**, and **Cloudinary**, optimized for **Cursor AI** workflows.

## Start here

1. Read [docs/BLUEPRINT.md](docs/BLUEPRINT.md) — ordered phases with checkboxes.
2. Follow [docs/SOP/README.md](docs/SOP/README.md) — one SOP per phase.
3. In Cursor, enable the **ecommerce-builder** skill (`.cursor/skills/ecommerce-builder/SKILL.md`).
4. Scaffold code from [`templates/`](templates/) using [templates/README.md](templates/README.md).

## Quick start (app)

```bash
cd web
cp .env.example .env.local
# Set DATABASE_URL (MongoDB), auth secrets, optional Stripe/RazorPay/Cloudinary
npm install
npx prisma generate
npx prisma db push   # when DATABASE_URL is ready
npm run dev
```

- Storefront: http://localhost:3000  
- Admin: http://localhost:3000/admin (requires `ADMIN` role user)

## Repository layout

```
ecommerce-platform/
├── ARCHITECTURE.md
├── AGENTS.md
├── docs/           Blueprint, walkthrough, SOPs
├── templates/      Reusable code & doc templates
├── .cursor/skills/ecommerce-builder/
└── web/            Runnable Next.js application (starter + stubs)
```

## Course alignment

The blueprint maps to the course module list (admin UI → Prisma → completion → storefront → auth → checkout → email). Track progress in [docs/BLUEPRINT.md](docs/BLUEPRINT.md).
