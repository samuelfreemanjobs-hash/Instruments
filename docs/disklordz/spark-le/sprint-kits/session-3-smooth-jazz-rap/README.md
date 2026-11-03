# Session 3 — smooth jazz rap (SparkLE load pack)

**Preset:** `lane-smooth-jazz-rap` · **BPM:** 86 · **DNA:** `smooth-jazz-8ths` (no rolls)

## SparkLE standalone (≈15 min load)

1. In SparkLE, set project **BPM to 86**.
2. Import **OneShots/** files to pads **1→8** per `spark-le-manifest.json` roles (same order as charter).
3. Program **8th-note closed hats** with lighter velocity on off-8ths; **no 32nd rolls**.
4. Optional: load `Loops/Tops_Kickless_2bar.wav` as reference tops.
5. Export **30s mix** from SparkLE when the pocket feels right.

**Folder path (this repo):**

`docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap/`

Copy the whole folder to your machine if you generated on another host, then import into SparkLE.

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
