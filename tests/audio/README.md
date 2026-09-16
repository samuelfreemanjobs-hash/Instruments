# Golden Ear Lab

Objective audio QA + human-approved references.

```text
tests/audio/
  golden/          # Human-approved reference renders
  references/      # Input fixtures (sine, impulse, drum loops)
  fixtures/        # Small WAV for CI
  regression/      # Plugin output vs golden (FFT, peak, dynamics)
```

## Regression flow

```text
Plugin build → render test signal → compare to golden/
  → FFT / peak / dynamics thresholds → PASS | REVIEW | FAIL
```

See `disklordz/docs/QA_LAB.md`. DSP contract requires updates here on behavior change.
