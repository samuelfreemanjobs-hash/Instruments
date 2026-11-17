# Shared Juno DSP (MpcJuno106 + Nebula6)

Extract target for Roland-style **DCO + 24 dB LP + BBD chorus** used by:

- `vst/MpcJuno106/` (Melodee headless, 6 voices)
- `Nebula6/` (desktop, remarkable chorus modes — future)

**Current source of truth:** `vst/MpcJuno106/Source/JunoVoice.h` + chorus in `PluginProcessor.cpp`.

**Extract plan (impl):**

| Module | File |
|--------|------|
| DCO + voice | `JunoVoice.h` → `shared/dsp/juno/JunoVoice.h` |
| LPF | from voice → `JunoLpf24.h` |
| Chorus | `JunoChorus.h` + `NebulaChorus.h` in Nebula6 |

Nebula6 adds **Frozen/Drift** in `vst/Nebula6` or repo-root `Nebula6/Source/Dsp/NebulaChorus.h` per `plugins/Nebula6/.ideas/architecture.md`.