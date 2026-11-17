# PlaySynth 6-pack — **MPC Sample (Melodee)** port

The same six **Kontakt Play Series–style** dual-layer engines as `plugins/PLAY_SYNTH_6PACK.md`, shipped as **headless** `vst/Mpc*` VST3 for **Linux aarch64** on Akai **MPC Sample**.

| Desktop (DAW) | MPC Sample SKU | CODE | Voices | Melodee **K1 / K2 / K3** (default) |
|---------------|----------------|------|--------|-------------------------------------|
| AnalogDreams84 | **MpcPlayAnalog84** | Ma84 | 8 | Blend / Cutoff / BBD |
| CloudSupplyTrap | **MpcPlayCloud** | MpCl | 8 | Tone / FM Bell / Sub |
| LoFiGlowTape | **MpcPlayLoFi** | MpLf | 8 | Warmth / Wow / Crunch |
| PhonkCowbell94 | **MpcPlayPhonk** | Mp94 | 6 mono+glide | Cowbell / 808 Sub / Tape |
| FortyKeysSoul | **MpcPlayForty** | Mp40 | 8 | Underwater / Sub / Chorus |
| SoulSessions70 | **MpcPlaySoul70** | Ms70 | 8 | Tone / Auto-Wah / Rotary |

## Shared DSP with desktop

Reuse the pre-wired core under `templates/juce8-synth-base/Source/`:

- `LayerAEngine.h`, `LayerBEngine.h`, `MacroRouter.h`, `PlaySynthVoice.*`
- **Do not** port WebView, Dice UI, or 2× oversampling on every SKU unless CPU budget allows (see below).

MPC scaffold: `templates/mpc-play-synth-base/` → copy to `vst/MpcPlay<Name>/`.

## MPC rules (non-negotiable)

Read **`docs/MPC_SAMPLE_VST3.md`** first.

| Rule | MPC headless |
|------|----------------|
| `hasEditor()` | `false` |
| `JUCE_WEB_BROWSER` | `0` |
| Build | `./tools/build-vst.sh <Name>` then `./tools/build-vst-aarch64-linux.sh <Name>` |
| Verify | `./scripts/vst/verify-mpc-sample-vst3.sh build-aarch64/.../Name.vst3` |
| Parameters | Put **K1–K3** targets first in APVTS order (see table above) |
| Factory programs | `FactoryPrograms.h` + `getNumPrograms()` / `setCurrentProgram()` (Melodee preset step) |
| CPU | 6–8 voices typical; no unison×7 on device |

## Factory queues

| File | Use |
|------|-----|
| `disklordz/vst-factory/data/mpc_play_synth_6pack_master.json` | Compact MPC batch manifest |
| `disklordz/vst-factory/data/mpc_play_factory_contract.md` | Headless agent contract |
| `tools/run-factory-mpc-play6.sh` | Linux: scaffold → build x86 → aarch64 → verify |

Desktop overnight (Windows WebView) stays **`play_synth_6pack_master.json`** + `run-factory-play6.ps1`.

## APC layout

| Lane | APC path | Code path |
|------|----------|-----------|
| Desktop | `plugins/PhonkCowbell94/` … | `PhonkCowbell94/` at repo root |
| MPC Sample | `plugins/MpcPlayPhonk/` … (per SKU) | `vst/MpcPlayPhonk/` |

One creative brief can drive **both** targets: same Layer A/B spec; MPC adds K1–K3 ordering and voice cap in `architecture.md`.

## Related

- Classic hardware line (Juno, Prophet, …): `plugins/MPC_CLASSIC_LINE.md`
- Play desktop index: `plugins/PLAY_SYNTH_6PACK.md`