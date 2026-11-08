# Disklordz sound factory

## Purpose

Offline/parametric **kit generation** scripts that produce WAV packs consumed by `disklordz/website` (`POST /api/generate`, factory batch routes).

## Build & run

```bash
python3 disklordz/sound-factory/scripts/generate_kit.py --help
python3 disklordz/sound-factory/scripts/generate_stub_kits.py
```

## Data flow

```text
GenerationSpec + preset → generate_kit.py → WAV files + manifest JSON → website sample URLs / ZIP
```

## Key modules

| Path | Role |
|------|------|
| `scripts/generate_kit.py` | Primary kit builder |
| `scripts/generate_stub_kits.py` | Stub / smoke kits |
| [README.md](README.md) | Operator notes |

## Related docs

- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md)  
- [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)
