# DISKLORDZ ROMPLER

Sample-based four-tone rompler VSTi (standalone product; not WAVE-909).

Factory audio is **generated at build time** by `tools/GenerateRomplerPack.cpp` (procedural bells, keys, pads, subs) and embedded as `DLROMPR1`.

## Build

From repo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target DisklordzRompler_VST3 DisklordzRompler_Standalone DisklordzRomplerTests
ctest --test-dir build -R DisklordzRompler
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/SAMPLE_PACK.md](Docs/SAMPLE_PACK.md)
- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
