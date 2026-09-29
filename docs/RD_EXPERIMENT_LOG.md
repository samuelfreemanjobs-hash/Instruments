# Audio R&D experiment log

Structured memory for the **audio-rd** agent. Pair with **ddsp-ml-engineer** (implementation) and **audio-plugin-coder** (JUCE/APC ship).

| ID | Date | Hypothesis | Owner | Status | Evidence |
|----|------|------------|-------|--------|----------|
| RD-808-001 | 2026-03-28 | Mel feature-loss beats waveform MSE for 808 clone | audio-rd | **promoted** | `mel_loss.py` + pytest |
| RD-808-002 | TBD | Encoder ONNX + C++ oscillator vs full-graph ONNX | audio-rd | park | `juce_onnx_pipeline_guide.txt` |
| RD-808-003 | 2026-09-28 | End-to-end torch DDSP + multi-scale spectral loss → `ddsp_808_encoder.onnx` | ddsp-ml-engineer | **promoted** | `torch_ddsp/`, `pytest tests/test_torch_ddsp.py` [executed] |
| RD-PHONK-001 | 2026-09-28 | Memphis kit ML — Phase 0 fingerprint from 5 reference loops | audio-rd | **active** | [memphis_phonk_fingerprint.md](../tools/drum-synth-blueprint/docs/memphis_phonk_fingerprint.md) |

## Template (copy per spike)

```markdown
### RD-XXX — title
- **Hypothesis:**
- **Metrics:**
- **Commands:** [executed]
- **Decision:** promote | kill | park
- **Handoff:** ddsp-ml-engineer | audio-plugin-coder | none
```

## Related

- [DDSP_TRAP_PHONK_AGENT_PROMPTS.md](DDSP_TRAP_PHONK_AGENT_PROMPTS.md)
- [tools/drum-synth-blueprint/ARCHITECTURE.md](../tools/drum-synth-blueprint/ARCHITECTURE.md)
