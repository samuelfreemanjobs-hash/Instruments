# Method 1 — Cycle-accurate chip emulation (ROM-backed)

How **chip-core + firmware/wave ROM** plugins fit the **Melodee / MPC Sample** lane in this monorepo, and how they differ from **Method 2** (VA / algorithmic JUCE DSP in `vst/Mpc*`).

## Two methods (summary)

| | **Method 1 — Chip emulation** | **Method 2 — VA / model** (current `MpcVoyager`, `MpcMessenger`, etc.) |
|---|------------------------------|----------------------------------------------------------------------|
| **DSP** | Reuse open-source **ASIC/CPU core** (MAME, Nuked-OPN2, …) | `juce::dsp`, ladders, morph oscs, FM graphs |
| **Timbre** | User-supplied **`.bin` ROM / firmware** (exact factory waves + quirks) | Factory **programs** in code (`FactoryPrograms.h`) |
| **CPU on MPC** | Often **low–moderate** for 80s FM (YM2612, OPL); can be **heavy** for LA-32 / sample+DSP hybrids | Tuned per SKU (mono bass = tiny; **MpcCs80** dual-layer = costly) |
| **Legal in repo** | **Never commit** copyrighted ROM dumps | Clean-room only |
| **Melodee fit** | Excellent when core is small and headless params map to K1–K3 | Default factory path today |

Method 1 does **not** replace APC lifecycle: still **Dream → Plan → Design → Impl** under `plugins/<Name>/`, then a JUCE **wrapper** in `vst/<Name>/`.

## How it works in JUCE

```text
┌─────────────────────────────────────────────────────────┐
│ MpcDxFmAudioProcessor (JUCE)                          │
│  prepareToPlay: load ROM if path valid, chip->reset()   │
│  processBlock: MIDI → chip regs; chip->generate(buf)    │
│  APVTS: map K1–K3 to chip registers / macro offsets   │
└───────────────────────┬─────────────────────────────────┘
                        │ calls (no alloc in callback)
                        ▼
┌─────────────────────────────────────────────────────────┐
│ Chip core (C/C++, MIT/BSD/GPL as licensed)            │
│  e.g. Nuked-OPN2, DOSBox OPL, MAME YM2612 slice         │
│  reads uint8_t rom[] or mmap’d .bin                     │
└─────────────────────────────────────────────────────────┘
```

### ROM location on microSD (operator policy)

Melodee loads **plugins** from `Melodee/Plugins/*.vst3`. **ROM files stay user-owned** — ship the plugin without dumps; document a sidecar path on the same card, e.g.:

| Path | Purpose |
|------|---------|
| `Melodee/Plugins/MpcDxFm.vst3` | aarch64 VST3 bundle |
| `Melodee/Data/MpcDxFm/ym2612.bin` | Operator-supplied wave ROM (optional second file for FM ROM) |
| `Melodee/Data/MpcDxFm/firmware.bin` | Optional MCU firmware for hybrid engines |

In `prepareToPlay` (or first block edge only), resolve:

1. Environment override (dev): `MPC_ROM_PATH` / per-plugin `MPC_MPCDXFM_ROM`
2. Default relative search: `../Data/<PluginName>/*.bin` from the host’s plugin install dir (document exact behavior in Design)
3. **Fail soft:** if ROM missing, init **synthetic fallback** (sine FM) and expose a parameter “ROM loaded” for debugging

**Never** bake Roland/Yamaha/E-mu copyrighted binaries into git or release zips.

## Representative cores (research lane — not vendored until Design approves license)

| Vintage target | Typical core projects | ROM / data | MPC Notes |
|----------------|----------------------|------------|-----------|
| DX7 / TX81Z (**YM2612**) | [Nuked-OPN2](https://github.com/nukeykt/Nuked-OPN2), MAME YM2612 | FM + optional ROM chips | **MpcDxFm** — good Method-1 candidate; 4–8 voices realistic |
| OPL2/OPL3 | DOSBox, Nuked-OPL3 | Minimal or embedded | Drum/organ lanes; very cheap |
| D-50 / **LA** synthesis | Partial MAME, community LA cores | **Large** PCM + control ROM | **High CPU**; often hybrid VA + small PCM bank (see **MpcLaSynth** plan) |
| Ensoniq DOC/OTTO | Various emu efforts | Wave + program ROM | Legal + size constraints; consider VA first |
| E-mu E-chip | Scarcer open cores | Wave ROMs | Same |

**Reality check:** “Almost zero CPU” applies to **single-digit FM chips at modest polyphony**, not to **full LA sample+synth** systems. Profile on **aarch64** before claiming 16 voices.

## Wiring parameters to MPC K1–K3

Same rule as all Melodee plugins: register **musical macros first** in APVTS / `params.json`:

Example **MpcDxFm** (chip mode):

1. **K1** — algorithm or OP1 level (macro)
2. **K2** — feedback / brightness register shadow
3. **K3** — LFO depth or chorus send (if core exposes it)

Factory **patches** = either:

- **Register snapshots** captured from hardware SysEx (converted to JSON in `plugins/MpcDxFm/data/`, no ROM), or
- **True ROM preset index** when the emulated MCU exposes program banks (user ROM required)

## CMake / build integration

1. **Vendor the core** as a git submodule or `third_party/nuked-opn2/` with license file (GPL vs MIT affects shipping — legal review in Design).
2. **Statically link** the core into the VST3; no `dlopen` on the audio thread.
3. Cross-build with `./tools/build-vst-aarch64-linux.sh` / `build-vst-aarch64-podman.sh`; verify with `verify-mpc-sample-vst3.sh`.
4. **Tests:** offline render 1 s @ 48 kHz with a **tiny synthetic ROM** (generated in test, not factory dump) to lock register→audio contract.

## Repo SKU mapping

| SKU | Default method | Method-1 upgrade path |
|-----|----------------|----------------------|
| **MpcDxFm** | L2 FM graph (Plan) | Optional `MPCDXFM_CHIP=1` build flag → Nuked-OPN2 + user ROM |
| **MpcLaSynth** | Embedded royalty-free PCM + synth (**no** D-50 ROM) | Do **not** load M1/D-50 dumps; LA emulation belongs in a **separate** SKU if ever attempted |
| **MpcJuno106** / **MpcCs80** | VA | Chip emulation N/A (custom ASIC modeling is Method 2) |
| **MpcTr808909** | Analog-style drums | TR machines are analog; no ROM |

## Security & licensing

- ROM paths from host are **untrusted input** — bound path length, reject `..`, only read under `Melodee/Data/`.
- `.cursor/rules/security-baseline.mdc` — no keys, no ROMs in git.
- User must **own** hardware or use redistribution-permitted dumps; document that in `INSTALL.txt` at ship.

## Related docs

- [MPC_SAMPLE_VST3.md](MPC_SAMPLE_VST3.md) — aarch64 bundle layout, Melodee install
- [plugins/MpcDxFm/.ideas/architecture.md](../plugins/MpcDxFm/.ideas/architecture.md) — VA FM fallback
- [plugins/MPC_CLASSIC_LINE.md](../plugins/MPC_CLASSIC_LINE.md) — product index

## Next agent steps

1. `/apc-design MpcDxFm` — choose **VA-only v1** vs **chip wrapper v1** (recommended: VA ship first, chip behind CMake flag).
2. If chip path: add `third_party/` README, ROM loader unit test, Melodee `Data/` convention in design-spec.