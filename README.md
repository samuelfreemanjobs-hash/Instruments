# Instruments — VST boilerplate + NTS-1 multi-bass

## VST / VSTi (start here for plugins)

**Every new plugin starts from `vst/template/`** (JUCE, VST3 + Standalone).
Cursor rule: `.cursor/rules/06-vst-vsti-template.mdc`.

```bash
git submodule update --init vst/JUCE   # once
./tools/new-vst.sh MySynth
cd vst/MySynth && cmake -B build && cmake --build build --config Release
```

Details: [vst/README.md](vst/README.md).

**VST lifecycle (required):** [docs/vst-lifecycle.md](docs/vst-lifecycle.md) — Dream → Plan → Design → Implement → Ship (`tools/package-vst.sh`)

**APC commands:** [docs/apc-workflow.md](docs/apc-workflow.md) · `/apc-dream` … `/apc-ship`

**DSP architecture:** [docs/dsp-architecture-specification.md](docs/dsp-architecture-specification.md) · `.agents/skills/` + `.cursor/skills/`

---

## NTS-1 multi-bass (logue SDK)

A **logue SDK** project and Cursor agent scaffold for building **genre bass and
kick user oscillators** on the Korg **NTS-1** (also builds for Minilogue XD /
Prologue). Each oscillator is a separate unit you load when you need that sound.

## Bass collection

| Unit | Name | Use |
|------|------|-----|
| [`tr808-kick`](src/oscillators/tr808-kick/) | **Phonk 808** | 808 kick / sub (phonk) |
| [`west-coast-moog`](src/oscillators/west-coast-moog/) | **P-Funk WC** | Funk · Moog · West Coast |
| [`juno-rnb`](src/oscillators/juno-rnb/) | **Juno R&B** | 80s R&B / Juno-106 bass |
| [`dilla-bass`](src/oscillators/dilla-bass/) | **Dilla Bass** | J Dilla–style wobble / glide / warm MPC tone |
| [`sub-phatty`](src/oscillators/sub-phatty/) | **Sub Phatty** | Moog Sub Phatty dual osc, sub, Multidrive |
| [`prophet-funk`](src/oscillators/prophet-funk/) | **P5 Funk RB** | Prophet-5 70s funk / 80s R&B bass |
| [`dx7-lately`](src/oscillators/dx7-lately/) | **Lately Bass** | DX7 FM electric bass (Lately Bass) |
| [`sh101-babyface`](src/oscillators/sh101-babyface/) | **SH-101 BF** | Warm PWM pulse, sub, hollow FM bass |
| [`moog-voyager-se`](src/oscillators/moog-voyager-se/) | **Voyager SE** | Model D / SE-1 fat stack + glide squelch |
| [`cardo-gfunk`](src/oscillators/cardo-gfunk/) | **Cardo GF** | Sub + warm mid late-night G-funk bass |
| [`larry-june`](src/oscillators/larry-june/) | **Larry June** | Laid-back rubbery PWM Bay cruise bass |
| [`khalifa-haze`](src/oscillators/khalifa-haze/) | **Khalifa Haze** | Warm floating cloud sub for stoner-rap |
| [`memphis-juicy`](src/oscillators/memphis-juicy/) | **Juicy 36** | Memphis trunk sub + tape (layer under kick) |
| [`ensoniq-eps1`](src/oscillators/ensoniq-eps1/) | **EPS-1 MEM** | Ensoniq-style dark wavetable Memphis sub |

Each unit is **multifunction** (mainly **Vibe**) and ships **presets** in
`manifest.json` → `_agent.presets` (knob values 0–100).

Full guide: [docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md)

### NTS-1 mkII (SDK v2)

mkI units above use logue SDK **v1.1.0**. For **NTS-1 mkII**, scaffold and build
`.nts1mkiiunit` files from `src/mkii/oscillators/`:

```bash
chmod +x tools/mkii-automate.sh
./tools/mkii-automate.sh all                    # clone SDK, gcc, scaffold, build all
./tools/mkii-automate.sh bootstrap              # SDK + toolchain only
./tools/mkii-automate.sh build tr808_kick_phonk # one unit
```

Manual steps (custom `LOGUE_SDK`): `python3 tools/mkii/scaffold-mkii.py --all-bass`, `./tools/build-mkii.sh <slug>`.

See [docs/nts1-mkii-porting.md](docs/nts1-mkii-porting.md).

## Quick start

1. Clone [logue-sdk](https://github.com/korginc/logue-sdk) and set `LOGUE_SDK`.
2. Build one unit or the whole bass set:

```bash
export LOGUE_SDK=/path/to/logue-sdk
chmod +x tools/build.sh tools/build-all-bass.sh

./tools/build.sh oscillators/juno-rnb nutekt-digital
./tools/build-all-bass.sh nutekt-digital
./tools/build.sh oscillators/dilla-bass nutekt-digital
./tools/build.sh oscillators/dx7-lately nutekt-digital
```

3. Load the resulting user osc on the NTS-1 with the Korg logue workflow.

## Layout

- `.cursor/rules/` — SDK + **multi-bass** conventions (`05-multi-bass.mdc`)
- `src/oscillators/` — bass units above + `template` / `wavetable`
- `src/effects/` — starter mod/del/rev FX
- `src/shared/` — DSP helpers
- `tools/build-all-bass.sh` — build all catalog bass oscs

## License

Example DSP is a starting point; align with Korg SDK terms for distribution.
