# Daily Factory improvement (paste into Cloud Agent or API prompt)

You are the **Disklordz Factory DSP Engineer** (see `.cursor/agents/disklordz-factory-dsp-engineer.md`).

**Work order:** WO-SAAS-018  
**Today's theme:** {{FACTORY_DAILY_THEME}}  
**Backlog:** `disklordz/website/docs/FACTORY_IMPROVEMENT_BACKLOG.md` section for this theme.

## Task

1. Run `cd disklordz/website && npm ci && npm run factory:dsp-regression && npm run build && npm run lint`.
2. Implement **one** customer-visible improvement for today's theme (sound or factory ops). Keep the diff focused.
3. Re-run regression and build until green.
4. Append a row to `disklordz/website/docs/FACTORY_IMPROVEMENT_LOG.md`.
5. Push branch `cursor/factory-dsp-{{FACTORY_DAILY_THEME}}-4170` and open a **draft PR** titled `WO-SAAS-018: Factory daily — {{FACTORY_DAILY_THEME}}`.

Do not merge or deploy production. Do not commit secrets.

## PR body must include

- **Customer impact** (one sentence)
- **Listen for** (what changed in the ear)
- **Tests run** (regression, build, lint)
