# MPC Sample — classic polysynth line (APC)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcJuno106](MpcJuno106/status.json) | Roland Juno-106 / 60 | Plan complete | `/apc-design MpcJuno106` |
| [MpcProphet5](MpcProphet5/status.json) | Sequential Prophet-5 | Plan complete | `/apc-design MpcProphet5` |
| [MpcObXa](MpcObXa/status.json) | Oberheim OB-Xa / SEM | Plan complete | `/apc-design MpcObXa` |
| [MpcPolysix](MpcPolysix/status.json) | Korg Polysix | Plan complete | `/apc-design MpcPolysix` |
| [MpcCs80](MpcCs80/status.json) | Yamaha CS-80 (dual-layer, poly AT) | Plan complete | `/apc-design MpcCs80` |

### Monophonic (ultra-low CPU)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcMinimoog](MpcMinimoog/status.json) | Minimoog Model D | Plan complete | `/apc-design MpcMinimoog` |
| [MpcVoyager](MpcVoyager/status.json) | Voyager-style mono ladder | **Impl** (vst/) | `/apc-test MpcVoyager` |
| [MpcSubPhatty](MpcSubPhatty/status.json) | Sub Phatty multidrive bass | **Impl** (vst/) | `/apc-test MpcSubPhatty` |
| [MpcMessenger](MpcMessenger/status.json) | Messenger wavefold + Res Bass | **Impl** (vst/) | `/apc-test MpcMessenger` |
| [MpcSh101](MpcSh101/status.json) | Roland SH-101 | Plan complete | `/apc-design MpcSh101` |

### Early digital & hybrid (FM)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcDxFm](MpcDxFm/status.json) | Yamaha DX7 / TX81Z | **Impl** (vst/) | `/apc-test MpcDxFm` |

### Sample + synth hybrid (M1 / D-50)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcLaSynth](MpcLaSynth/status.json) | Korg M1 / Roland D-50 | Plan complete | `/apc-design MpcLaSynth` |

### Drum machines (synth, not samples)

| Plugin | Vintage target | Phase | Next |
|--------|----------------|-------|------|
| [MpcTr808909](MpcTr808909/status.json) | Roland TR-808 / TR-909 | Plan complete | `/apc-design MpcTr808909` |

**Platform:** Linux **x86_64** (DAW + pluginval) and **aarch64** VST3 for **Melodee** (`docs/MPC_SAMPLE_VST3.md`). Default **headless** (`hasEditor() == false`). Reference precursor: `vst/MpcAnalogSynth` (Prophet-lite sketch).

**Stop after Plan** — run Design per SKU, do not impl all four in one turn.