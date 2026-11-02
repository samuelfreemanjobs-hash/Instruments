# SOP-008 — Roadmap and progress dashboard

## Purpose

Single view of **products**, **agents**, and **SaaS phases** for PM and agents.

## Canonical doc

[docs/ROADMAP.md](../../ROADMAP.md)

## Update procedure

1. **Milestone tables** — Edit manually when a WO phase ships (v0, 007+, fleet, integrations).
2. **Metrics block** — Auto-generated between `ROADMAP_PROGRESS_BEGIN/END`:

```bash
python3 scripts/update-roadmap-progress.py
```

Also runs at end of `./scripts/sync-disklordz-agent-fleet.sh`.

3. **Product audit table** — When adding a new CMake/npm product, add row with `ARCHITECTURE.md` path.
4. **PR** — Mention roadmap in PR body if milestone status changed.

## Sources of truth

| Metric | Source |
|--------|--------|
| Agent count / activation | `.github/agents/fleet.json` |
| Integration counts | `disklordz/integrations/manifest.json` |
| SaaS depth | [DISKLORDZ_ILLUGEN_RESEARCH.md](../../DISKLORDZ_ILLUGEN_RESEARCH.md) twelve-systems table |
