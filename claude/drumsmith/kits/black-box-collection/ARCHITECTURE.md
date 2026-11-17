# DiskLordz — The Black Box Collection

## Purpose

Commercial **drum sample kit** franchise: era-specific underground Southern / Memphis machine characters, original synthesis only. Kits ship as WAV packs with CNV-style process banks; shared BLACK BOX drum engine later feeds MPC expansions, desktop, and VST3.

**Positioning:** DISKLORDZ BLACK BOX — THE LOST DRUM MACHINE COLLECTION  
*Four eras. Four machines. One underground legacy. Built from circuits. Raised in the underground.*

## Lineup

| Release | Identity | Era | Target | Status |
|---------|----------|-----|-------:|--------|
| **BLACK BOX 93** | Memphis Archives | Underground | 175–225 | Generate+package |
| **BLACK BOX 96** | The Original Machine | Signature ★ | 200–250 | Generate+package |
| **BLACK BOX 98** | Trunk Pressure | Bass | 175–225 | Generate+package |
| **BLACK BOX 2000** | Southern Circuitry | Digital | 200–250 | Generate+package |
| **DIRTY CIRCUITS** | Corrupted Machine | Bus | 120–200 | Package from BB96 |
| **BB96 Dirty Circuits Edition** | Expanded Flagship | Clean→dirty | — | Package from BB96 |

## Shared engine

**Python (shipping kits now):** `scripts/black_box/generate_era.py` reads era profile JSON and renders distinct personalities via character flags (`hollow_tape` / `signature` / `trunk_bass` / `digital_south`).

**Post FX (every zip):** `process_fx.py` + `package_era.py` → pitched / cassette / RAW·TAPE·LOFI·CASSETTE / mini-kits / `Hardware_Processing_Guide.md`.

**C++ (planned JUCE):** `native/include/BlackBoxEraProfile.h` mirrors profile knobs for a future shared drum engine — not four separate synths.

```text
profiles/*.json → generate_era.py → masters/
                      ↓
              package_era.py (+ process_fx)
                      ↓
         zip with cores + FX banks + _DOCS guide
                      ↓
         (later) JUCE engine / MPC / VST3
```

## Build all

```bash
cd claude/drumsmith
python3 scripts/black_box/build_all_collection.py
python3 -m unittest tests.test_black_box_collection tests.test_black_box_96 -v
```

## Related

- `PRODUCT_LINEUP.md` · `SONIC_DNA.md` · `profiles/`
- Kits: `../black-box-{93,96,98,2000,dirty-circuits}/`
- Prior R&D: `../converter-night/`
