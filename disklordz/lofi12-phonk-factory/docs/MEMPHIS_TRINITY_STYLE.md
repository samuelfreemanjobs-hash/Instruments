# Juicy J / DJ Paul / DJ Toomp style

This is the **default aesthetic** for the Lofi-12 phonk factory: dirty Memphis 808s, SP-1200-style sample crunch, and early Atlanta trap weight—without trap hat rolls or sample packs.

## What each piece does

| Name | Lane (`--lane`) | Engine (`--engine`) | Feel |
|------|-----------------|---------------------|------|
| **Juicy J** | `juicy_j` | `juicy_j` (default) | 10-bit dirt, tape hiss, FM cowbell, clap-heavy |
| **DJ Paul** | `dj_paul` | `juicy_j` | Bounce kick grid, extra cowbell, darker FX |
| **DJ Toomp** | `dj_toomp` | `dj_toomp` | 12-bit SP-1200 nod, harder kick click, more sub |

Mention names in the **prompt** or use **`--preset`** so groove + tone stay aligned.

## One-command presets

```bash
# Combined stack (good default for the sequencer + Lofi)
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --preset memphis_trinity --batch 4 --out ~/Music/Lofi12/Trinity

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --preset juicy_j --out ~/Music/Lofi12/Juicy

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --preset dj_paul --out ~/Music/Lofi12/Paul

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --preset dj_toomp --out ~/Music/Lofi12/Toomp
```

## One-shot bank (same presets)

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --preset memphis_trinity --out ~/Music/Lofi12/Trinity/bank_a
```

## Sequencer workflow

```bash
python3 disklordz/lofi12-phonk-factory/sequencer/serve.py
```

Use the **Style** dropdown (Juicy J / Paul / Toomp / Trinity) → **Apply preset** → **Load groove → grid** (6 tracks) → **Generate backing loop** → **Play**. Live **pads** or keys **1–6** for practice over the loop.

Groove prompt examples:

- `juicy j dj paul dirty memphis 86`
- `dj paul three 6 cowbell 84`
- `dj toomp atlanta trap 808 78`

Use **Generate backing loop** → **Play** → edit the 6×16 grid → MIDI to Lofi.

## BPM guide

- **Juicy J / Paul:** ~84–86 (classic Memphis slow bounce)
- **Toomp lane:** ~78 default (heavy half-feel); add `140 bpm` in prompt only if you want drift-style half-time programming

## Hardware note

SP-1200 character is **emulated** (12-bit / sample-hold in the Toomp engine + tape/drive on the loop bus)—not ROM samples. Line-in your rendered WAVs or one-shot bank to the Lofi-12 as today.
