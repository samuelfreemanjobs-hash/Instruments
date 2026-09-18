# NTS-1 mkII logue oscillators

## Purpose

Custom **user oscillators** for **NTS-1 mkII** (Korg logue SDK **v2**). Agents and humans ship `.nts1mkiiunit` files from `src/mkii/oscillators/<slug>/`.

## Build & run

```bash
export LOGUE_SDK=/path/to/logue-sdk   # or .deps/logue-sdk after bootstrap
./tools/mkii-automate.sh bootstrap
./tools/build-mkii.sh tr808_kick_phonk
./tools/osc-eval-mkii.sh tr808_kick_phonk
```

## Data flow

```text
MIDI / panel → unit_render (SDK) → Osc::setPitch / setParameter
              → Osc::process(in, out, frames) → float mono out
              → .nts1mkiiunit → Kontrol Editor / device load
```

## Threading / realtime

`Osc::process()` must not allocate, block, or log. State in class members or statics initialized in `init()`.

## Key modules

| Path | Role |
|------|------|
| `src/mkii/oscillators/*/osc.h` | DSP (`Osc` class) |
| `src/mkii/oscillators/*/unit.cc` | SDK glue |
| `src/oscillators/*` | v1.1 sources for porting |
| `tools/mkii/scaffold-mkii.py` | Generate mkII tree |
| `tools/build-mkii.sh` | Compile one slug |
| `tools/osc-eval-mkii.sh` | Static + optional build gate |

## Extension points

- New slug: scaffold from v1 folder or copy `template_osc`
- Presets: document in v1 `manifest.json` `_agent.presets`; mirror semantics in mkII param scaling

## Related docs

- [docs/nts1-mkii-porting.md](../docs/nts1-mkii-porting.md)
- [docs/logue-mkii-oscillator-agent.md](../docs/logue-mkii-oscillator-agent.md)
- [docs/logue-mkii-golden-unit.md](../docs/logue-mkii-golden-unit.md)
