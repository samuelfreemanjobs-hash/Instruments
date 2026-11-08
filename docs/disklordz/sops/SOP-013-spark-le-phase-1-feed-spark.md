# SOP-013 — Phase 1: Feed the MPC500

**Phase:** 1 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.1

## Purpose

Get **one-shots and loops** onto the **Akai MPC500** fast, every time — same pad map, predictable folders, no DAW required. Rig: [MPC500-PC-ONLY.md](../spark-le/MPC500-PC-ONLY.md) · Card export: [MPC500-CARD-EXPORT.md](../spark-le/MPC500-CARD-EXPORT.md).

## Prerequisites

- MPC500 + CF card (card reader or MPC500 USB mode to reach it from the PC).
- "Verify on your unit" checklist in MPC500-CARD-EXPORT.md done once (RAM, name length, WAV format).
- Lane chosen: `GANGSTA-TRAP` | `PHONK-TRAP` | `VINTAGE-SOUTH` (or any `lane-*`).
- Kit from sound-factory / Disklordz generate / `export-spark-sprint-kit.ts` / manual export.

## Procedure

### 1. Plan the drop

1. Set **BPM** and **lane** (write in session notes).
2. Target **one folder** containing:
   - `OneShots/` — 8 files mapped to pads A01–A08 (see SOP-012).
   - `Loops/` — optional 1–2 bar stems (kick-only, kickless tops, full reference).
   - `spark-le-manifest.json` — copy from [template](../templates/TEMPLATE-spark-le-manifest.json). Add `overflow[]` entries with `mpc_pad` 9–12 for extra pads.

### 2. Prepare audio (minimum bar)

1. **808 sub (A01):** mono-compatible low end; trim silence tails (tails cost MPC RAM).
2. **Knock (A02):** short; do not overpower sub in manifest levels.
3. **Gangsta trap:** prioritize **knock + clap stack** clarity over length.
4. **Phonk:** allow dirt on snap (A04); dull hats vs gangsta trap.
5. **Vintage south:** fat A03 before harsh A04.
6. **16-bit / 44.1 kHz WAV only.** Factory output already is; convert DAW keepers before export.

### 3. Export to the card

```powershell
cd disklordz\website
npm run export:mpc500 -- --kit <kit folder> --out E:\DISKLORDZ\<CODE>
```

1. Exporter must print `OK`; any error (bit depth, name clash, RAM) is fixed before the session, not on the MPC.
2. Keep `PADS.TXT` open (or printed) for pad assignment.

### 4. MPC500 session (load)

1. Load the `<CODE>_*.WAV` samples from the card.
2. New program; assign pads **A01→A08** per `PADS.TXT` (do not reorder casually). A09–A12 only if listed.
3. **Save the program** to the card next to the samples (`<CODE>.PGM`) — next load is one step.
4. Set sequence BPM from `PADS.TXT`; apply the swing note.
5. **15-minute rule:** audible beat with one-shots **or** one loop stack — if not, tag `workflow` in review.

### 5. Export proof

1. Record a **30s mix** (MPC out → PC/recorder, or bounce in a DAW from MPC audio).
2. Store with `batch_id` for later phases.

## KPIs (Phase 1)

| Metric | Target | Improve when |
|--------|--------|--------------|
| Time to first beat (generate → audible) | ≤ 20 min | > 30 min two sessions in a row |
| Pad map compliance | 100% | Any ad-hoc pad swap without manifest note |
| Exporter rejects at session time | 0 | Any → fix factory or manifest, not the MPC |
| Clipping on export | 0 | Any clip → fix normalization |

## Success criteria

- [ ] 8 one-shots on A01–A08 per `PADS.TXT`
- [ ] Program saved on card
- [ ] Manifest committed or saved beside pack
- [ ] 30s recording exists
- [ ] Session review started ([template](../templates/TEMPLATE-spark-le-session-review.md))

## Improvement loop

- If **workflow** scores ≤ 3 twice: prioritize the `.PGM` writer (see MPC500-CARD-EXPORT.md) and naming, not new DSP.
- Update this SOP when pad law or folder layout changes.

## Escalation

- Sample refuses to load or names display wrong: record the MPC500 behavior in MPC500-CARD-EXPORT.md "Verify on your unit" and adjust the exporter rule; do not hand-rename on the MPC.

**SOP-013 v1.1**
