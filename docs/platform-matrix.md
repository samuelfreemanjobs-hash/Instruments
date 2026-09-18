# Platform compatibility matrix

| Logical target | SDK platform folder | SDK API | CPU | Binary format |
|----------------|---------------------|---------|-----|---------------|
| NTS-1 mkI | `nutekt-digital` | 1.1-0 | Cortex-M4 | `.prg` / ELF |
| Minilogue XD | `minilogue-xd` | 1.1-0 | Cortex-M4 | Compatible with NTS-1 mkI |
| Prologue | `prologue` | 1.1-0 | Cortex-M4 | Compatible with NTS-1 mkI |
| NTS-1 mkII | (SDK v2 tree) | 2.x | Cortex-M7 | `.so` |
| NTS-3 | (SDK v2 tree) | 2.x | Cortex-M7 | `.so` |

## Building for a v1.1.0 target

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build.sh oscillators/template minilogue-xd
```

Edit `manifest.json` → `header.platform` to match the platform name before
distributing the built unit.

## mkII / NTS-3 (v2.x)

v1 units live under `src/oscillators/`. mkII projects are generated under
`src/mkii/oscillators/` via `tools/mkii/scaffold-mkii.py` and built with
`tools/build-mkii.sh`. See [nts1-mkii-porting.md](nts1-mkii-porting.md).
