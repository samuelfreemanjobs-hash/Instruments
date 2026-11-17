# DiskLordz Drum Forge 4.0

## Purpose

Offline, **synthesis-only** drum sample manufacturing engine for trap / Memphis / phonk /
southern underground kits. Produces curated WAV packs with QC, manifests, and a searchable
HTML catalog. This is the software product; it is not the Disklordz website SaaS.

## Build & run (Python — current release)

```powershell
cd DiskLordz_Drum_Forge
python -m pip install -r requirements.txt
python test_forge.py
python drum_forge_v4.py --genres memphis drift_phonk --variations 4 --notes C1 D1 F1 G1 --out DiskLordz_Kits
```

## Build & run (C++ M1 offline — in progress)

```powershell
cmake -S native -B native/build -G "Visual Studio 17 2022" -A x64
cmake --build native/build --config Release --target drum_forge_offline
.\native\build\Release\drum_forge_offline.exe --preset memphis --family 808 --midi 24 --out out.wav
```

## Data flow

```text
CLI (drum_forge_v4.py)
  → genre preset DNA (forge_core_v2.PRESETS)
  → mutate (drum_forge_pro) × variations
  → family generator (808 / kick / snare / …)
  → trim → master (4× true-peak estimate)
  → QC + similarity gate
  → WAV + manifest.csv/json + catalog.html
```

## Threading / realtime

Python factory is offline / batch only. Future JUCE plugin audio thread must not allocate,
touch disk, or take locks (see `docs/JUCE_NEXT_MILESTONE.md` when present, or Desktop
`JUCE_NEXT_MILESTONE.md`).

## Key modules

| Path | Role |
|------|------|
| `forge_core_v2.py` | Presets, voices, true-peak, reverb, generators |
| `drum_forge_pro.py` | Mutation + QC descriptors |
| `drum_forge_v4.py` | Curated pack CLI + catalog |
| `*_lab.py` | Genre-specialized labs (Detroit, Rage, Hyper DNA, …) |
| `native/` | C++20 offline renderer (M1: 808 + kick) |

## Extension points

- Add genre DNA in `PRESETS` / lab scripts; keep generators synthesis-only.
- C++ `native/dsp/` is the shared core for future Standalone + VST3 (APC Dream before `vst/DrumForge`).
- Do not wire SaaS / website into this package.

## Related docs

- Desktop README: Drum Forge 4 product notes
- JUCE roadmap: M1–M6 standalone + VST3 after APC Dream
