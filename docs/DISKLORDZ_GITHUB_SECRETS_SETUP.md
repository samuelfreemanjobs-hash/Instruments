# Disklordz — GitHub Actions secrets setup (safe)

**Repository:** [samuelfreemanjobs-hash/Instruments](https://github.com/samuelfreemanjobs-hash/Instruments)

Use this guide to load **Disklordz go-live** secrets into GitHub Actions without putting credentials in git, chat, or committed files.

Related: [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md), [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md), workflow [`.github/workflows/disklordz-go-live.yml`](../.github/workflows/disklordz-go-live.yml).

---

## Warnings

- **Never** commit `.env` files, tokens, keys, or webhook secrets to the repository.
- **Never** paste secret values into Cursor chat, issues, or PRs.
- The helper script only uploads to GitHub via `gh`; it does not write secrets into the repo.
- **Do not** run **Disklordz go-live** until you intend to migrate, sync env, deploy, and smoke-test production.

---

## Secret names (values are opaque — do not document real values here)

### Required

| Secret | Where to get the value |
|--------|-------------------------|
| `SUPABASE_ACCESS_TOKEN` | [Supabase → Account → Access tokens](https://supabase.com/dashboard/account/tokens) |
| `SUPABASE_PROJECT_REF` | Supabase project URL subdomain (`https://<ref>.supabase.co`) |
| `DISKLORDZ_URL` | Vercel production URL, **no** trailing `/` |
| `VERCEL_DEPLOY_HOOK_URL` | Vercel → Project → Settings → Git → Deploy Hooks → Production |
| `VERCEL_TOKEN` | [vercel.com/account/tokens](https://vercel.com/account/tokens) |
| `VERCEL_PROJECT_ID` | Vercel → Project → Settings → General → Project ID |
| `NEXT_PUBLIC_SUPABASE_URL` | Supabase → Project Settings → API → Project URL |
| `NEXT_PUBLIC_SUPABASE_ANON_KEY` | Supabase → API → `anon` public key |
| `SUPABASE_SERVICE_ROLE_KEY` | Supabase → API → `service_role` secret key |
| `STRIPE_SECRET_KEY` | Stripe → Developers → API keys → Secret key |
| `STRIPE_WEBHOOK_SECRET` | See **Stripe webhook** below (`whsec_...`) |
| `STRIPE_PRO_PRICE_ID` | Stripe → Pro product → Price id `price_...` |

### Optional

| Secret | When to set |
|--------|-------------|
| `SAAS_DAILY_GEN_LIMIT` | Skip to use default `20` |
| `VERCEL_TEAM_ID` | Only if the Vercel project lives under a team |

---

## Stripe webhook secret (`STRIPE_WEBHOOK_SECRET`)

You need a signing secret before **Sync Vercel env** can configure Stripe on Vercel.

1. Ensure `STRIPE_SECRET_KEY` and `DISKLORDZ_URL` are already GitHub secrets.
2. **GitHub → Actions → Disklordz go-live → Run workflow** with **Register Stripe webhook: on** (sync can stay off).
3. In the job log, step **Register Stripe webhook**, copy `STRIPE_WEBHOOK_SECRET=whsec_...` (do not paste into chat).
4. Re-run the helper script for `STRIPE_WEBHOOK_SECRET` only, or set it in **Settings → Secrets → Actions**.
5. Run go-live again with **Sync Vercel env: on**, **Register Stripe webhook: off**.

Local alternative: `disklordz/website/scripts/setup-stripe-webhook.sh` with `STRIPE_SECRET_KEY` and `DISKLORDZ_URL` exported in your shell (not in a committed file).

Manual: Stripe → Webhooks → `https://YOUR_DOMAIN/api/stripe/webhook` → events `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted`.

---

## Prerequisites

### Install GitHub CLI

- **macOS:** `brew install gh`
- **Windows:** `winget install GitHub.cli` or [cli.github.com](https://cli.github.com/)
- **Linux:** see [GitHub CLI installation](https://github.com/cli/cli#installation)

Also need **git** (usually already installed).

### Authenticate `gh`

```bash
gh auth login
```

Choose:

- **GitHub.com**
- **HTTPS**
- **Login with a web browser**

Do **not** paste a personal access token into Cursor chat or commit it to a file.

Confirm access:

```bash
gh auth status
gh repo view samuelfreemanjobs-hash/Instruments
```

Your account needs permission to manage **Actions secrets** on this repo (typically repo admin or custom role with secrets write).

Check secret **names** only (never values):

```bash
gh secret list --repo samuelfreemanjobs-hash/Instruments
```

If this returns 403, use an account with admin access on the repository or set secrets in the GitHub UI.

---

## Run the helper script

From the repository root, on **your machine** (interactive terminal):

```bash
bash scripts/set-disklordz-github-secrets.sh
```

- Input is hidden (`read -s`); the script prints only `OK`, `SKIP`, or `FAIL` per secret name.
- Optional secrets: press **Enter** to skip.
- The script does not create commits or pull requests.

---

## After secrets are set

1. Open [Actions](https://github.com/samuelfreemanjobs-hash/Instruments/actions).
2. Choose **Disklordz go-live** → **Run workflow**.
3. For a first pass with only minimum checks, turn **Sync Vercel env** off until `STRIPE_WEBHOOK_SECRET` exists (see [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md)).

Confirm explicitly before dispatching the workflow from Cursor or `gh workflow run`.
