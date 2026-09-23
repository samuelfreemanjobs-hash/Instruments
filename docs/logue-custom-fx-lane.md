# Custom FX lane (NTS-1 mkII)

**Status:** Planned — requires `LOGUE_SDK` dummy-delay / dummy-reverb templates on the build host.

## Layout

| Path | Role |
|------|------|
| `src/mkii/fx/<slug>/` | Custom delay/reverb-slot units (`.nts1mkiiunit`) |
| `tools/build-mkii-fx.sh` | Build wrapper (to add when SDK path is wired) |
| `tools/mkii/scaffold-mkii-fx.py` | Copy from SDK dummy FX (to add) |

## Planned slugs

- `sp1200_fx` — bit crush + sample-rate reduction (primary)
- `dream_reverb_fx` — bright shimmer reverb
- `tape_echo_dub_fx`, `tape_sat_fx`, `cassette_hiss_fx`

## Load order on hardware

One custom **oscillator** and one custom **FX** can be loaded on NTS-1 mkII; FX processes the synth/drum output in the FX slot. Osc drum kits (`tr808_drumkit_4voice`, etc.) remain oscillators, not FX.

## Agent verify (when SDK available)

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build-mkii-fx.sh sp1200_fx
```

Cloud agents without SDK: document-only until bootstrap.
