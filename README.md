# nts1-coder

Cursor agent scaffold for building **Korg logue SDK** user oscillators and effects
(NTS-1, Minilogue XD, Prologue; with notes for mkII / NTS-3).

## Quick start

1. Clone [logue-sdk](https://github.com/korginc/logue-sdk) and set `LOGUE_SDK`.
2. Open this repo in Cursor — rules under `.cursor/rules/` load automatically.
3. Build a template unit:

```bash
export LOGUE_SDK=/path/to/logue-sdk
chmod +x tools/build.sh
./tools/build.sh oscillators/template nutekt-digital
./tools/build.sh oscillators/tr808-kick nutekt-digital
./tools/build.sh oscillators/west-coast-moog nutekt-digital
./tools/build.sh oscillators/juno-rnb nutekt-digital
```

**Phonk 808** (`tr808-kick`): long sub decay, **Sub** / **Drive** knobs — play low notes (C1–F1), long **Decay** for rumble.

**P-Funk WC** (`west-coast-moog`): Minimoog-style dual osc + filter (uses host **cutoff/res**), glide, sub — **Vibe** low = Parliament bass, high = West Coast lead/fold.

**Juno R&B** (`juno-rnb`): Juno-106 style 80s R&B bass — **Vibe** = ballad ↔ groove ↔ bright; **Chorus**, **PWM**, panel **cutoff/res**.

## Layout

- `.cursor/rules/` — agent identity, SDK API, DSP patterns, build matrix, style
- `src/oscillators/` — `template`, `wavetable`, `tr808-kick`, `west-coast-moog`, `juno-rnb`
- `src/effects/` — `modfx`, `delfx`, `revfx`
- `src/shared/` — `dsp_utils.h`, `logue_helpers.h`
- `tools/build.sh` — build wrapper
- `docs/` — SDK notes and platform table

## Agent system prompt

The legacy `.cursorrules` file mirrors the user-rule prompt from the project spec.
Prefer `.cursor/rules/00-role.mdc` for version-controlled, scoped rules.

## Tooling ideas

- [logue-sdk websim](https://github.com/korginc/logue-sdk) — browser DSP iteration (mkII / NTS-3)
- [hvcc_nts1mkii](https://github.com/boochow/hvcc_nts1mkii) — Pure Data → logue units

## License

Example DSP code is provided as a starting point; align licensing with your
distribution and Korg SDK terms.
