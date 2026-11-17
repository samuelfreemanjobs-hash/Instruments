# MPC Sample Play 6-pack — headless build contract

Target: **Melodee** on Akai **MPC Sample** — `vst/MpcPlay*/`, **aarch64** `.vst3`, **no editor**.

## Scaffold

1. Copy `templates/mpc-play-synth-base/` → `vst/<Name>/`.
2. Set `PLUGIN_CODE` in `CMakeLists.txt` per `mpc_play_synth_6pack_master.json`.
3. Customize only:
   - `Source/LayerAEngine.h` (or include path to desktop twin after port)
   - `Source/LayerBEngine.h`
   - `Source/MacroRouter.h` — map **macro1–macro3** to K1–K3 on MPC first
   - `Source/FactoryPrograms.h` — 8 factory programs (Melodee preset step)
4. `createParameterLayout()`: **first three** parameters MUST be the K1–K3 controls (names in master JSON).

## DSP limits (MPC)

- `kVoiceCount` ≤ 8 (6 for phonk/cowbell SKU).
- No `juce::dsp::Oversampling` on audio thread unless ≤ 2× and profiled.
- Skip PerformanceEngine arp/chord if CPU tight; keep dual-layer + macros.
- `hasEditor() == false`, `MPC_VST3_HEADLESS=ON`.

## Build & verify

```bash
./tools/build-vst.sh MpcPlayPhonk
./tools/build-vst-aarch64-linux.sh MpcPlayPhonk
./scripts/vst/verify-mpc-sample-vst3.sh vst/MpcPlayPhonk/build-aarch64/MpcPlayPhonk_artefacts/Release/VST3/MpcPlayPhonk.vst3
```

Batch: `tools/run-factory-mpc-play6.sh`.

## Desktop parity

DSP spec is identical to `play_synth_6pack_master.json` **layerA/layerB**; desktop adds WebView + 8 macros UI. When fixing DSP bugs, prefer fixing `templates/juce8-synth-base/Source/*` then port diffs into `vst/MpcPlay*/Source/`.