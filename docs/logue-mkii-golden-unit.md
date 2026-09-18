# Golden mkII reference unit

CI and agents use **`tr808_kick_phonk`** as the canonical **logue SDK v2** oscillator example.

| Item | Path |
|------|------|
| mkII tree | `src/mkii/oscillators/tr808_kick_phonk/` |
| v1.1 source | `src/oscillators/tr808-kick/` (port reference in `PORTING.md`) |
| Meta | `src/mkii/oscillators/tr808_kick_phonk/mkii.meta.json` |

## Why this unit

- Full **DSP port** in `osc.h` (not scaffold-only stub)
- Documents kick / phonk 808 role in the multi-bass catalog
- Listed in [nts1-mkii-porting.md](nts1-mkii-porting.md) as reference port

## Verify

```bash
export LOGUE_SDK="${LOGUE_SDK:-$PWD/.deps/logue-sdk}"
./tools/mkii-automate.sh bootstrap   # once
./tools/osc-eval-mkii.sh tr808_kick_phonk
```

Expected: static checks pass; with toolchain, `build-mkii.sh` produces `*.nts1mkiiunit`.

## New units

Copy structure and patterns from this folder; run scaffold for new slugs:

```bash
python3 tools/mkii/scaffold-mkii.py oscillators/<v1-folder>
```

Then implement `osc.h` using `tr808_kick_phonk/osc.h` as the style reference.
