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

## Tests

```bash
python3 -m unittest discover -s disklordz/lofi12-phonk-factory/tests -q
```

## Architecture

See [ARCHITECTURE.md](ARCHITECTURE.md).
