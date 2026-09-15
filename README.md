# nts1-coder — multi-bass oscillators for NTS-1

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

Each unit is **multifunction** (mainly **Vibe**) and ships **presets** in
`manifest.json` → `_agent.presets` (knob values 0–100).

Full guide: [docs/nts1-multi-bass-oscillators.md](docs/nts1-multi-bass-oscillators.md)

## Quick start

1. Clone [logue-sdk](https://github.com/korginc/logue-sdk) and set `LOGUE_SDK`.
2. Build one unit or the whole bass set:

```bash
export LOGUE_SDK=/path/to/logue-sdk
chmod +x tools/build.sh tools/build-all-bass.sh

./tools/build.sh oscillators/juno-rnb nutekt-digital
./tools/build-all-bass.sh nutekt-digital
./tools/build.sh oscillators/dilla-bass nutekt-digital
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
