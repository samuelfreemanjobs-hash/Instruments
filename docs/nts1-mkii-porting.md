# NTS-1 mkII porting guide

All units under `src/oscillators/` are **logue SDK v1.1.0** (`userosc.h`, `OSC_CYCLE`, `.prg` on mkI).  
**NTS-1 mkII** needs **SDK v2** (`unit.cc`, C++ `Osc` class, `.nts1mkiiunit` shared object).

## What is automated

| Step | Tool |
|------|------|
| Generate mkII tree (`header.c`, `config.mk`, `Makefile`, `unit.cc`, stub `osc.h`) | `python3 tools/mkii/scaffold-mkii.py --all-bass` |
| Build one unit | `./tools/build-mkii.sh <slug>` |
| Scaffold + build entire catalog | `./tools/build-all-bass-mkii.sh` |
| Docker build (link into SDK tree) | `./tools/build-mkii.sh <slug> --docker` then `build nts-1_mkii/inst-<slug>` in logue-sdk container |

Generated projects live in **`src/mkii/oscillators/<slug>/`**. Each folder has **`PORTING.md`** with unit-specific v1 source paths.

## One-time setup (automated)

From the repo root:

```bash
chmod +x tools/mkii-automate.sh
./tools/mkii-automate.sh bootstrap    # clones .deps/logue-sdk, CMSIS, ARM gcc 10.3
./tools/mkii-automate.sh scaffold     # refresh mkII trees (keeps PORT_COMPLETE osc.h)
./tools/mkii-automate.sh build tr808_kick_phonk
./tools/mkii-automate.sh all          # bootstrap + scaffold + build entire catalog
```

Default SDK path: **`<repo>/.deps/logue-sdk`** (override with `export LOGUE_SDK=...`).

If gcc download fails (CI/firewall), use **Docker**: `./tools/mkii-automate.sh docker tr808_kick_phonk` then `build nts-1_mkii/inst-tr808_kick_phonk` inside the logue-sdk container.

Manual setup: clone [logue-sdk](https://github.com/korginc/logue-sdk), `git submodule update --init`, install gcc from `tools/gcc/get_gcc_10_3-2021_10_linux.sh`.

## What you still must do (DSP port)

The scaffold **does not** translate `OSC_CYCLE` automatically. For each unit:

1. Open the v1 `.cc` file listed in `PORTING.md`.
2. Move audio logic into `Osc::process()` in `osc.h` (float samples, `w0_` phase increment).
3. Wire parameters in `setParameter()` (mkII knobs are **0–1023**; v1 presets use **0–100** — use `param_10bit_to_f32()` or scale).
4. Implement `noteOn` / `noteOff` on the `Osc` class if the v1 unit uses `OSC_NOTEON` / `OSC_NOTEOFF`.
5. Assign **`dev_id`** / **`unit_id`** in `header.c` before public release ([developer IDs](https://github.com/korginc/logue-sdk/blob/master/developer_ids.md)).

**Reference:** `tr808_kick_phonk` includes a full mkII port in `osc.h` (ported from `kick808.cc`).

### v1 → mkII API cheat sheet

| v1 (mkI) | mkII |
|----------|------|
| `OSC_INIT` | `Osc::init()` |
| `OSC_CYCLE(..., int32_t *yn, frames)` | `Osc::process(in, out, frames)` float mono |
| `osc_w0(params->pitch)` | `setPitch(osc_w0f_for_note(...))` in `unit_render` |
| `OSC_PARAM(i, 0..65535 or 0..100)` | `unit_set_param_value` → `setParameter(i, 0..1023)` |
| `f32_to_q31(x)` | write `x` directly to `out[0]` |

## Build outputs

- Artifact: **`build/<project>.nts1mkiiunit`** (and copy in project root after `make install` rules).
- Load on hardware via **KORG Kontrol Editor** or **loguecli** (see logue-sdk `platform/nts-1_mkii/README.md`).
- **Web simulator:** `make sim` in the unit directory (requires emsdk in SDK).

## Firmware

NTS-1 mkII firmware **≥ 1.0.0** is required for SDK **2.0.0** units.

## CI / agent workflow

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build-all-bass-mkii.sh --scaffold-only   # no toolchain
./tools/build-all-bass-mkii.sh                     # compile all
```

Update `manifest.json` `_agent.targets` to include `nts-1-mkii` when a port is complete.
