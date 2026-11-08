# Cloud Agent prompt — Disklordz (copy below the line)

---

## Goal

[One sentence: what ships when this is done]

## Context

- Read: `ARCHITECTURE.md`, `AGENTS.md`, `DISKLORDZ_AGENTS.md`, `docs/disklordz/README.md`
- Product: `disklordz/website/ARCHITECTURE.md` (or other product ARCHITECTURE)
- Branch: `cursor/<feature>-<suffix>`
- Work order: `WO-SAAS-___` (if applicable)

## Requirements

1. …
2. …

## Out of scope

- Production deploy unless explicitly requested
- Force-push / amend commits unless requested
- …

## Success criteria

- [ ] `cd disklordz/website && npm ci && npm run build`
- [ ] `DISKLORDZ_URL=http://127.0.0.1:3000 bash disklordz/website/scripts/verify-go-live.sh` (if SaaS touched)
- [ ] `./scripts/sync-disklordz-agent-fleet.sh` (if agents/workflows/specs touched)
- [ ] UI: screenshot or video artifact
- [ ] Update `ARCHITECTURE.md` / `docs/ROADMAP.md` if milestones moved

## Security

- No secrets in git; `.env.example` names only
- Validate API inputs; rate-limit public POSTs
- Stripe/Supabase prod writes need explicit confirmation

## Deliverable

- Commits pushed; draft PR with evidence; do not merge unless asked
