# Factory preset banks (200 programs)

Both flagship synths ship **200 factory programs** aimed at MPC / sample-pack workflows: searchable names, category filters, and headless WAV bounce.

## JD Upgraded — 200 programs

| Index | Bank | Count | Marketing angle |
|------:|------|------:|-----------------|
| 0–23 | EP | 24 | Glass / JD-style keys |
| 24–47 | Pad | 24 | Wide stacks |
| 48–63 | Bass | 16 | Sub-focused |
| 64–79 | Vaporwave | 16 | Mall / VHS names |
| 80–95 | 80s R&B | 16 | Classic stacks |
| 96–111 | 90s R&B | 16 | Layered keys |
| 112–127 | Elite | 16 | Sync / ring / X-mod design |
| **128–151** | **Trap** | **24** | 808-forward MPC packs |
| **152–175** | **Phonk** | **24** | Cowbell / organ / Memphis |
| **176–199** | **Lo-Fi** | **24** | Vapor-adjacent chill |

**Curated anchors:** 0, 24, 48, 64, 80, 112, **128**, **152**, **176**.

Implementation: `Source/Assets/FactoryPatchLibrary.cpp` (`kFactoryPatchCount = 200`).

## WAVE-9090 — 200 programs

Eight lanes × 25 variations:

| Category | Programs | Use |
|----------|---------:|-----|
| TRAP | 0–24 | 808, subs, dark plucks |
| PHONK | 25–49 | Cowbell, dirt, detune |
| DRILL | 50–74 | Stabs, short envelopes |
| BELL | 75–99 | Metallic / funeral bells |
| PAD | 100–124 | Tape-wobble beds |
| LEAD | 125–149 | Night-drive leads |
| PLUCK | 150–174 | Short trap plucks |
| KEYS | 175–199 | Smoke keys / wide stack |

Implementation: procedural `FactoryPresets.cpp` with curated overrides at 0–3, 25, 50, 175.

## UI — Preset Browser

Shared component: `Source/UI/PresetBrowserComponent.*`

- Category filter + text search
- Scrollable list (200 rows)
- Prev / next + program summary

| Plugin | Editor change |
|--------|----------------|
| **JD Upgraded** | Browser strip under title; default size **760×920**, resizable |
| **WAVE-9090** | **300px right sidebar**; default **940×420**, resizable |

## Headless render / MPC packs

```bash
cmake --build build -j --target OfflineRender Wave9090OfflineRender
./scripts/render_mpc_preview_pack.sh /path/to/output
```

Use `Wave9090OfflineRender` program indices **0–199** and `OfflineRender` program indices **0–199** for batch bounces.

## Review checklist

1. Open each standalone → confirm **200** shows in browser footer (`N / 200`).
2. Filter **Trap** on WAVE-9090 → 25 programs.
3. Filter **Vaporwave** on JD Upgraded → 16 programs.
4. Spot-check curated names at indices 0, 64, 128, 152, 176.
5. Run `ctest -R Wave9090PresetCount --output-on-failure`.

Related: [PRESETS.md](PRESETS.md) · [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md)
