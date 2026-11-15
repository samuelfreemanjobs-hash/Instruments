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
render_memphis_vault.py → kits/memphis-vault-vol-1/ (catalog one-shots + ZIP)
```

## Key modules

| Path | Role |
|------|------|
| `scripts/generate_kit.py` | Primary kit builder |
| `scripts/generate_stub_kits.py` | Stub / smoke kits |
| `scripts/render_memphis_vault.py` | First catalog kit: Memphis Vault Vol. 1 → `kits/memphis-vault-vol-1/` |
| `scripts/kit_qa.py` | Technical gate: clip, silence, DC, edge clicks, exact and near duplicates |

## Commercial kit factory

Next product path: recipe → original WAVs → `kit_qa.py` → human audition → ZIP with approved art. Memphis Vault Vol. 1 is pack 001. One-click 100-sound packs, generated covers, and store draft publish are not built. Final creative approval stays human.
| [README.md](README.md) | Operator notes |

## Related docs

- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md)  
- [docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)
