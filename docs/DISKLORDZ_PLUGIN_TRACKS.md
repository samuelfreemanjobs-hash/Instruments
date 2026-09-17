# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **Implementer:** Cursor Cloud Agent · **Advisory:** Grok Plugin team  
**App track (SaaS):** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) — separate lane; do not mix iPlug2 WOs with web WOs in one PR.

## Active plugin products

| Track | Product | Path | Stack | Priority |
|-------|---------|------|-------|----------|
| **A** | **Junova-X** | [Junova-X/](../Junova-X/REPO_HANDOFF.md) | **JUCE** VST3 + CLAP | **P0** — JUCE port + host smoke (WO 001–003) |
| **B** | **NovaDrum** (TR-808 class) | [vst-tr808/](../vst-tr808/) | iPlug2 + VST3 | **P1** — Spec/DSP parallel; code after A |
| **C** | **JD Upgraded** | `Source/` | JUCE | Maintenance + CI unless WO ties to SaaS assets |

## Priority rule (solo + two Grok teams)

1. **Finish Junova-X** first host build (`Juno106-vst3` Release \| x64) and honest README status.
2. **TR-808 / NovaDrum:** Grok continues Spec + DSP skeleton **without blocking** Junova repo prep.
3. **No third plugin** greenfield until one of A or B reaches a ship candidate or is killed in Airtable.

## WIP (Factory Manager)

Max **2** implementation WOs. Example week during Junova push:

- WO-1 `[Plugin][Junova-X]` import tree + submodule + fetch-deps doc
- WO-2 `[Plugin][Junova-X]` Release x64 smoke + QA notes
- (Optional) WO-3 `[Plugin][NovaDrum]` voice table only — **docs**, no `plugin/` code

## Grok Plugin team prompt (attach to team)

Use the charter in [GROK_PLUGIN_TEAM.md](GROK_PLUGIN_TEAM.md). Add for this week:

> Continue **Junova-X** from `vst-juno106/REPO_HANDOFF.md`. Parallel: **NovaDrum** MVP in `vst-tr808/plugin-spec-mvp.md` — voice schematics for BD→CP first. Do not implement JUCE/JD Upgraded. Output Airtable-ready WOs with `[Plugin][Junova-X]` or `[Plugin][NovaDrum]` prefixes.

## Landing / GTM

- **Junova-X:** separate `junova-x-landing` repo (Vite, $29/$49).
- Store copy lives in `vst-juno106/gtm/` when imported.
