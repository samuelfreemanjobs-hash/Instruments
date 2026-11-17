# MpcPolysix — architecture (APC Plan)

## Program chain

```text
per voice: VCO (saw) + sub (-1 oct) → SSM2044-style LP → ADSR → mono bus
sum voices
→ EnsembleFx (stereo mod delay bank)
→ PhaserFx (optional 4-stage, bypass when depth=0)
→ out
```

## Components

- `PolysixVoice`, `Ssm2044Filter` (TPT or Chamberlin 2-pole ×2)
- `EnsembleFx`, `PhaserFx` as **post-bus** inserts (single instance, not per-voice)

## FX CPU policy

Ensemble: 2–3 taps max; phaser off by default on Melodee preset Init.