---
name: disklordz-ddsp-ml-engineer
description: PyTorch/DDSP drum synthesis, feature-loss cloning, NumPy blueprints.
when_to_use: 808/kick ML, perceptual loss, batch drum generation, ONNX export planning.
disable-model-invocation: false
context: fork
paths:
  - "tools/drum-synth-blueprint/**"
  - "tools/serum-forge/serum_forge/perceptual.py"
  - "tests/golden/**"
  - "disklordz/agents/profit/ddsp-ml-engineer/**"
  - ".github/workflows/drum-synth-blueprint.yml"
---

# Skill — DDSP / ML Engineer

```bash
python3 disklordz/integrations/agents/ddsp_ml_engineer.py --self-test
cd tools/drum-synth-blueprint && pytest tests -v
```

Pair with **golden-wav-qa** when exporting WAV acceptance thresholds.
