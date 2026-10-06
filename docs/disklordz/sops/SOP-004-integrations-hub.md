# SOP-004 — Integrations hub (35 OSS references)

## Purpose

Wire open-source tools **by reference** (manifest + scripts + API status), not by vendoring full upstream repos.

## Procedure

1. **Setup** — `./scripts/setup-open-source-integrations.sh` (or product doc in `disklordz/integrations/ARCHITECTURE.md`).
2. **Manifest** — Edit `disklordz/integrations/manifest.json`: `status` = `integrated` | `partial` | `external`, plus `wiring` path.
3. **Activate** (optional, secrets required) — `disklordz/integrations/scripts/activate-integrations.sh`  
   GitHub: `.github/workflows/activate-integrations.yml` (workflow_dispatch).
4. **Verify** — `bash disklordz/integrations/scripts/verify-integrations.sh` with `DISKLORDZ_URL` pointing at running site.
5. **Status API** — Confirm `GET /api/integrations/status` shows `repoCount: 35`.
6. **Roadmap** — `python3 scripts/update-roadmap-progress.py` after manifest changes.

## Partial → integrated criteria

- [ ] Documented wiring path exists in repo
- [ ] At least one automated check or API route uses the integration
- [ ] Env vars named in `disklordz/website/.env.example` (no values)

## Template

[New manifest row](../templates/TEMPLATE-integration-manifest-row.md)
