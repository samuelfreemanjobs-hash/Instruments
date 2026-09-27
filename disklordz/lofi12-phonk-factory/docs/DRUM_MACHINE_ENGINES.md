# Drum machine sound engine

Procedural **original** drum voices modeled after machines common in **1990s Memphis rap** and modern **phonk** (in the spirit of lo-fi tape/sample-pack aesthetics — not copies of commercial sample libraries).

## Engines

| ID | Machine / vibe | Character |
|----|----------------|-----------|
| `tr808` | Roland TR-808 | Long sine kick, noisy snare, short hats |
| `tr909` | Roland TR-909 | Tighter kick click, brighter snare, sharper hats |
| `boss_dr660` | Boss DR-660 | 12-bit lean, boxy transients |
| `alesis_sr16` | Alesis SR-16 | Duller 90s sample-style filter |
| `roland_r8mk2` | Roland R-8 MKII | Fuller snare body, electronic punch |
| `dj_screw` | DJ Screw tape | Slowed pitch, wow, heavy tape + bit crush |
| `mr_tape` | Tape-warped kit | **Default for memphis/phonk prompts** — hiss, warmth, 12-bit, Splice-pack *style* |
| `classic` | Legacy synth | Original simple `phonk_synth` voices |

## Prompt keywords

- `808`, `909`, `dr660`, `boss`, `sr16`, `alesis`, `r8`, `screw`, `splice`, `mr tape`

## CLI

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --engine mr_tape \
  --prompt "1990s memphis phonk dj paul 84" \
  --batch 4 --out ~/Music/Lofi12/PhonkLoops

python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --engine roland_r8mk2 --prompt "memphis r8 snare" --out ~/Music/Lofi12/bank_r8
```

Implementation: `scripts/phonk_machine_engine.py`.

## Honest limits

This is **algorithmic** approximation — not sampled ROMs from Roland/Boss/Alesis and **not** a clone of any third-party Splice pack. For commercial releases, layer recorded one-shots or licensed kits on top.
