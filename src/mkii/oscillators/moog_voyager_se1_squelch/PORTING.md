# mkII port: moog_voyager_se1_squelch

## Status

**Port complete** — `osc.h` contains full DSP (`PORT_COMPLETE`). Rebuild with `./tools/build-mkii.sh moog_voyager_se1_squelch`; load `*.nts1mkiiunit` on NTS-1 mkII.

## v1 reference

- Source: `src/oscillators/moog-voyager-se/*.cpp`
- Re-port only if v1 DSP changes; then re-run `tools/mkii/port_bass_osc.py` or edit `osc.h` by hand.

See [docs/nts1-mkii-porting.md](../../../docs/nts1-mkii-porting.md).
