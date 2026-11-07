# VST AI plugin map (active core files)

**Use in Composer:** `#VST_AI_PLUGIN_MAP.md` plus the product block you are changing.

Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md). Engineer setup: [CURSOR_VST_ENGINEER.md](CURSOR_VST_ENGINEER.md).

---

## JD Upgraded (main synth)

| File | Role |
|------|------|
| `Source/PluginProcessor.cpp` | APVTS `createParameterLayout()` ~L88; **`processBlock`** ~L744; voice render entry |
| `Source/PluginProcessor.h` | Processor API, cached parameters |
| `Source/PluginEditor.cpp` / `.h` | UI layouts, APVTS attachments |
| `Source/DSP/VoicePool.cpp` | 32-voice mix; no heap on audio path |
| `Source/DSP/Tone.cpp` | Per-tone PCM + envelopes |
| `Source/Assets/FactoryPatchLibrary.cpp` | 128 factory programs |
| `Source/Preset/JdPatchSysexMapper.cpp` | SysEx → APVTS |

**QA:** `python3 vst-testing-ops/run_business.py --profile ci` · golden: `tests/golden/verify_golden.sh`

Deep dive: [docs/ARCHITECTURE.md](ARCHITECTURE.md) · UI: [Source/UI/ARCHITECTURE.md](../Source/UI/ARCHITECTURE.md)

---

## WAVE-9090

| File | Role |
|------|------|
| `Wave9090/Source/PluginProcessor.cpp` | `createParameterLayout()` ~L18; **`processBlock`** ~L198 |
| `Wave9090/Source/PluginEditor.cpp` | Wavetable UI |
| `Wave9090/Source/Presets/FactoryPresets.cpp` | Factory state |

**QA:** `ctest --test-dir build -R Wave9090` (also in CI pipeline)

Deep dive: [Wave9090/ARCHITECTURE.md](../Wave9090/ARCHITECTURE.md)

---

## SP-1200 Drumulator

| File | Role |
|------|------|
| `SP1200/Source/PluginProcessor.cpp` | **`processBlock`** ~L53; MIDI → engine |
| `SP1200/Source/Engine/SamplerEngine.cpp` | Sample playback + voices |
| `SP1200/Source/Engine/PatternSequencer.cpp` | Pattern timing |
| `SP1200/Source/PluginEditor.cpp` | Standalone/VST UI |

**QA:** `ctest --test-dir build -R SP1200`

Deep dive: [SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md)

---

## Scaffold / learning

| File | Role |
|------|------|
| `MyFirstPlugin/PluginProcessor.cpp` | Minimal APVTS + `processBlock` template |

---

## When adding a feature (checklist)

1. **Parameter** → `createParameterLayout` + `ParameterIds` + editor attachment + `applyPatchesFromParameters` (JD) or product equivalent  
2. **DSP** → tone/voice/engine file from table above — not unrelated UI  
3. **UI** → `PluginEditor` only; no DSP in paint/layout callbacks  
4. **Build** → `./scripts/vst/autopilot-compile.sh` then CI profile if DSP changed  

Line numbers drift — search `processBlock` / `createParameterLayout` if anchors are stale.
