# SOP-010 — Batch open-source integration (research → hub)

## Purpose

Evaluate many GitHub repos (skills, agents, automation) and integrate **without** copying entire upstream trees.

## Pattern used (35-repo effort)

1. **Research list** — Document in [DISKLORDZ_OPEN_SOURCE_REFERENCES.md](../../DISKLORDZ_OPEN_SOURCE_REFERENCES.md).
2. **Hub manifest** — One row per repo in `disklordz/integrations/manifest.json`.
3. **Wiring** — Prefer: Copilot skill, script shim, API route, or doc pointer — not submodule of full app.
4. **Profit agents** — Map high-value automations to `disklordz/agents/profit/_specs.json` (15 → 29 → 31 with orchestration).
5. **META charter** — `./scripts/sync-meta-llm-charter.sh` → `CLAUDE.md` + `.claude/skills/` for shared discipline.
6. **Verify** — `verify-integrations.sh` + `/api/integrations/status`.
7. **Follow-up CI** — `.github/workflows/activate-integrations.yml` for migrations/RAG/Vercel env when secrets exist.

## Anti-patterns

- Vendoring 35 full repos into `third_party/`
- Announcing agents only in a deep folder without `DISKLORDZ_AGENTS.md`
- Marking `integrated` without a check or route

## Related agents

**integration-health**, **engine-swap**, **ship-velocity**
