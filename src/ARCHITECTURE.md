# Logue SDK user units (`src/`)

Korg **logue SDK** custom oscillators and starter effects for **NTS-1 (mkI)**, **minilogue xd**, **Prologue**, and **NTS-1 mkII** (SDK v2 under `src/mkii/`).

## Data flow

1. **v1 units** (`src/oscillators/*`, `src/effects/*`) implement `OSC_*` / `MODFX_*` callbacks in C/C++.
2. `tools/build.sh` selects the platform tree under `$LOGUE_SDK/platform/{nutekt-digital,minilogue-xd,prologue}` and links the SDK `tpl/` glue.
3. **mkII ports** (`src/mkii/oscillators/*`) use float `Osc::process()` in `osc.h`; bass catalog is `PORT_COMPLETE`.
4. `tools/package-nts1.sh` copies `*.ntkdigunit`, `*.mnlgxdunit`, `*.prlgunit`, `*.nts1mkiiunit` into `release/nts1-custom-oscillators/`.

## Build

```bash
./tools/mkii-automate.sh bootstrap
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-units.sh --all-platforms --mkii --package
```

Windows: [docs/BUILD_WINDOWS_LOGUE.md](../docs/BUILD_WINDOWS_LOGUE.md).

## Key modules

| Path | Role |
|------|------|
| `src/shared/dsp_utils.h` | Real-time helpers, `osc_w0()` when `userosc.h` is included |
| `src/shared/logue-v1-unit.mk` | Shared Makefile for v1 platforms |
| `tools/build-all-units.sh` | Full matrix build + `release/logue-build-status.tsv` |
| `tools/mkii/` | mkII scaffold, port scripts, `build-mkii.sh` |

## DSP constraints

- No heap allocation in audio callbacks; bounded delay lines (see `effects/delfx`).
- Optional peak ceiling: `dsp_soft_limit_neg_half_dbfs()` in `dsp_utils.h`.
- mkII and v1 bass ports share the same macro topology per `manifest.json` presets.

## Related docs

- [docs/nts1-multi-bass-oscillators.md](../docs/nts1-multi-bass-oscillators.md)
- [docs/LOGUE_UNIT_STATUS.md](../docs/LOGUE_UNIT_STATUS.md)
- [docs/NTS1_INSTALL_TODAY.md](../docs/NTS1_INSTALL_TODAY.md)
