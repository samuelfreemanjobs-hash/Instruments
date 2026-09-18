# Vercel env vars, redeploy, and Supabase MCP

You already linked **Vercel** → repo `Instruments` → root **`disklordz/website`**. Finish production with env vars, one redeploy, then optional Cursor MCP.

---

## A. Vercel environment variables (Production)

### Where to click

1. [vercel.com](https://vercel.com) → your **Instruments** project.
2. **Settings → Environment Variables**.
3. For each row below: **Add** → name + value → check **Production** (and **Preview** if you test billing there) → Save.

### What to add

| Name | Where the value comes from | Notes |
|------|----------------------------|--------|
| `NEXT_PUBLIC_SUPABASE_URL` | Supabase → **Project Settings → API** → Project URL | Safe in browser (`NEXT_PUBLIC_`) |
| `NEXT_PUBLIC_SUPABASE_ANON_KEY` | Same page → `anon` `public` key | Safe in browser |
| `SUPABASE_SERVICE_ROLE_KEY` | Same page → `service_role` **secret** | **Server only** — kits storage + credits |
| `STRIPE_SECRET_KEY` | Stripe → Developers → API keys | Use test keys on Preview, live on Production |
| `STRIPE_WEBHOOK_SECRET` | Stripe → Webhooks → your endpoint → Signing secret | After creating webhook (section C) |
| `STRIPE_PRO_PRICE_ID` | Stripe → Product → recurring Price id `price_...` | See [STRIPE.md](STRIPE.md) |
| `SAAS_DAILY_GEN_LIMIT` | Optional, e.g. `20` | Guest IP batch cap |

Never put `SUPABASE_SERVICE_ROLE_KEY` in a `NEXT_PUBLIC_` variable.

### Redeploy (required after env changes)

**Settings → Environment Variables** only stores values; running deployments keep old env until redeployed.

- **Deployments** tab → latest **Production** deployment → **⋯ → Redeploy** → Production.
- Or push any commit to `main` (Vercel auto-builds).

Copy your production URL (e.g. `https://instruments-xxx.vercel.app`) for the steps below.

---

## B. Supabase Auth URL (one time)

Supabase → **Authentication → URL configuration**:

| Field | Value |
|--------|--------|
| Site URL | `https://YOUR_VERCEL_DOMAIN` |
| Redirect URLs | `https://YOUR_VERCEL_DOMAIN/auth/callback` |

---

## C. Stripe webhook (one time)

Stripe → **Developers → Webhooks → Add endpoint**:

- URL: `https://YOUR_VERCEL_DOMAIN/api/stripe/webhook`
- Events: `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted`

Put the signing secret in Vercel as `STRIPE_WEBHOOK_SECRET`, then **redeploy** again.

---

## D. SQL migrations

**Option 1 — GitHub Actions** (repo secrets `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF`):

Actions → **Disklordz Supabase migrations** → Run workflow.

**Option 2 — CLI** from `disklordz/website`:

```bash
export SUPABASE_ACCESS_TOKEN=...
export SUPABASE_PROJECT_REF=...
bash scripts/apply-supabase-migrations.sh
```

**Option 3 — Supabase MCP** in Cursor (section F) then ask the agent to run migration SQL / `db push`.

---

## E. Smoke test

```bash
cd disklordz/website
DISKLORDZ_URL=https://YOUR_VERCEL_DOMAIN npm run verify:go-live
```

Then manually: sign in, generate, `/account`, product pack, Stripe test checkout.

---

## F. Supabase MCP in Cursor (one time)

Repo config: [`.cursor/mcp.json`](../../../.cursor/mcp.json). Details: [SUPABASE_MCP.md](SUPABASE_MCP.md).

### 1. Shell env (macOS / Linux)

Add to `~/.zshrc` or `~/.bashrc`, then **restart Cursor**:

```bash
export SUPABASE_PROJECT_REF="abcdefghijklmnop"   # from https://REF.supabase.co
export SUPABASE_ACCESS_TOKEN="sbp_..."           # optional; Account → Access tokens
```

### 2. Connect in Cursor

**Cursor Settings → MCP** → server **supabase** → **Connect** → browser OAuth → pick org/project.

### 3. Cloud Agents (this repo’s remote agents)

In [Cloud Agent environment](https://cursor.com/dashboard/cloud-agents) for **Instruments**:

1. **Secrets:** add `SUPABASE_PROJECT_REF` and `SUPABASE_ACCESS_TOKEN` (same names as above).
2. **Network / egress:** allow `mcp.supabase.com` and your project host `*.supabase.co` if you use a restricted allowlist (default cloud egress may already allow `*.supabase.co` via `google.com` / general lists — add `mcp.supabase.com` explicitly if MCP fails to connect).

MCP does **not** configure Vercel; it only lets agents talk to Supabase from the IDE.

### Stripe MCP (payments)

[STRIPE_MCP.md](STRIPE_MCP.md) — install **Stripe** Cursor plugin or connect **stripe** in MCP settings (`https://mcp.stripe.com`). Optional env: `STRIPE_RESTRICTED_KEY` for bearer auth. Use **`stripe_implementation_planner`** in a new chat for a tailored plan.

---

## G. CLI alternative (Vercel)

If the project is linked locally (`vercel link` in `disklordz/website`):

```bash
cd disklordz/website
export VERCEL_TOKEN=...   # vercel.com/account/tokens
bash scripts/push-vercel-env.sh
vercel deploy --prod
```

See script header for interactive `vercel env add` flow.
