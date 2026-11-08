# mkII port: cardo_got_wings_gfunk

## Status

**Port complete** — `osc.h` contains full DSP (`PORT_COMPLETE`). Rebuild with `./tools/build-mkii.sh cardo_got_wings_gfunk`; load `*.nts1mkiiunit` on NTS-1 mkII.

## v1 reference

- Source: `src/oscillators/cardo-gfunk/*.cpp`
- Re-port only if v1 DSP changes; then re-run `tools/mkii/port_bass_osc.py` or edit `osc.h` by hand.

See [docs/nts1-mkii-porting.md](../../../docs/nts1-mkii-porting.md).
