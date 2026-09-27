# Lofi-12 phonk drum factory

Turn the **SONICWARE LIVEN Lofi-12** into a portable **phonk drum machine**: 16-slot banks of 808s, Memphis snares, cowbells, and FX — pre-normalized for **24 kHz mono** sampling.

## Quick start

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --prompt "dirty memphis phonk 808 cowbell 140" \
  --out /tmp/lofi12_phonk_bank_a

# Four kit variations at once
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --prompt "drift phonk distorted cowbell 155" \
  --batch 4 \
  --out ~/Music/Lofi12/PhonkFactory
```

Output:

- `01_kick_808.wav` … `16_fx_riser.wav` — load to bank A slots 1–16
- `load_manifest.json` — provenance + durations
- `PATTERN_GUIDE.md` — sound-lock beat template

## Disklordz kits

Point at an extracted SaaS ZIP folder (manifest + WAVs). Mapped one-shots replace core slots; cowbells/FX stay factory-synthesized.

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --from-disklordz ~/Music/Disklordz/Inbox/my-kit \
  --out ~/Music/Lofi12/PhonkFactory/bank_a
```

## Drum machine sound engine

Loops and one-shot banks use **`phonk_machine_engine.py`**: TR-808, TR-909, Boss DR-660, Alesis SR-16, Roland R-8 MKII, **DJ Screw tape**, and **`mr_tape`** (lo-fi tape pack *style* — good starting point for Splice-like warmth). See [docs/DRUM_MACHINE_ENGINES.md](docs/DRUM_MACHINE_ENGINES.md).

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --engine mr_tape --prompt "memphis phonk splice dj paul 84" --out ~/Music/Lofi12/PhonkLoops
```

## Memphis phonk **loops** (60–190 BPM)

Unique **1990s Memphis–style** loops: half-time feel when BPM is high, 8th-note hats with shuffle, no trap hat rolls.

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --prompt "1990s memphis phonk dirty 808 cowbell 86 bpm" \
  --batch 8 \
  --out ~/Music/Lofi12/PhonkLoops

# Drift tempo (sparse programming at 155 BPM)
python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --bpm 155 --bars 2 --batch 4 --lofi12 \
  --out ~/Music/Lofi12/PhonkLoops/drift
```

Programming rules for agents: [docs/PHONK_PROGRAMMING_PLAYBOOK.md](docs/PHONK_PROGRAMMING_PLAYBOOK.md).

## Vocal textures (phonk)

Synthetic Memphis-style chops/loops (formant synth — **not** artist voice clones) plus optional processing of **your** acapella:

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_vocal_factory.py \
  --prompt "memphis screw dark vocal" --chops-only --batch 4 --out ~/Music/Lofi12/PhonkVocals

python3 disklordz/lofi12-phonk-factory/scripts/phonk_loop_factory.py \
  --prompt "dj paul phonk 84" --with-vocals --out ~/Music/Lofi12/PhonkLoops
```

## Step sequencer (computer → Lofi-12 MIDI)

Browser **4×16** grid with Web MIDI + optional Python player:

```bash
python3 disklordz/lofi12-phonk-factory/sequencer/serve.py
# http://127.0.0.1:8765 — pick MIDI OUT, connect to Lofi-12 MIDI IN

python3 disklordz/lofi12-phonk-factory/sequencer/scripts/pattern_from_groove.py \
  --prompt "dj paul memphis 84" --out ~/Music/Lofi12/pattern.json
```

See [sequencer/ARCHITECTURE.md](sequencer/ARCHITECTURE.md).

## Tests

```bash
python3 -m unittest discover -s disklordz/lofi12-phonk-factory/tests -q
```

## Architecture

See [ARCHITECTURE.md](ARCHITECTURE.md).
