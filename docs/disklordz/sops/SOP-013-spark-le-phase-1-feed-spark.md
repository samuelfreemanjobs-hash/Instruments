# SOP-013 — SparkLE Phase 1: Feed Spark

**Phase:** 1 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.0

## Purpose

Get **one-shots and loops** into **SparkLE standalone** fast, every time — same pad map, predictable folders, no DAW required.

## Prerequisites

- SparkLE installed; know your **user sample import** path (OS-specific).
- Lane chosen: `GANGSTA-TRAP` | `PHONK-TRAP` | `VINTAGE-SOUTH`.
- Optional: kit from sound-factory / Disklordz generate / manual export.

## Procedure

### 1. Plan the drop

1. Set **BPM** and **lane** (write in session notes).
2. Target **one zip or folder** containing:
   - `OneShots/` — 8 files mapped to pads 1–8 (see SOP-012).
   - `Loops/` — optional 1–2 bar stems (kick-only, kickless tops, full reference).
   - `spark-le-manifest.json` — copy from [template](../templates/TEMPLATE-spark-le-manifest.json).

### 2. Prepare audio (minimum bar)

1. **808 sub (pad 1):** mono-compatible low end; trim silence tails.
2. **Knock (pad 2):** short; do not overpower sub in manifest levels.
3. **Gangsta trap:** prioritize **knock + clap stack** clarity over length.
4. **Phonk:** allow dirt on snap (pad 4); dull hats vs gangsta trap.
5. **Vintage south:** fat pad 3 before harsh pad 4.
6. Peak levels documented in manifest (no clipping in Spark).

### 3. Inbox (when automation exists)

1. Drop zip in watched folder or run inbox **once** script.
2. Verify rename rules: `Lane_BPM_PadName_V01.wav`.
3. Confirm files appear in SparkLE user sample browser.

### 4. SparkLE session (load)

1. Create/new kit; assign pads **1→8** per charter (do not reorder casually).
2. Load loops to spare pads or duplicate kit — note in manifest.
3. **15-minute rule:** audible beat with one-shots **or** one loop stack — if not, tag `workflow` in review.

### 5. Export proof

1. Export **30s mix** from SparkLE (WAV).
2. Store with `batch_id` for later phases.

## KPIs (Phase 1)

| Metric | Target | Improve when |
|--------|--------|--------------|
| Time to first beat (generate → audible) | ≤ 20 min | > 30 min two sessions in a row |
| Pad map compliance | 100% | Any ad-hoc pad swap without manifest note |
| Clipping on export | 0 | Any clip → fix inbox normalization |

## Success criteria

- [ ] 8 one-shots loaded on correct pads
- [ ] Manifest committed or saved beside pack
- [ ] 30s export exists
- [ ] Session review started ([template](../templates/TEMPLATE-spark-le-session-review.md))

## Improvement loop

- If **workflow** scores ≤ 3 twice: prioritize inbox + naming (not new DSP).
- Update this SOP when pad law or folder layout changes.

## Escalation

- Missing SparkLE path / import failure: document OS path in manifest repo doc; do not hack Arturia binaries.

**SOP-013 v1.0**
