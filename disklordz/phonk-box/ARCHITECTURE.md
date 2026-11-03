# DiskLordz Phonk Box (808 Lab hardware leg)

## Purpose

Optional **DIY USB MIDI controller** + **Windows PC sound engine** for phonk/808 experiments.

**Default controller: Arturia SparkLE** (standalone + USB MIDI) — see [SPARKLE-MIDI-AND-CONTROLLER.md](../../docs/disklordz/spark-le/SPARKLE-MIDI-AND-CONTROLLER.md). **Do not build DIY hardware** until SparkLE + PC engine limits are clear.

**Users:** You · same kits as SparkLE / 808 Lab hybrid.

## Build & run (Phase 0 — engine only)

```bash
cd disklordz/phonk-box/engine
python3 -m venv .venv && source .venv/bin/activate   # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python phonk_engine.py --self-test
python phonk_engine.py --kit /path/to/OneShots --midi "Phonk Box"
```

Firmware: see [firmware/README.md](firmware/README.md) (Teensy / RP2040 USB MIDI).

## Data flow

```text
USB controller (pads, encoders, transport)
  → USB MIDI (see protocol/MIDI-MAP.md)
  → phonk_engine.py (Windows) — sample playback + 16-step sequencer + lane params
  → WAV from sound-factory / website factory / SparkLE exports (8-pad law)
  → optional: manifest spark-le-manifest.json (pad file paths)
```

```text
Disklordz lane-registry (phonk lanes) → kit generation → phonk-box kit folder → controller session
SparkLE hybrid: same 8 one-shots can load in SparkLE OR phonk-box engine
```

## Threading / realtime

- **Controller firmware:** no audio; debounce pads; send MIDI only; avoid blocking USB.
- **PC engine:** audio callback must not allocate; preload 8 pads into RAM; pattern queue on main thread.

## Key modules

| Path | Role |
|------|------|
| [protocol/MIDI-MAP.md](protocol/MIDI-MAP.md) | Notes, CCs, modes (play / seq / param) |
| [engine/phonk_engine.py](engine/phonk_engine.py) | Phase 0 MIDI-in sampler + step sequencer |
| [firmware/README.md](firmware/README.md) | BOM, Teensy/RP2040 USB MIDI device |
| [docs/BOM-100USD.md](docs/BOM-100USD.md) | Parts list under ~$100 |
| [docs/ENCLOSURE-AND-CONNECTORS.md](docs/ENCLOSURE-AND-CONNECTORS.md) | Item 5: box, USB, JST ($8–15, when stable) |
| [../website/src/lib/spark-le/lane-registry.ts](../website/src/lib/spark-le/lane-registry.ts) | Phonk lane presets (`midnight-circuit`, OG phonk, etc.) |
| [../../SP1200/ARCHITECTURE.md](../../SP1200/ARCHITECTURE.md) | Future Phase 2: native JUCE engine reuse (sequencer, MIDI clock) |

## Extension points

1. **Kits:** Drop folder with `kick.wav` … `cowbell_fx.wav` or SparkLE manifest paths.
2. **Lanes:** `--preset lane-og-phonk` loads default CC map + BPM hint from registry (future).
3. **Firmware:** Add step buttons (notes 48–63) without changing engine API.
4. **Phase 2:** Replace Python engine with `PhonkBox_Standalone` JUCE app sharing SP1200 sequencer code.

## Phases

| Phase | Deliverable |
|-------|-------------|
| **0** | Python engine + MIDI map + self-test (this tree) |
| **1** | Teensy/RP2040 8-pad USB MIDI + transport + 1 encoder |
| **2** | Windows standalone (JUCE) + disk streaming + MIDI learn |
| **3** | Sync with 808 Lab keepers + `verify:spark` kit import wizard |

## Related docs

- [docs/disklordz/spark-le/808-LAB-HYBRID.md](../../docs/disklordz/spark-le/808-LAB-HYBRID.md)  
- [docs/disklordz/spark-le/LANES.md](../../docs/disklordz/spark-le/LANES.md) (phonk family)  
- [sound-factory/ARCHITECTURE.md](../sound-factory/ARCHITECTURE.md)
