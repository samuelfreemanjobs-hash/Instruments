# Memphis Phonk Factory

JUCE **Standalone + VST3** app that procedurally generates a Memphis phonk MPC kit (20 one-shots + 3 loop WAVs). DSP matches `tools/memphis_phonk_generator.py`.

## Build

```bash
./tools/build-vst.sh MemphisPhonkFactory
```

Artifacts: `vst/MemphisPhonkFactory/build/MemphisPhonkFactory_artefacts/Release/`

## Use

- Adjust loop BPM, swing, tape drive, hiss, and variation (1–4).
- **Loop tempo:** rotary **70–170 BPM** (fine adjust around 120), **+/−** nudge; preview and export use this value (`Memphis_Loop_<bpm>BPM.wav`).
- Optional **Also export 112 / 118 / 124 loops** for the classic triple pack.
- **Pattern editor:** 16-step kick / snare / open-hat grid (Memphis & Trap sparse presets).
- Toggle **Loop preview** to hear the current pattern at the selected BPM.
- **Export kit** → `Memphis_Phonk_MPC_Kit/` (23 WAVs).
- **Batch export** → `Memphis_Phonk_Batch_<timestamp>/Kit_001…` (batch count param × 23 WAVs each, unique grit per kit).

## Tests

```bash
cd vst/MemphisPhonkFactory/build && ctest -R MemphisPhonkEngine -V
```
