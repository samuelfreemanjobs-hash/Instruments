# BLACK BOX 96 — The Original Machine

| Field | Value |
|-------|--------|
| **Store title** | **BLACK BOX 96** |
| **Identity** | The Original Machine |
| **Era** | Signature Era |
| **Collection** | DiskLordz Black Box Collection |
| **SKU** | BB96 |
| **Category** | Drum sample kit (oneshots + loops + process banks) |
| **Promise** | Signature vintage drum-machine sound — thick 808s, aggressive snares, metallic cowbells, crunchy hats, analog toms, dry claps — plus pitched / cassette / RAW·TAPE·LOFI parallel banks |
| **Target hits** | 200–250 core (locked **480**) + FX banks |
| **Format** | 44.1 kHz · PCM16 · mono |
| **Street price** | HITL ($24–$34 band) |
| **release_ready** | **false** until HITL |

## Positioning

Part of **DISKLORDZ BLACK BOX — THE LOST DRUM MACHINE COLLECTION**.  
Flagship of four eras. Original synthesis — not a ROM dump.

## Zip layout (follow the instructions)

Commercial zip matches Converter Night kit instructions:

1. `01_CORE_ONESHOTS/` — RAW cores  
2. `02_BITCRUSHED_AND_PITCHED/` — rate-drop / pitch / alias  
3. `03_CASSETTE_AND_TAPE/` — cassette desk FX  
4. `04_PARALLEL_PROCESS_BANKS/` — RAW / TAPE / LOFI / CASSETTE siblings  
5. `05_LOOPS/` + `06_MATCHED_MINI_KITS/`  
6. `_DOCS/Hardware_Processing_Guide.md` — what to do with each folder  

## Rebuild

```bash
cd claude/drumsmith
python3 scripts/black_box/generate_bb96.py
python3 scripts/black_box/package_bb96.py --zip /opt/cursor/artifacts/Black_Box_96.zip
```
