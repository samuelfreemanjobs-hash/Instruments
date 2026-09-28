# SOP-SAAS-001 — Disklordz web build and deploy smoke

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-web |
| **Consumer seats** | hermes-web, hermes-data, hermes-qa |
| **Cadence** | every_saas_pr |

## Purpose

Drum SaaS (`disklordz/website`) builds cleanly; deploy steps documented; no secrets in git.

## Procedure

1. Read [disklordz/website/ARCHITECTURE.md](../../website/ARCHITECTURE.md).
2. Local:
   ```bash
   cd disklordz/website && npm ci && npm run build && npm test
   ```
3. Schema changes: hermes-data reviews migrations; update `.env.example` names only.
4. Deploy: follow [disklordz/website/DEPLOY.md](../../website/DEPLOY.md) — **human** promotes production unless user explicitly asks agent to deploy.
5. MCP Supabase/Stripe: read docs; no write without user confirm ([security baseline](../../../.cursor/rules/security-baseline.mdc)).

## Verification

- `npm run build` exit 0.
- Nightly `run_business.py --profile full` includes web stage when enabled.

## Related

- [SOP-SEC-001](SOP-SEC-001-secrets-baseline.md)
- [SOP-DATA — use hermes_tool data checklist]
