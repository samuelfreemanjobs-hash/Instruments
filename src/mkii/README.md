# NTS-1 mkII units (SDK v2)

Generated and maintained mkII ports live here. v1 sources remain under `src/oscillators/`.

```bash
export LOGUE_SDK=/path/to/logue-sdk
python3 tools/mkii/scaffold-mkii.py --all-bass    # create/update scaffolds
./tools/build-mkii.sh tr808_kick_phonk              # build one .nts1mkiiunit
./tools/build-all-bass-mkii.sh                       # build all
```

See [docs/nts1-mkii-porting.md](../docs/nts1-mkii-porting.md).

**Fully ported:** `tr808_kick_phonk` (Phonk 808). Other folders are scaffolds until `osc.h` is finished.
