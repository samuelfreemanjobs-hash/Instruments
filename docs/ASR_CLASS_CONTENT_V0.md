# ASR-class rompler — content stack v0 (locked)

**Decision (product owner):**

1. **Start with both** [VCSL](https://github.com/sgossner/VCSL) and **VSCO 2 Community Edition** (CC0 orchestral/general material).
2. **Use** [Salamander Grand Piano](https://github.com/sfzinstruments/SalamanderGrandPiano) (**CC-BY 3.0** — attribution required in product/docs).
3. **Agent + parametric factory** remains the source for **ASR-style chromatic waves** and electronic layers.

Engine/platform: [ASR_CLASS_ROMPLER.md](ASR_CLASS_ROMPLER.md) · Authoring: [ENSONIQ_SAMPLE_AUTHORING.md](ENSONIQ_SAMPLE_AUTHORING.md)

## Roles

| Source | Role in v0 library | License |
|--------|-------------------|---------|
| VCSL | Breadth: world, orchestral, experimental | **CC0** |
| VSCO 2 CE | Core orchestral sections, consistent mapping | **CC0** |
| Salamander | Flagship acoustic piano | **CC-BY 3.0** (credit Alexander Holm / SFZ Instruments) |
| `tools/rompler-factory/` | `ASR_Wave_*` short chromatic zones | **You own** (factory_parametric) |

## On-disk layout (git)

```text
content/asr-class-v0/
├── ATTRIBUTION.md          ← shipped credits (required for Salamander)
├── README.md               ← how to fetch large SFZ packs (not in git)
├── factory/                ← agent-generated WAVs + zones.json (committed, small)
│   └── waves/
└── third_party/            ← gitignored; local SFZ/WAV after fetch script
    ├── vcsl/
    ├── vsco2ce/
    └── salamander/
```

HISE project (future): `hise-sketch/AsrClass/` references `content/asr-class-v0/` via relative paths or copied subsets.

## Workflow

### A. Third-party libraries (you, once per machine)

```bash
./tools/rompler-factory/fetch_third_party_libraries.sh
```

Downloads/releases go to `content/asr-class-v0/third_party/`. Pick instruments in **Polyphone** or load SFZ in **sfizz**; export/curate subsets into HISE SampleMaps. Do not commit multi-GB trees.

### B. Factory waves (agent / CI-friendly)

```bash
python3 tools/rompler-factory/generate_asr_waves.py --preset dark --seed-base 1000
```

Writes `content/asr-class-v0/factory/waves/<preset>/` + `zones.json` for HISE import ([authoring doc](ENSONIQ_SAMPLE_AUTHORING.md)).

### C. HISE (Windows / Antigravity)

1. Import `factory/waves/...` using `zones.json` root keys.
2. Add curated SFZ subsets from `third_party/` (piano = Salamander path first).
3. Build performance presets (layers); FX as ASR B2 stand-in.

## v0 instrument checklist (suggested)

| Map ID | Source | Priority |
|--------|--------|----------|
| `ASR_Wave_Dark` | factory | P0 |
| `ASR_Wave_Bright` | factory | P0 |
| `ASR_Piano_Salamander` | Salamander (subset or full) | P0 |
| `ASR_Strings_VSCO` | VSCO2 CE strings | P1 |
| `ASR_Brass_VCSL` | VCSL brass pick | P1 |

## Legal reminders

- **CC0 (VCSL/VSCO):** no attribution required; still document provenance in `ATTRIBUTION.md` for transparency.
- **Salamander:** attribution required; note derivative SFZ if you remap ([repo README](https://github.com/sfzinstruments/SalamanderGrandPiano)).
- **No** Ensoniq ROM/disk content in this stack.

## Related

- [tools/rompler-factory/README.md](../tools/rompler-factory/README.md)
- [ENSONIQ_CLASS_SAMPLER_PLATFORM.md](ENSONIQ_CLASS_SAMPLER_PLATFORM.md)
