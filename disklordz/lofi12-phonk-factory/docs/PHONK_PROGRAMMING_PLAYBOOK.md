# Phonk programming playbook (Lofi-12 factory)

Synthesized loops and banks only — neutral DiskLordz naming.

## Eras (sonic reference, not credits)

| Era | BPM | Traits |
|-----|-----|--------|
| 1990s Memphis | 70–95 | Dirty 808, cowbell bounce, cassette warble |
| Southern trap roots | 75–90 | Traproom tape, hard knock, sparse hats |
| Modern drift phonk | 140–170 | Distorted cowbell, fast hats |

## Factory mode vs lane vs engine

| Kind | Example id | Role |
|------|------------|------|
| Factory mode | `memphis_underground` | Full kit + fusion macros |
| Lane | `bounce_cowbell` | Groove template + tone bias |
| Engine | `hard_dirt_808` | Drum machine voicing |

## Lane quick map

| Lane id | Prompt tags | Default BPM |
|---------|-------------|---------------|
| `bounce_cowbell` | bounce, cowbell grid | 118 |
| `hard_dirt_808` | hard dirt, dirty 808 | 122 |
| `traproom_tape` | traproom, trap room | 118 |
| `sparse_machine_grid` | sparse grid, machine grid | 80 |
| `slow_hat_drag` | slow hat, minimal hat | 74 |
| `deep_knock_memphis` | deep knock, laid back memphis | 76 |
| `ultra_slow_bounce` | ultra slow | 72 |
| `horror_memphis` | horror memphis, dark crisis | 79 |

## CLI examples

```bash
python3 scripts/phonk_loop_factory.py \
  --prompt "bounce cowbell memphis phonk 118" \
  --batch 4 --out ~/Music/Lofi12/PhonkLoops

python3 scripts/phonk_loop_factory.py \
  --preset memphis_insanity_pack \
  --lane traproom_tape \
  --engine traproom_tape \
  --prompt "traproom tape atlanta trap 78" \
  --batch 2 --out ~/Music/Lofi12/PhonkLoops/traproom

python3 scripts/phonk_kit_factory.py \
  --mode memphis_raw_southern --kit-size demo --out ./kits/raw_southern_demo
```

Loop filenames use lane + engine ids, e.g. `traproom_tape_traproom_tape_78bpm_v00.wav`.
