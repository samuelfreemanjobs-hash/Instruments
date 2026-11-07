# Grok Bot + phone PWA (revised stack)

You already have **Grokbot** (X / Cursor Pro). **Grok Bot is the cloud backend** — persistent VM, browser, terminal, git. You do **not** need to host FastAPI + LangGraph + E2B on Render for production mobile use.

## Topology

```text
Phone ──► Next.js PWA (/remote-agent)
            │
            └── POST /api/agent ──► GROK_INFERENCE_HOOK_URL (Grok inference hook)
                      │
                      ▼
              Grok Bot cloud VM ──► GitHub / tests / draft PRs
                      │
                      └── (optional) Composio MCP ──► Cursor Cloud Agent
```

| Lane | Role |
|------|------|
| **Claude Code** (laptop) | Foreground CLI — git, single-file, local compile |
| **Grok Bot** (cloud) | Async general contractor — clone, test, PRs |
| **Cursor Cloud Agent** | Multi-file harness (delegated by Grok via MCP/Composio) |
| **Next.js PWA** | Thin remote control — webhooks only |

Optional: `tools/coding-agent-stack/` remains a **local dev harness** (VST compile smoke, dry-run ACP payloads), not production hosting.

## 1. Vercel env (one-time)

In `disklordz/website/.env` / Vercel project settings:

| Variable | Purpose |
|----------|---------|
| `GROK_INFERENCE_HOOK_URL` | Grok Bot inference / ACP hook URL from your dashboard |
| `CURSOR_AGENT_API_URL` | Optional Cursor Cloud Agent API URL if Grok delegates there |
| `CURSOR_API_KEY` | Bearer token from Cursor/X developer settings |
| `GROK_SESSION_ID` | Your bot workspace session |
| `GROK_SIGNING_SECRET` | Sent as `X-Grok-Signing-Secret` |
| `GROK_BOT_TOKEN` or `CURSOR_API_KEY` | Bearer auth if required by hook |
| `GROK_DEFAULT_REPO_URL` | Default git remote for preset buttons |

Copy from Grok Bot → **Developer / Webhooks / Inference hooks** (exact label varies by xAI/Cursor UI).

## 2. Mobile UI

- **URL:** `/remote-agent` (install to home screen — `remote-agent-manifest.json`)
- **Presets:** QA build · Bug squash · Code review
- **Custom:** free-text → same hook

Implementation: `disklordz/website/src/app/api/agent/route.ts`, `src/lib/grok/inference-hook.ts`.

## 3. AGENTS.md (no rewrite)

Grok Build reads **`AGENTS.md`** like Claude Code. Optional `GROK_AGENTS_MD_EXCERPT` on Vercel for a short pinned snippet; otherwise the hook appends standard read-`AGENTS.md` instructions.

## 4. Composio MCP (Grok ↔ GitHub ↔ Cursor)

In Grok Bot desktop / x.ai bot:

1. Add **Composio** plugin  
2. Connect GitHub + Cursor accounts  
3. Grok runs git/test/PR flows while the laptop is closed  

Approve sensitive steps from the PWA chat or Grok mobile app.

## 5. Completion webhooks (phone notifications)

1. In Vercel env set `GROK_CALLBACK_PUBLIC_URL=https://<app>/api/agent/webhook` (or rely on `VERCEL_URL`).
2. In Grok Bot, set the same URL as the task **callback** / webhook target.
3. Grok POSTs JSON: `{ "client_ref": "<requestId>", "status": "completed", "summary": "…" }`
4. Optional `GROK_WEBHOOK_SECRET` — HMAC `X-Grok-Signature: sha256=<hex>` (body HMAC-SHA256).
5. PWA polls `GET /api/agent/events?clientRef=` every 12s and fires **Web Notifications** when allowed.

Apply migration `supabase/migrations/20261107210000_grok_agent_events.sql` for durable storage (service role). Without Supabase, events use in-memory fallback (dev only).

## 6. Dry-run without hook URL

If `GROK_INFERENCE_HOOK_URL` is unset, `POST /api/agent` returns **202** `dryRun: true` — safe for local `npm run dev`.

## Related

- [CODING_AGENT_STACK.md](CODING_AGENT_STACK.md) — optional Python stack  
- [disklordz/website/DEPLOY.md](../disklordz/website/DEPLOY.md)
