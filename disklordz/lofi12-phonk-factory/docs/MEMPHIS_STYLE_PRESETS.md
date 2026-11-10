# Memphis style presets (neutral IDs)

Default aesthetic for the Lofi-12 phonk factory: dirty Memphis 808s, SP-1200-style crunch, and trap-weight subs — **100% synthesized** in Drum Factory Phase 1.

## Lane + engine map

| Label | Lane (`--lane`) | Engine (`--engine`) | Feel |
|-------|-----------------|----------------------|------|
| Hard Dirt 808 | `hard_dirt_808` | `hard_dirt_808` | 10-bit dirt, tape hiss, FM cowbell, clap-heavy |
| Bounce & Cowbell | `bounce_cowbell` | `hard_dirt_808` | Bounce kick grid, extra cowbell |
| Traproom Tape | `traproom_tape` | `traproom_tape` | 12-bit SP-1200 nod, harder click, more sub |
| Tape Lag | `tape_lag` | `mr_tape` | Sparse behind-the-beat tape groove |
| Occult Tape | — | `mr_tape` | Slow 110–120 menacing cassette (factory mode `occult_tape`) |

Legacy CLI ids (`juicy_j`, `dj_paul`, `dj_toomp`) still resolve via aliases but are **deprecated**.

## Factory modes (commercial kits)

Use `phonk_kit_factory.py --mode <id>` — see `scripts/factory_modes.py` for the nine genre modes plus `black_cassette` and `occult_tape`.

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_kit_factory.py \
  --mode occult_tape --kit-size demo --out ~/Music/DiskLordz/OccultDemo
```

## Loop / bank presets

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --preset memphis_insanity_pack --batch 4 --out ~/Music/Lofi12/MemphisPack

python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --preset memphis_underground --out ~/Music/Lofi12/bank_a
```

## Sequencer

```bash
python3 disklordz/lofi12-phonk-factory/sequencer/serve.py
```

Apply a preset → load groove → generate backing loop. Prompt examples (no artist names required):

- `dirty memphis bounce cowbell 118 bpm`
- `traproom tape 808 78 bpm`
- `occult tape dragging menacing 115 bpm`

## BPM guide (Memphis factory)

- **Bounce / dirt:** 110–130 BPM (tape variants 110–125)
- **Traproom:** 78 default half-feel, or factory mode BPM when using kits

SP-1200 character is **emulated** in DSP (bit-crush, sample-hold, tape bus) — not ROM samples.
