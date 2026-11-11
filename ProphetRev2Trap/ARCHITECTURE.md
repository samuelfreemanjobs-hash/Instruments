# Prophet Rev2 Trap — architecture

8-voice polyphonic **JUCE VST3** inspired by Prophet Rev2 ergonomics, tuned for trap/phonk presets.

## Build & run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target ProphetRev2Trap
```

Artifact: `build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/Prophet Rev2 Trap.vst3`

## Data flow

```text
MIDI → ProphetRev2TrapAudioProcessor → ProphetRev2SynthEngine (8× Rev2Voice)
     → stereo buffer → master gain
```

DSP core: `Source/ProphetRev2Engine.h` (RT-safe: no alloc in `processBlock`).

## Factory programs

1. Phonk Brass  
2. Rev2 Trap Pluck  
3. Glide Pad  

## Cursor workflow

- Index repo; Composer with `#ProphetRev2Trap/Source/ProphetRev2Engine.h`  
- Build: `./scripts/vst/autopilot-compile.sh --target ProphetRev2Trap`  
- Rules: `.cursor/rules/vst-engineer.mdc`, `docs/VST_AI_PLUGIN_MAP.md`  

## Related

Root index: [ARCHITECTURE.md](../ARCHITECTURE.md)  
Multi-agent sync (Cursor ↔ Claude Code): [docs/PROPHET_REV2_TRAP_HANDOFF.md](../docs/PROPHET_REV2_TRAP_HANDOFF.md)
