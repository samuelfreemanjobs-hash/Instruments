# Kit spec — BLACK BOX 96 (BB96)

**Store title:** BLACK BOX 96 — The Original Machine  
**Collection:** DiskLordz Black Box · see `../black-box-collection/`  
**Ship status:** DRAFT — `release_ready=false`

## Promise

The Signature Era flagship: vintage Memphis drums, dirty hardware character, hard-hitting 90s Southern rhythms — shipped with pitched variants, cassette FX, and parallel process banks (same instructions family as Converter Night commercial zips).

## Core hit inventory (240)

| # | Voice | Count | Folder | Prefix |
|---|-------|------:|--------|--------|
| 1 | Thick 808 | 36 | `01_CORE_ONESHOTS/01_808_Subs/` | `BB96_808_` |
| 2 | Punch kick | 24 | `01_CORE_ONESHOTS/02_Kicks/` | `BB96_KICK_` |
| 3 | Aggressive snare | 40 | `01_CORE_ONESHOTS/03_Snares/` | `BB96_SNR_` |
| 4 | Dry clap | 24 | `01_CORE_ONESHOTS/04_Claps/` | `BB96_CLAP_` |
| 5 | Crunchy closed hat | 32 | `01_CORE_ONESHOTS/05_Hats_Closed/` | `BB96_CH_` |
| 6 | Open hat | 16 | `01_CORE_ONESHOTS/06_Hats_Open/` | `BB96_OH_` |
| 7 | Metallic cowbell | 24 | `01_CORE_ONESHOTS/07_Cowbells/` | `BB96_COW_` |
| 8 | Analog tom | 20 | `01_CORE_ONESHOTS/08_Toms/` | `BB96_TOM_` |
| 9 | Perc / rim | 12 | `01_CORE_ONESHOTS/09_Percussion/` | `BB96_PERC_` |
| 10 | 2-bar loops | 12 | `05_LOOPS/` | `BB96_LOOP_` |
| | **Core total** | **240** | | |

Count check: 36+24+40+24+32+16+24+20+12+12 = **240** ✓ (within 200–250)

## Process / FX banks (packaged extras)

| Folder | Content |
|--------|---------|
| `02_BITCRUSHED_AND_PITCHED/` | Rate-dropped 808s (−12/−24 st), pitched snares & cowbells, aliased color shots |
| `03_CASSETTE_AND_TAPE/` | Cassette desk FX (drive → tape LPF → wow/flutter → hiss) |
| `04_PARALLEL_PROCESS_BANKS/` | RAW / TAPE / LOFI / CASSETTE siblings of core drums |
| `06_MATCHED_MINI_KITS/` | 4 mini-kits (oneshots + BPM loop + cassette kick A/B) |

## Grid

| Param | Value |
|-------|--------|
| Master BPM (loops) | 94 |
| Half-time loops | 134 |
| SR / bit / ch | 44.1 kHz · PCM16 · mono |
| Peak ceiling | −0.1 dBFS |

## Storefront tree

```text
Black_Box_96/
  01_CORE_ONESHOTS/
    01_808_Subs/ … 09_Percussion/
  02_BITCRUSHED_AND_PITCHED/
    RateDropped_808s/
    Pitched_Snares/
    Pitched_Cowbells/
    Aliased_One_Shots/
  03_CASSETTE_AND_TAPE/
  04_PARALLEL_PROCESS_BANKS/
    RAW/ TAPE/ LOFI/ CASSETTE/
  05_LOOPS/
  06_MATCHED_MINI_KITS/
  _DOCS/
    Hardware_Processing_Guide.md
    PRODUCT.txt
    …
```

## Commands

```bash
cd claude/drumsmith
python3 scripts/black_box/generate_bb96.py
python3 scripts/black_box/package_bb96.py
```
