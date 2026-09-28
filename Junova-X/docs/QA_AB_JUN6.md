# Jun-6 V A/B QA protocol (WO-2026-008)

Automated **in-repo** captures: `tests/golden/junova/manifest.tsv` + `JunovaOfflineRender --scenario …`.

Manual **cross-plugin** comparison against Arturia Jun-6 V (or a bounce from [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md) references):

## 1. Match scenario in DAW

| Scenario ID | Starting patch intent |
|-------------|------------------------|
| `ab01-dry-saw` | Chorus off, open filter, full sustain |
| `ab02-fat-pad` | Chorus I, pad filter/env |
| `ab03-chorus-*` | Modes I, II, I+II on held A3 |
| `ab04-filter-sweep` | Resonant filter + envelope |
| `ab05-arp-sync` | Host 120 BPM, 1/16 arp, triad (manual — not in CI golden) |
| `ab06-noise-hpf` | HPF on + noise |
| `ab07-unison-lead` | Unison + chorus II |
| `ab-juno6-poly` | 6-voice poly limit |

Render Junova headlessly:

```bash
cmake --build build -j --target JunovaOfflineRender
./build/Junova-X/JunovaOfflineRender /tmp/junova.wav --scenario ab02-fat-pad 48 100 4.0 44100
```

## 2. Export reference from Jun-6 V

Same MIDI note, velocity, length, **no extra FX**. Peak-normalize both files in the DAW if levels differ.

## 3. Spectral diff (optional)

**Open-source KR-106** (built from source, not binary RE):

```bash
./Junova-X/scripts/setup_kr106_reference.sh
cmake --build build -j --target JunovaOfflineRender
./tests/golden/junova/compare_kr106_reference.sh ab03-chorus-i 57 3.0
```

**Any plugin bounce** (EightySix, Yonu60, RJU-60, TAL-U-NO-LX demo, TAL-Chorus-LX, Chorus JUN-6, etc.):

See name mapping: [FREE_JUNO_VST_CATALOG.md](FREE_JUNO_VST_CATALOG.md).

```bash
./tests/golden/junova/compare_jun6_reference.sh /path/to/reference.wav ab02-fat-pad 48 4.0
```

**TAL-Chorus-LX on dry Junova:** render `ab01-dry-saw`, send through chorus in DAW, export, compare to `ab03-chorus-i`.

Thresholds are intentionally loose; **solo + in-mix** listening wins disputes (see Bass Valley / Luke Million methodology in COMPETITIVE_JUN6.md).

## 4. CI

```bash
./tests/golden/verify_junova_golden.sh
```

After intentional DSP changes:

```bash
./tests/golden/refresh_junova_golden.sh
git add tests/golden/junova/*.wav
```
