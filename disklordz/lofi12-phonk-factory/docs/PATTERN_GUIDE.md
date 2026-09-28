# Phonk drum factory — Lofi-12 load & pattern guide

## Load samples (no SysEx encoder yet)

1. Set Lofi-12 sample rate to **24 kHz** (or 12 kHz if you want longer tails).
2. Select **bank A**, slot **01** … **16** in order; filenames in this pack match slot numbers.
3. For each WAV: **LINE IN** from your interface (or resample internal sampling):
   - Play the file from your DAW/phone at comfortable level.
   - Use **auto-start** sampling or manual record; trim start/end on the box if needed.
   - Enable **12-bit sampler mode** on key slots for extra grit (kicks, snares).
4. Optional: export the filled bank back to PC via MIDI SysEx for backup (manual § export bank).

## Track 1 — sound-lock phonk beat (4/4, 130–160 BPM)

Use **one track** with **sound lock** so each step triggers a different slot:

| Steps (1–16) | Slot | Role |
|--------------|------|------|
| 1, 5, 9, 13 | 01 kick_808 | Four-on-the-floor / half-time kick |
| 3, 7, 11, 15 | 02 kick_dist | Distorted layer (optional) |
| 5, 13 | 04 snare_memphis | Backbeat |
| 2, 6, 10, 14 | 07 hat_closed | Offbeat hats |
| 4, 12 | 08 hat_open | Accents |
| 8, 16 | 10 cowbell_low | Phonk bell hits |

Program with **sound lock** per step (see Lofi-12 manual). Turn **laid-back** slightly up for drift; **dice** 75–90% for live variation.

## Tracks 2–4

| Track | Use |
|-------|-----|
| **2** | Cowbell melody (slots 10–11), pitch via keyboard |
| **3** | Sub 808 (slot 03) or tom fills (12–13) |
| **4** | FX (15–16) or resampled loop from track 1 |

## Performance macros (MIDI CC)

If you sequence from a DAW, map filter cutoff (CC 38) and reverb send (CC 36) for drops. Filter sweeps on snare bus = classic phonk tension.

## Regenerate

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --prompt "dirty memphis 808 cowbell 150" \
  --batch 4 \
  --out ~/Music/Lofi12/PhonkFactory
```

Each batch folder is a full alternate bank — sample one, keep the rest as SysEx/WAV archive.
