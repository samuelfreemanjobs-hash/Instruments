# Custom FX lane (NTS-1 mkII)

**Status:** **FX units implemented** (static eval in CI). ARM build requires `LOGUE_SDK` on host.

## Layout

| Path | Role |
|------|------|
| `src/mkii/fx/<slug>/` | modfx / revfx / delfx `.nts1mkiiunit` |
| `src/mkii/fx/ARCHITECTURE.md` | Product architecture |
| `tools/build-mkii-fx.sh` | Build one FX slug |
| `tools/fx-eval-mkii.sh` | Static (+ optional build) gate |

## Slot types (mkII)

| SDK module | Example slug |
|------------|----------------|
| `k_unit_module_modfx` | `broken_cassette_trap_fx`, `parallel_comp_fx` |
| `k_unit_module_revfx` | `cardo_got_wings_spring_revfx` |
| `k_unit_module_delfx` | _(planned: `tape_echo_dub_fx`)_ |

Only **one custom unit per slot** is loaded at a time; swap `.nts1mkiiunit` files in Kontrol Editor.

## Shipped FX

| Slug | Genre / reference |
|------|-------------------|
| `broken_cassette_trap_fx` | **Trap**-tilted broken cassette — 16-step **digital glitch table**, crush, hiss, tempo gate |
| `parallel_comp_fx` | Parallel NY compression (dry + squashed branch) |
| `cardo_got_wings_spring_revfx` | **Cardo Got Wings** drum spring — preset **WINGZ** / OPEN / TIGHT |

## Verify

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/fx-eval-mkii.sh broken_cassette_trap_fx --static-only
./tools/build-mkii-fx.sh cardo_got_wings_spring_revfx
```

## Related

- [logue-agent-pm-automation.md](logue-agent-pm-automation.md)
- [logue-oscillator-ideas-backlog.md](logue-oscillator-ideas-backlog.md)
