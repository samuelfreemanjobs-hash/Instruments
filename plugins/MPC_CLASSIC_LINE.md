# MPC Sample — classic polysynth line (APC)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcJuno106](MpcJuno106/status.json) | Roland Juno-106 / 60 | Plan complete | `/apc-design MpcJuno106` |
| [MpcProphet5](MpcProphet5/status.json) | Sequential Prophet-5 | Plan complete | `/apc-design MpcProphet5` |
| [MpcObXa](MpcObXa/status.json) | Oberheim OB-Xa / SEM | Plan complete | `/apc-design MpcObXa` |
| [MpcPolysix](MpcPolysix/status.json) | Korg Polysix | Plan complete | `/apc-design MpcPolysix` |

### Monophonic (ultra-low CPU)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcMinimoog](MpcMinimoog/status.json) | Minimoog Model D | Plan complete | `/apc-design MpcMinimoog` |

**Platform:** Linux **x86_64** (DAW + pluginval) and **aarch64** VST3 for **Melodee** (`docs/MPC_SAMPLE_VST3.md`). Default **headless** (`hasEditor() == false`). Reference precursor: `vst/MpcAnalogSynth` (Prophet-lite sketch).

**Stop after Plan** — run Design per SKU, do not impl all four in one turn.