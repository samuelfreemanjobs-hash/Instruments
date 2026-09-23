# NTS-1 mkII custom FX

## Purpose

Custom **modfx / revfx / delfx** units for NTS-1 mkII (`.nts1mkiiunit` per slot type). Process **stereo in → stereo out** via `unit_render()`.

## Build & run

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build-mkii-fx.sh broken_cassette_trap_fx
./tools/fx-eval-mkii.sh cardo_got_wings_spring_revfx --static-only
```

## Shipped units (PORT_COMPLETE)

| Slug | Slot | Role |
|------|------|------|
| `broken_cassette_trap_fx` | modfx | Trap-biased glitch table + crush/hiss |
| `parallel_comp_fx` | modfx | Parallel NY compression |
| `cardo_got_wings_spring_revfx` | revfx | Cardo Got Wings–style drum spring |

## Data flow

```text
Synth/drum bus (stereo) → Fx::process → out to host FX chain
```

## Related

- [docs/logue-custom-fx-lane.md](../../docs/logue-custom-fx-lane.md)
- [docs/logue-agent-pm-automation.md](../../docs/logue-agent-pm-automation.md)
