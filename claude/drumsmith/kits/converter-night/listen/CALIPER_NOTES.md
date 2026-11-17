# CALIPER draft — Converter Night (CNV)

**Auditor:** Cursor agent draft (CALIPER Task subagent unavailable this turn)  
**Date:** 2026-11-17  
**Kit:** `claude/drumsmith/kits/converter-night/`  
**Kit size:** 384 (SKU max; factory ceiling 512)

**Verdict for commercial release:** **BLOCKED/UNVERIFIED**  
**Technical verifier:** PASS (observed) · **Diversity:** PASS (observed) · **`release_ready`:** false

## Observed

| Artifact | Evidence |
|----------|----------|
| 384 masters + SHA in manifest | `manifest.json` (local), `masters/*.wav` |
| `technical_status: PASS`, `errors: []` | `qa-report.json` |
| Warnings: listening/release gates; devices unverified | `qa-report.json` → `warnings` |
| Diversity PASS; no exact duplicates | `diversity-report.json` |
| Advisory similar envelopes (many hat/FX pairs) | `diversity-report.json` → `similar_envelope_pairs_advisory` |
| Pitch screen PASS on CNV-001 @ 52 Hz target | `qa-report.json` → files[0].pitch_check (approx. meter) |
| Human approvals null | `status.json` / `HITL.md` |
| Provenance lists refs as analysis-only | `provenance.md`, `docs/provenance/converter-night.md` |

## Flags

- **Sample peak ≠ true peak** — verifier reports `sample_peak_dbfs` only.
- **Pitch check is approximate** — autocorrelation/spectral screen; not certified tuning.
- **Loops upgraded** to true 2-bar PCM via `render_loop_from_blueprint.py` (`W2B_TRUE_LOOPS.md`) — still need human ear; file length = `loop_samples(bars)` not `bars * samples_per_bar`.
- **Devices UNVERIFIED** — do not market MPC/Maschine+/SP-404 compatibility.
- **No FORGE/VST claim** from this kit.

## Assumed

- Memphis-tape genre multipliers produce “converter” character without RE lock on DR-660/SP-1200 `[assumed` until refs + ear].

## Remediation before ship

1. Human listening log → `approvals.listening`  
2. Release owner → `approvals.release`  
3. Legal EULA final; price/cover HITL  
4. Optional: re-render weak similar pairs after ear  
5. Replace FX loop stubs with true `loop_samples`-aligned renders if selling loops as such  

**Release recommendation:** do **not** set `release_ready=true`. Technical PASS only.
