# Session 3 — smooth jazz rap (MPC500 load pack)

**Preset:** `lane-smooth-jazz-rap` · **BPM:** 86 · **DNA:** `smooth-jazz-8ths` (no rolls)

## Export to the CF card

```powershell
cd disklordz\website
npm run export:mpc500 -- --kit ../../docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap --out E:\DISKLORDZ\SJR86
```

Replace `E:` with your card drive. Output: `SJR86_01SUB.WAV` … `SJR86_08FX.WAV`, `SJR86_L1TOPS.WAV`, and `PADS.TXT` (≈0.6 MB total).

## MPC500 session (≈15 min load)

1. Load the `SJR86_*` samples from the card.
2. New program; assign pads **A01→A08** per `PADS.TXT` (same order as the charter). **Save the program** as `SJR86`.
3. Sequence **BPM 86**; light swing (54–58%) on 8th hats.
4. Program **8th-note closed hats** (A05) with lighter velocity on off-8ths; **no 32nd rolls** (leave Note Repeat off for hats).
5. Optional: `SJR86_L1TOPS` on a free pad as reference tops.
6. Record a **30s mix** when the pocket feels right.

**Folder path (this repo):**

`docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap/`

`spark-le-manifest.json` keeps its legacy name; the exporter reads it as-is.

## Regenerate factory WAVs

```bash
cd disklordz/website
npx tsx scripts/export-spark-sprint-kit.ts \
  --manifest ../../docs/disklordz/spark-le/examples/manifest-smooth-jazz-rap.json \
  --out ../../docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap \
  --prompt "smooth jazz rap smoky cruise no rolls" \
  --seed 86031103
npm run verify:spark
```

## Review

File session notes: [reviews/2026-11-03_SMOOTH-JAZZ-RAP_sprint3.md](../reviews/2026-11-03_SMOOTH-JAZZ-RAP_sprint3.md)
