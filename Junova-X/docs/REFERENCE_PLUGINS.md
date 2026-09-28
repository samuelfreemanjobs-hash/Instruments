# Free Juno-class reference plugins (WO-2026-011)

Junova-X uses **lawful** reference work only:

| Method | OK for Junova-X | Notes |
|--------|-----------------|--------|
| **Audio capture + spectral diff** | Yes | Any free/commercial plugin; match MIDI/level; see [QA_AB_JUN6.md](QA_AB_JUN6.md) |
| **Read open-source DSP + reimplement** | Yes | Must not paste GPL/AGPL code; document inspiration in `ARCHITECTURE_DSP.md` |
| **Decompile / crack proprietary VST binaries** | **No** | TAL-U-NO-LX, Roland Cloud, Arturia, etc. — ears + WAV only |

## Open-source references (preferred)

| Plugin | License | Formats | Use in repo |
|--------|---------|---------|-------------|
| [Ultramaster KR-106](https://github.com/kayrockscreenprinting/ultramaster_kr106) | **GPLv3** | VST3, CLAP, LV2, AU, Standalone | Clone for study; `tools/render-midi/render_midi` for reference WAVs; **no code copy** |
| [ATLAS-06](https://github.com/sbadon122/ATLAS-06-Synthesizer) | GPLv3 | VST3, AU | Secondary JUCE signal-flow reference |
| [UB_Poly16](https://github.com/gPTPPs/UB_Poly16) | **AGPLv3** | VST3 (Win) | Juno-106–inspired; study only |

### KR-106 chorus targets (hardware-documented)

Public KR-106 header docs (hardware measurements) — Junova `BbdChorus` aligns to:

| Mode | LFO | Rate | Delay swing (approx.) |
|------|-----|------|------------------------|
| I | Triangle | **0.393 Hz** | **4.66 ms** pp |
| II | Triangle | **0.797 Hz** | ~4 ms pp |
| I+II | Sine | **8 Hz** | shorter vibrato-style swing |

Dry/wet summer ratios (IC6): **kDry 0.863**, **kWet 1.257** (peak-normalized in Junova).

VCF: KR-106 uses TPT ladder + weak tanh (BA662-style). Junova uses JUCE SVF + OTA tanh — converge via golden + optional KR-106 WAV diff.

## Free closed-source (audio-only)

| Plugin | Notes |
|--------|--------|
| [TAL-U-NO-LX](https://tal-software.com/products/tal-u-no-lx) | Juno-60; industry A/B baseline (YouTube shootouts in [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md)) |
| [Tyrell N6](https://u-he.com/products/tyrelln6/) | Free; Juno-*inspired*, not 106-accurate — good for pad/chorus vibe checks |
| Roland / Arturia / Cherry | Paid or subscription — manual WAV export for competitive matrix |

## Workflow in this monorepo

### 1. Clone KR-106 locally (not a submodule — GPLv3)

```bash
./Junova-X/scripts/setup_kr106_reference.sh
```

Default path: `.reference/ultramaster_kr106/` (gitignored).

### 2. Build offline renderer

```bash
cd .reference/ultramaster_kr106/tools/render-midi && make
```

### 3. Match Junova golden scenario

```bash
python3 Junova-X/scripts/make_test_note_mid.py 57 3.0 /tmp/a3.mid
.reference/ultramaster_kr106/tools/render-midi/render_midi /tmp/a3.mid /tmp/kr106-a3.wav 44100

./build/Junova-X/JunovaOfflineRender /tmp/junova-a3.wav --scenario ab03-chorus-i 57 100 3.0 44100
./build/SpectralDiff /tmp/kr106-a3.wav /tmp/junova-a3.wav --max-rms-db -35 --max-spectral-db -12
```

Or use the wrapper:

```bash
./tests/golden/junova/compare_kr106_reference.sh ab03-chorus-i 57 3.0
```

**Fairness:** `render_midi` starts from KR-106 **init** unless the MIDI file contains Juno SysEx. Large spectral gaps vs Junova `GoldenScenarios` are expected until we add SysEx/MIDI fixture files (future WO). Use this script to track **convergence** after DSP edits, not as a merge gate.

### 4. Store optional reference WAVs

Place third-party renders under `tests/golden/junova/reference/` (see README there). **Do not commit** vendor plugin binaries.

## Parity checklist (living)

| Subsystem | Junova-X | KR-106 (read-only target) | iPlug2 (when imported) |
|-----------|----------|---------------------------|-------------------------|
| Chorus LFO | Triangle I/II, sine I+II | Documented Hz + MN3009 model | TBD |
| VCF | SVF + tanh | TPT + BA662 tanh | TBD |
| Voices | 8 + Juno 6 cap | 6/8/10 | TBD |
| SysEx | — | Full 106 IPR/APR | TBD |

Update this table when a reference render closes a gap.

## Related

- [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md)
- [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md)
- [QA_AB_JUN6.md](QA_AB_JUN6.md)
