# DiskLordz — The Black Box Collection

## Purpose

Commercial **drum sample kit** franchise: era-specific underground Southern / Memphis machine characters, original synthesis only. Not a VST SKU first — kits ship as WAV packs; shared BLACK BOX drum engine later feeds MPC expansions, desktop, and VST3.

**Positioning:** DISKLORDZ BLACK BOX — THE LOST DRUM MACHINE COLLECTION  
*Four eras. Four machines. One underground legacy. Built from circuits. Raised in the underground.*

## Lineup

| Release | Identity | Era | Target samples | Status |
|---------|----------|-----|---------------:|--------|
| **BLACK BOX 93** | Memphis Archives | Underground Era | 175–225 | Spec'd |
| **BLACK BOX 96** | The Original Machine | Signature Era (flagship) | 200–250 | **Building** |
| **BLACK BOX 98** | Trunk Pressure | Bass Era | 175–225 | Spec'd |
| **BLACK BOX 2000** | Southern Circuitry | Digital Transition | 200–250 | Spec'd |
| **DIRTY CIRCUITS** | Corrupted Machine | Distortion lane | TBD | Spec'd |
| **BB96 Dirty Circuits Edition** | Expanded Flagship | Clean→dirty variants | TBD | Spec'd |

## Engine architecture (planned)

One shared **JUCE/C++ BLACK BOX drum engine** with **four era profiles** (+ Dirty Circuits bus), not four separate synths.

Each profile owns: oscillator tuning, transient envelopes, noise generators, bit/sample resolution, saturation curves, filtering, pitch behavior.

```text
Pattern / recipe → BLACK BOX Engine (era profile) → WAV kit pack
                         ↓
              MPC expansion / Standalone / VST3 (later)
```

Python generators under `scripts/black_box/` prototype the profiles until the C++ engine lands.

## Data flow (kit production)

1. Era profile JSON (`profiles/bb96.json`, …)
2. Voice inventory from `SONIC_DNA.md`
3. Render PCM16 mono @ 44.1 kHz → `kits/black-box-96/masters/`
4. Package zip via `scripts/black_box/package_bb96.py`
5. HITL → `release_ready`

## Related

- `PRODUCT_LINEUP.md` · `SONIC_DNA.md`
- Flagship kit: `../black-box-96/`
- Prior Memphis R&D: `../converter-night/` (research / CNV codepath → maps into BB96 DNA)
