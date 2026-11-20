# Sample source & license log — Lofi-12 / DiskLordz Drum Factory (Phase 1)

| Asset class | Source | License / resale |
|-------------|--------|------------------|
| One-shots, loops, texture beds | **Procedural synthesis** (`scripts/phonk_synth.py`, `drum_texture_synth.py`, `phonk_machine_engine.py`, `atl_trap_voices.py`) | Original algorithmic output; commercial kit license in `LICENSE.txt` |
| MIDI patterns | **Generated** (`drum_pattern_intel.py`, `smf_export.py`) | Same as kit audio |
| Vocals (optional factory) | **Formant synth** (`phonk_vocal_synth.py`); user WAV only via `--acapella` (not bundled) | Synth = original; user acapellas are operator responsibility |
| Reference GUI PNGs | In-repo design exports (`references/gui/`) | DiskLordz internal art |

## Explicitly excluded (never ingested)

- Splice, Cymatics, SampleFocus, YouTube rips, or other third-party sample libraries
- Master stems from released tracks

## CI attestation

`kit_output_qa.py` scans generated demo kits for zero-byte files, peak ceiling, and forbidden source markers in `manifest.json`. Unit tests live under `tests/test_kit_output_qa.py`.