# Memphis Phonk Factory

**MPC pack maker** — procedural Memphis phonk kits (Standalone + VST3). Tagline: *never the same pack twice.*

## Sound design

- **Dust** macro — tape drive, hiss, wow/flutter, bit depth (Variation still shifts RNG seed).
- **Tighten drums to tempo** — shortens kick/snare decay to fit the loop BPM.

## Pattern workflow

- 16-step kick / snare / open-hat grid; **<< / >>** shift, **Save/Load** JSON, **A/B** patterns.
- **Loop bars:** 1 / 2 / 4 · **Half-time** preview · **Sync DAW BPM** · **Tap** tempo.

## Export (background thread + progress bar)

| Output | Default |
|--------|---------|
| 20 one-shots | always |
| `Memphis_Loop_<bpm>BPM.wav` | your tempo + bar count |
| Classic 112/118/124 | optional |
| `Stems/` kick·snare·hats·mix | on |
| `Memphis_Pattern.mid` | on |
| `PACK_MANIFEST.json` + `README_MPC.txt` | on |

**Batch:** kit count + optional **BPM sweep** (from/to across kits).

## Build

```bash
./tools/build-vst.sh MemphisPhonkFactory
cd vst/MemphisPhonkFactory/build && ctest -R MemphisPhonkEngine -V
```

## Factory

SKU **MemphisPhonkFactory** in `disklordz/vst-factory/data/product-library.json` · `./scripts/sync-memphis-phonk-factory.sh`
