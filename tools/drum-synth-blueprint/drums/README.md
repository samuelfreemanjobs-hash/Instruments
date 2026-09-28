# Personal drum samples for DDSP training

Place **flat** `.wav` files here (kicks, 808s, percussion). Subfolders are not scanned.

```bash
cd tools/drum-synth-blueprint
python scripts/ddsp_training_loop.py --folder ./drums --epochs 20 --batch 4
```

Output: `artifacts/ddsp_808_encoder.onnx` (mel → four synth parameters).
