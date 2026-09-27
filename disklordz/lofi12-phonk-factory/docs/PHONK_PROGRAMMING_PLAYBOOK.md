# Phonk & Memphis drum programming playbook

Agent and human reference for **1990s Memphis rap** and modern **drift phonk** loops without trap-clutter. Used by `phonk_groove.py` and RAG in this repo.

## Aesthetic target

| Era | Feel | Reference vibe |
|-----|------|----------------|
| **1990s Memphis** | Slow–mid (70–95 BPM), dirty 808, simple repeating patterns, cowbell accents | Three 6 Mafia, DJ Paul / Juicy J early |
| **Drift phonk** | 130–170 BPM *metadata*, **half-time feel**, sparse hats, hard snare/clap | YouTube drift edits — not festival trap |

**Avoid:** 32nd-note hat rolls, constant triplet fills, EDM build-ups, “extra fast” double-time grids when BPM is already high.

## Memphis reference lanes (your list)

Mention an artist in the **prompt** (or pass `--lane`) and `phonk_memphis_lanes.py` biases kicks, hat density, cowbell, swing, and tone.

| Lane ID | Names in prompt | Default BPM | Programming bias |
|---------|-----------------|-------------|------------------|
| `dj_paul` | dj paul, three 6, triple 6 | 84 | Bounce kicks, extra cowbell, clap-heavy snare |
| `juicy_j` | juicy j, juicy | 86 | Paul-adjacent; slightly brighter snare/clap |
| `dj_zirk` | dj zirk, zirk | 80 | Simpler drum-machine grid, sparse hats |
| `shawty_pimp` | shawty pimp, shawty | 74 | Slow, minimal hats, long 808 decay |
| `kingpin_skinny_pimp` | kingpin, skinny pimp, short pimp | 76 | Lean patterns, laid-back swing |
| `blackout` | blackout | 88 | Harder snare, more distorted kick layer |
| `toy_wright_iii` | toy wright, wright iii | 72 | Very slow, low hat count |
| `apoc_crisis` | apoc, apocalypse, crisis, apoc crisis | 79 | Dark, gritty, sparse bells |

Example:

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --prompt "dj paul style memphis 808 cowbell dirty" \
  --batch 4 --out ~/Music/Lofi12/PhonkLoops/paul

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --lane shawty_pimp --batch 2 --out ~/Music/Lofi12/PhonkLoops/shawty
```

Drop labeled reference WAVs under `references/` (see `references/README.md`) to tune weights later.

## BPM policy (60–190)

- **60–95:** Full 4-bar loops, 8th hats with shuffle, 1–2 cowbell hits per bar.
- **96–120:** 2–4 bars, 8th hats, syncopated kicks (templates in code).
- **121–190:** Treat as **fast tag only** — programming uses `effective_groove_bpm()` (~half time) so kicks/snares/hats stay Memphis-sparse.

## Instrument roles

1. **Kick 808** — anchor on 1; syncopated & and 3; optional distorted layer quiet underneath.
2. **Snare / clap** — backbeat on 2 and 4 (16th steps 4 & 12); clap slightly late (~6 ms).
3. **Hats** — 8th-note grid max; swing ~55–62%; never exceed `max_hats_per_bar()` at high BPM.
4. **Cowbell** — 1–2 hits per bar, offbeats; not melodic runs.
5. **Rim** — bar 2 fill only, occasional.

## Making generation “very good”

### In this repo (implemented)

- **`phonk_groove.py`** — seeded templates, halftime density cap, swing, velocity jitter.
- **`phonk_loop_render.py`** — micro-delays, kick ducking, one-shot cache per loop.
- **`phonk_loop_factory.py`** — batch unique loops + optional Lofi-12 12 kHz export.
- **`PHONK_PROGRAMMING_PLAYBOOK.md`** — this file; add to RAG corpus for prompt assist.

### Recommended next steps (quality ladder)

1. **Reference loops** — Drop 10–20 labeled WAVs (BPM, bars, “Memphis” vs “drift”) under `disklordz/lofi12-phonk-factory/references/`; script compares hit density and RMS envelope vs generated loops.
2. **RAG** — Chunk this playbook + `disklordz/rag` snippets so Disklordz prompts default to Memphis language.
3. **Human-in-the-loop** — Rate loops 1–5; store `seed` + template id in manifest for fine-tuning weights.
4. **Sample replacement** — Swap procedural one-shots for short 909/808 recordings (still mono, 24 kHz for Lofi).
5. **SysEx** — Send finished loops to Lofi-12 sample RAM for chop / sound-lock.

### Prompt tips for users

- Good: `1990s memphis phonk dirty 808 cowbell 82 bpm`, `slow screw memphis snare`, `drift phonk 148 half time`
- Weak: `fast trap roll hi hat`, `double time`, `EDM phonk`

## CLI

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --prompt "1990s memphis phonk dirty 808 cowbell 86 bpm" \
  --batch 8 \
  --out ~/Music/Lofi12/PhonkLoops

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --bpm 155 --bars 2 --batch 4 --lofi12 \
  --out ~/Music/Lofi12/PhonkLoops/drift
```
