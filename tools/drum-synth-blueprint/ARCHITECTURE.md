# Drum synth blueprint (NumPy)

## Purpose

Reference **vectorized** Trap kick and 808 curves for:

- **DDSP / ML Engineer** training targets and feature-loss baselines
- **TrapForge / Memphis** parity checks
- **Serum Forge** text-to-preset prompt validation (pitch envelope semantics)

## Build & run

```bash
cd tools/drum-synth-blueprint
pip install -r requirements.txt
pip install -e .
pytest tests -v
python -m drum_synth_blueprint
```

## Data flow

```text
parameters → render_trap_kick / render_trap_808 (NumPy)
          → normalize_peak → WAV export (consumer scripts) or torch tensors (DDSP)
```

## Related

- [tools/serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md](../serum-forge/docs/SERUM_TEXT_TO_PRESET_PROMPTS.md)
- [docs/JUCE_APC_AGENT_BOOTSTRAP.md](../../docs/JUCE_APC_AGENT_BOOTSTRAP.md)
