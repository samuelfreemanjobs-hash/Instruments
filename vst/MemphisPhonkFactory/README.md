# Memphis Phonk Factory

JUCE **Standalone + VST3** app that procedurally generates a Memphis phonk MPC kit (20 one-shots + 3 loop WAVs). DSP matches `tools/memphis_phonk_generator.py`.

## Build

```bash
./tools/build-vst.sh MemphisPhonkFactory
```

Artifacts: `vst/MemphisPhonkFactory/build/MemphisPhonkFactory_artefacts/Release/`

## Use

- Adjust loop BPM, swing, tape drive, hiss, and variation (1–4).
- Toggle **Loop preview** to hear the break in the plugin.
- **Export MPC Kit** writes `Memphis_Phonk_MPC_Kit/` with 23 files.

## Tests

```bash
cd vst/MemphisPhonkFactory/build && ctest -R MemphisPhonkEngine -V
```
