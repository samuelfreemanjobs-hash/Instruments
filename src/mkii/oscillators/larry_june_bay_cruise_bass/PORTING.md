# mkII port: larry_june_bay_cruise_bass

## Status

Scaffold only — `osc.h` plays a placeholder tone until you port DSP.

## Finish the port

1. Open v1 source: `src/oscillators/larry-june/larry_bass.cc`
2. Copy float math from `OSC_CYCLE` into `Osc::process()` in `osc.h` (mkII uses **float** mono out, stereo `in`).
3. Map `OSC_PARAM` cases to `setParameter()` (0–100 v1 → use `param_10bit_to_f32` for 0–1023).
4. Map `OSC_NOTEON` / `OSC_NOTEOFF` to `noteOn` / `noteOff` overrides if needed.
5. Pitch: use `w0_` from `setPitch()` instead of `osc_w0(params->pitch)`.
6. Build: `../../tools/build-mkii.sh larry_june_bay_cruise_bass`
7. Load `*.nts1mkiiunit` on NTS-1 mkII (Kontrol Editor / loguecli).

See [docs/nts1-mkii-porting.md](../../../docs/nts1-mkii-porting.md).
