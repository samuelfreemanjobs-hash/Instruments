# Drum machine sound engine

Procedural **original** drum voices modeled after machines common in **1990s Memphis rap** and modern **phonk** — algorithmic synthesis only, not commercial sample libraries.

## Synthesized-only (Drum Factory Phase 1)

**DiskLordz Drum Factory** (`phonk_kit_factory.py`, `drum_kit_builder.py`, catalog `full_kit` products) renders **exclusively synthesized** audio. No external one-shots, no licensed WAV imports, and no pre-rendered `*phonk*.wav` loop stems in export ZIPs. Recipes + seeds provide bit-exact reproduction.

The Lofi-12 **hardware workflow** (line-in sampling from generated banks) is separate: you may still record the factory’s synthesized WAVs into hardware — that does not violate the synthesis rule.

> **Doc fix (former “Honest limits”):** Older text suggested layering recorded or licensed one-shots for commercial releases. That applies only if you **manually** augment kits outside this factory. Phase 1 automated kits are **synthesis-only by policy**.

## Engines (neutral IDs)

| ID | Machine / vibe | Character |
|----|----------------|-----------|
| **`hard_dirt_808`** | Memphis dirt 808 | **Default for memphis/phonk** — 808+sub, parallel dirt, 10-bit, tape hiss, crunchy snare |
| **`traproom_tape`** | SP-1200-style trap | 12-bit grit + trap 808 click/sub |
| `tr808` | Roland TR-808 | Long sine kick, noisy snare, short hats |
| `tr909` | Roland TR-909 | Tighter kick click, brighter snare, sharper hats |
| `boss_dr660` | Boss DR-660 | 12-bit lean, boxy transients |
| `alesis_sr16` | Alesis SR-16 | Duller 90s sample-style filter |
| `roland_r8mk2` | Roland R-8 MKII | Fuller snare body, electronic punch |
| `tape_slowdown` | Slowed tape | Wow, heavy tape + bit crush |
| `mr_tape` | Tape-warped kit | Hiss, warmth, 12-bit lo-fi |
| `classic` | Legacy synth | Original simple `phonk_synth` voices |

Legacy ids `juicy_j`, `dj_toomp`, `dj_screw` → alias table in `phonk_machine_engine.ENGINE_LEGACY_ALIASES`.

## Prompt keywords (neutral)

- `hard dirt`, `dirty memphis`, `memphis`, `phonk`, `traproom`, `tape slow`, `screw`, `808`, `909`, `splice`, `mr tape`

## CLI

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --engine mr_tape \
  --prompt "1990s memphis phonk bounce 118" \
  --batch 4 --out ~/Music/Lofi12/PhonkLoops

python3 disklordz/lofi12-phonk-factory/scripts/phonk_kit_factory.py \
  --mode phonk_drift --kit-size full --out ~/Music/DiskLordz/DriftKit
```

Implementation: `scripts/phonk_machine_engine.py` + **`scripts/phonk_fm.py`** (2-operator FM for cowbells, rim, clave-style perc).

Cowbells/rim use **FM** when `fm_perc ≥ 0.5` (default on). `hard_dirt_808` / tape engines use a darker Memphis FM bell; `tr808`-style engines use dual FM burst cowbell.

## Honest limits

Algorithmic approximation — not sampled ROMs from Roland/Boss/Alesis. For **factory export**, do not bolt on unverified third-party samples; extend DSP/recipes instead.
