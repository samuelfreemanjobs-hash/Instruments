# Lofi-12 Phonk drum factory

## Purpose

Batch-generate **Memphis / drift phonk** one-shots sized for the **SONICWARE LIVEN Lofi-12** (mono PCM, 24 kHz ≈ 2 s or 12 kHz ≈ 4 s per slot). Output is a **named bank folder** you sample from line-in or archive for SysEx round-trips.

## Build & run

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --prompt "dirty memphis 808 cowbell 140" \
  --out ~/Music/Lofi12/PhonkFactory/bank_a

python3 -m unittest discover -s disklordz/lofi12-phonk-factory/tests -q
```

Optional: convert an existing Disklordz kit folder (manifest + WAVs):

```bash
python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py \
  --from-disklordz /path/to/kit \
  --out ~/Music/Lofi12/PhonkFactory/bank_a
```

## Data flow

```text
prompt + seed → phonk_synth (808, cowbell, hats, memphis snare, FX)
  → lofi12_prepare (trim, soft clip, resample 24 kHz mono)
  → bank_a/01_kick_808.wav … 16_slot_fx.wav + load_manifest.json + PATTERN_GUIDE.md
  → (user) LINE IN sample or future SysEx send
```

## Threading / realtime

N/A — offline Python CLI only.

## Key modules

| Path | Role |
|------|------|
| `scripts/phonk_synth.py` | Procedural phonk drum renderers |
| `scripts/lofi12_prepare.py` | Lofi-12 sample rate, length, level |
| `scripts/bank_layout.py` | 16-slot phonk bank naming |
| `scripts/phonk_factory.py` | CLI: generate, batch, import Disklordz |
| `docs/PATTERN_GUIDE.md` | Sound-lock drum pattern on hardware |

## Extension points

- WAV → Lofi-12 `.syx` encoder + USB-MIDI send (see repo `docs/SYSEX.md` patterns)
- Hook `disklordz/daw-inbox` post-extract to run `--from-disklordz`
- Preset packs aligned with `midnight-circuit` / phonk prompts in SaaS RAG

## Related docs

- [README.md](README.md)
- [disklordz/sound-factory/README.md](../sound-factory/README.md)
- Sonicware Lofi-12 manual (sample export/import via MIDI)
