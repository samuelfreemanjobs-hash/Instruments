# CNV — Converter Night provenance

**SKU:** CNV · **Status:** DRAFT · **`release_ready`:** false  
**Date:** 2026-11-17 · **Operator:** Cursor agent (design pass)

## Source

- **Masters (when rendered):** original oscillator/noise / recipe synthesis via DRUMSMITH (`memphis-tape` + CNV bank recipes).  
- **Not included in ship tree:** third-party hardware sample archives.

## Generation

| Field | Value |
|-------|-------|
| Tool | DRUMSMITH native / Python factory (pending W2 render) |
| Commit / version | TBD at first render batch |
| Recipes | `kits/converter-night/recipes/*` |
| Random seed policy | Deterministic factory seed when used; document at render |
| Master → export | PCM16 44.1 mono copy into storefront 01–04 |

## Third-party assets

| Asset | Role | License / clearance |
|-------|------|---------------------|
| 26 unique hardware zips under `kits/converter-night/references/` | **Analysis-only RE** | Not cleared for redistribution; must not appear in `01_RAW…`–`04_…` |
| Boss_DR-5_9f45.zip | Duplicate of dr5 | **Rejected / not staged** |
| DR-660, SP-1200/Drumulator, SR-16, Planet Phatt | Text / research only | No WAV in kit yet |

Full list: `kits/converter-night/references/INVENTORY.md` and `R2P.md`.

## Approvals (HITL — empty until human)

| Gate | Reviewer | Date | Evidence |
|------|----------|------|----------|
| Listening | — | — | — |
| Release | — | — | — |
| Legal / EULA final | — | — | — |
| Price / channel | — | — | — |
| Cover art | — | — | — |
| Device tests | — | — | All targets **UNVERIFIED** |

## Device profiles

Generic WAV PCM16: INTERNAL after render.  
FL Studio / MPC / Maschine+ / MV-1 / Lofi12 / SP-404: **UNVERIFIED**.

## Notes

Technical `verify_kit` PASS ≠ commercial release. See `docs/RELEASE_GATE.md` and kit `status.json`.
