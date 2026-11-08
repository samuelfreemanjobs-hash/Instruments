# Incident / debug — Disklordz SaaS

## Symptom

- URL / route:
- User impact:
- Started:

## Reproduce [executed]

```bash
export DISKLORDZ_URL=https://…
bash disklordz/website/scripts/verify-go-live.sh
bash disklordz/integrations/scripts/verify-integrations.sh
```

Paste failing step output.

## Evidence

- `/api/integrations/status` JSON (redact secrets):
- Generate response snippet:
- CI run link:

## Hypothesis

1. …

## Fix + verify

- PR:
- Re-run verify scripts:
- Agent owner: `conversion-qa` | `integration-health` | `async-generation`

## Post-incident

- [ ] Update SOP or runbook if gap found
- [ ] `docs/ROADMAP.md` if milestone regressed
