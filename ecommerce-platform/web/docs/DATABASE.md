# Database setup

## Option A — MongoDB Atlas (recommended for production)

1. Create a free cluster at https://www.mongodb.com/cloud/atlas
2. Copy the connection string into `web/.env.local`:

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

## Option B — In-memory Mongo (local dev only)

```bash
npm run dev:db
```

Starts MongoDB Memory Server, writes `.env.local`, runs `db push` + seed, then Next.js.

> **Note:** Some cloud/sandbox networks block MongoDB binary downloads. If `dev:db` fails, use Atlas (Option A).
