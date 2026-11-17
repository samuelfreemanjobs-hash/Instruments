# Logue SDK unit status

Generated from `./tools/build-all-units.sh --all-platforms --mkii --package` on Linux with `gcc-arm-none-eabi` (Debian) and `LOGUE_SDK=.deps/logue-sdk`.

**Hardware:** None of these builds were loaded on physical NTS-1 / XD / Prologue hardware in CI or during this audit.

## Legend

| Column | Meaning |
|--------|---------|
| **DSP** | `complete` = production C++ / mkII `PORT_COMPLETE`; `starter` = template/demo only |
| **Build** | Result of the last full scripted build for that target |

## Bass / genre oscillators (v1 → all platforms)

| v1 folder | mkII slug | Params | DSP | NTS-1 `.ntkdigunit` | XD `.mnlgxdunit` | Prologue `.prlgunit` | mkII `.nts1mkiiunit` |
|-----------|-----------|--------|-----|---------------------|------------------|----------------------|----------------------|
| `tr808-kick` | `tr808_kick_phonk` | 9 | complete | OK | OK | OK | OK |
| `west-coast-moog` | `west_coast_moog_funk` | 8 | complete | OK | OK | OK | OK |
| `juno-rnb` | `juno_106_rnb_bass` | 8 | complete | OK | OK | OK | OK |
| `dilla-bass` | `dilla_type_bass` | 8 | complete | OK | OK | OK | OK |
| `sub-phatty` | `moog_sub_phatty_bass` | 8 | complete | OK | OK | OK | OK |
| `prophet-funk` | `prophet5_funk_rnb_bass` | 8 | complete | OK | OK | OK | OK |
| `dx7-lately` | `dx7_lately_bass` | 8 | complete | OK | OK | OK | OK |
| `sh101-babyface` | `sh101_babyface_bass` | 8 | complete | OK | OK | OK | OK |
| `moog-voyager-se` | `moog_voyager_se1_squelch` | 9 | complete | OK | OK | OK | OK |
| `cardo-gfunk` | `cardo_got_wings_gfunk` | 8 | complete | OK | OK | OK | OK |
| `larry-june` | `larry_june_bay_cruise_bass` | 8 | complete | OK | OK | OK | OK |
| `khalifa-haze` | `khalifa_haze_stoner_bass` | 8 | complete | OK | OK | OK | OK |
| `memphis-juicy` | `juicy_j_three6_memphis_bass` | 8 | complete | OK | OK | OK | OK |
| `ensoniq-eps1` | `ensoniq_eps1_memphis_bass` | 8 | complete | OK | OK | OK | OK |

## Starters & effects

| Unit | DSP | NTS-1 | XD | Prologue | mkII |
|------|-----|-------|-----|----------|------|
| `oscillators/template` | starter | OK | OK | OK | scaffold (`template_osc`) |
| `oscillators/wavetable` | starter | OK | OK | OK | scaffold (`wavetable_osc`) |
| `effects/modfx` | starter | OK | OK | OK | — |
| `effects/delfx` | starter | OK | OK | OK | — |
| `effects/revfx` | starter | OK | OK | OK | — |

## Windows / CI

- Windows: [BUILD_WINDOWS_LOGUE.md](BUILD_WINDOWS_LOGUE.md) · `tools/windows/build-all-logue.ps1`
- GitHub Actions: `.github/workflows/logue-nts1-units.yml` (artifact zip)
- Machine-readable log: `release/logue-build-status.tsv` after each `build-all-units.sh` run

## Refresh

```bash
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-units.sh --all-platforms --mkii --package
```
