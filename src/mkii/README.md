# NTS-1 mkII units (SDK v2)

Generated and maintained mkII ports live here. v1 sources remain under `src/oscillators/`.

```bash
./tools/mkii-automate.sh all                         # recommended: bootstrap + scaffold + build all
./tools/mkii-automate.sh bootstrap                   # clone .deps/logue-sdk + ARM gcc
./tools/mkii-automate.sh build tr808_kick_phonk
```

Legacy: `python3 tools/mkii/scaffold-mkii.py --all-bass`, `./tools/build-mkii.sh <slug>`.

See [docs/nts1-mkii-porting.md](../docs/nts1-mkii-porting.md).

**Bass catalog:** all `*_bass` / `tr808_kick_phonk` / `moog_voyager_se1_squelch` folders ship **PORT_COMPLETE** `osc.h` DSP (not hardware-tested here). Starters: `template_osc`, `wavetable_osc`.
