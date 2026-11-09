# DSP knowledge base (index)

Short, testable articles for the **DSP Research** agent. Each article should cite a contract test in `vst/factory-lib/tests/` when implementation exists.

| Topic | Article | Contract test |
|-------|---------|---------------|
| Anti-aliasing | [anti-aliasing.md](anti-aliasing.md) | (planned) `test_oscillator_bandlimit` |
| Oversampling | [oversampling.md](oversampling.md) | (planned) |
| Denormals | [denormals.md](denormals.md) | (planned) `test_denormal_sweep` |
| Parameter smoothing | [parameter-smoothing.md](parameter-smoothing.md) | (planned) |
| Voice allocation | [voice-allocation.md](voice-allocation.md) | (planned) |
| Filters / stability | [filter-stability.md](filter-stability.md) | (planned) `test_biquad_step` |

Repo normative spec: [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md).

Shared code: [shared/dsp/](../../../shared/dsp/), [vst/factory-lib/](../../../vst/factory-lib/).
