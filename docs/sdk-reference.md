# logue SDK reference (curated)

This document complements `.cursor/rules/01-sdk-api.mdc` with repo-specific notes.

## Repository layout

| Path | Purpose |
|------|---------|
| `src/oscillators/*` | Custom oscillator units (`module`: `osc`) |
| `src/effects/modfx` | Modulation FX (`module`: `modfx`) |
| `src/effects/delfx` | Delay FX (`module`: `delfx`) |
| `src/effects/revfx` | Reverb FX (`module`: `revfx`) |
| `src/shared/` | Headers safe to include from any unit |

## Prerequisites

1. Clone [korginc/logue-sdk](https://github.com/korginc/logue-sdk).
2. Export `LOGUE_SDK` to the checkout root.
3. Build with `./tools/build.sh <unit> <platform>`.

Platform directory names (v1.1.0):

- `nutekt-digital` — NTS-1 mkI
- `minilogue-xd`
- `prologue`

## Oscillator contract (v1.1.0)

- Output: Q31 in `yn` (`int32_t *`), `frames` samples per call.
- Support buffer sizes up to **64** frames; optimize for powers of two.
- Pitch: use `osc_w0(params->pitch)` for phase increment.
- Parameters: handle `OSC_PARAM` and mirror indices in `manifest.json` `params`.

## Effect contracts (v1.1.0)

Read the platform `usermodfx.h`, `userdelfx.h`, and `userrevfx.h` in your SDK
tree before changing signatures — they differ slightly by module type.

## manifest.json

Builds use the official `header` object. This repo adds an optional `_agent` block
(logical id, type, targets) for Cursor rules; strip or ignore `_agent` if your
packaging tool rejects unknown keys.

When retargeting a unit, update `header.platform` and rebuild with the matching
`PLATFORMDIR`.

## Further reading

- [logue-sdk on GitHub](https://github.com/korginc/logue-sdk)
- [Prologue SDK oscillator reference (v1.1.0)](https://korginc.github.io/logue-sdk/ref/prologue/v1.1-0/html/group__osc__inst.html)
