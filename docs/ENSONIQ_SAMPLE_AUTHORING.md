# Creating samples for Ensoniq-class romplers (ASR / EPS)

**Audience:** You (content author) building **C** — factory maps for HISE Phase 1 and later JUCE `ZoneMap` import.  
**Platform:** [ENSONIQ_CLASS_SAMPLER_PLATFORM.md](ENSONIQ_CLASS_SAMPLER_PLATFORM.md) · lead SKU [ASR_CLASS_ROMPLER.md](ASR_CLASS_ROMPLER.md)

## What you are making

Not disk images or Ensoniq file formats. You are building **multisample instruments**:

- **Zones** — one WAV (or stereo pair) per region: MIDI note range, optional velocity range, **root key**.
- **Loops** — start/end (and optional crossfade) for pads, strings, tones.
- **Performances (ASR)** — several maps layered (e.g. bass + pad + FX hit) saved as one preset.

All material must be **yours, recorded by you, or properly licensed**. Do not rip Ensoniq wave ROM, other people’s `.EFE`/disk images, or commercial library redistribution unless the license allows it in a product.

---

## 1. Capture sources (pick what fits the sound)

| Source | Good for | Tips |
|--------|----------|------|
| **Mic + preamp** | Vocals, acoustic, amps | 48 kHz interface; leave headroom (−12 dBFS peak); consistent distance |
| **DI / reamp** | Synths, drum machines | Record dry; bake ASR-like FX in HISE/plugin later (B2) |
| **Your existing WAVs** | Beats, one-shots, stems | Normalize policy below; strip project watermarks |
| **Soft synths** | “Factory wave” style short tones | Short releases; multisample every 3–5 semitones |
| **Disklordz drum factory** | Layered kits under performances | [disklordz/sound-factory/README.md](../disklordz/sound-factory/README.md) — drum-focused; one-shots can sit in a layer, not a full rompler library by themselves |
| **Cursor / Disklordz factory (agent)** | Chromatic waves, synthetic multis, batch libraries | See [§ Agent as main source](#agent-as-main-source) below |

**ASR lead recommendation:** split library into **(a) short chromatic “waves”** and **(b) longer multisampled instruments** (strings, brass, choirs). EPS-skewed libraries can emphasize (b) and simpler FX.

---

## Agent as main source

You can treat **the agent + repo factories** as the primary sample source instead of recording hardware. That fits the Disklordz direction ([ILLUGEN research](DISKLORDZ_ILLUGEN_RESEARCH.md): orchestration separate from engine).

### Your role vs the agent’s role

| You | Agent / factory |
|-----|------------------|
| Briefs (“ASR wave: dark, short, 32 zones”, “brass section M3, 3 velocities”) | Implements or extends **deterministic renderers**, runs batch jobs, writes WAVs + **manifest** (root key, vel, SHA-256) |
| Picks winners from variations / seeds | Regenerates with new seeds; never ships Ensoniq ROM or scraped libraries |
| Imports approved folders into HISE | Can scaffold SampleMap XML once HISE project exists |

### Three engine tiers (pick per instrument)

1. **Parametric / code (best v0)** — TypeScript or Python synths (same idea as [factory.ts](../disklordz/website/src/lib/generation/factory.ts) and [generate_stub_kits.py](../disklordz/sound-factory/scripts/generate_stub_kits.py)). **You own the output**; seeds reproduce WAVs. Ideal for **ASR “waves”**, bass stabs, synthetic pads, lo-fi tones.
2. **Hosted generative model (later)** — Prompt → remote API → candidates (ILLUGEN-shaped WO-SAAS-007+). Use for **organic** timbres; **read provider ToS** for commercial use inside a **paid plugin**. Agent wires gateway; you approve clips and normalize/loudness-match before zoning.
3. **Hybrid (recommended for “real” brass/strings)** — Model or procedural **body** + agent **post** (loops, crossfade, velocity layers from one good note). You still curate; factory scales spacing across the keyboard.

**Honest limit:** Code-only factories won’t match multi-gig acoustic libraries without tier 2 or recording. For an **ASR-class inspired** SKU, synthetic + curated waves is a valid product identity.

### Target pipeline (not all built yet)

```text
Prompt / instrument spec (YAML or Airtable WO)
  → rompler-factory batch (Python/TS) OR Disklordz generate API (new mode)
  → out/InstrumentName/*.wav + zones.json
  → you audition → HISE SampleMap → AsrClass preset
```

Today: drum **`one_shot` / `loop` / `sfx`** modes only. Rompler batch is a **new WO** (extend `sound-factory` or `tools/rompler-factory/`).

### Provenance fields (use on every WAV)

Record in `zones.json` or manifest:

- `source: "factory_parametric_v1"` or `source: "external_model:<provider>"`
- `seed`, `prompt`, `engine`, `generatedAt`
- Keeps plugin/store compliance auditable

### How to ask the agent

Example WO text:

> Batch **ASR_Wave_Dark**: 48 kHz mono, notes C1–C6 every minor third, 200 ms decay, seed range 1000–1015, output `SampleRaw/Waves/ASR_Wave_Dark/` + `zones.json` for HISE import.

Open a **`[Plugin][HISE]`** or **`[Plugin][Factory]`** row when you want this in git/CI, not only ad-hoc Cloud runs.

---

## 2. Technical format (before HISE)

| Setting | Recommendation |
|---------|------------------|
| Sample rate | **48 kHz** (or 44.1 kHz — pick one library-wide) |
| Bit depth | **24-bit** PCM WAV on disk; HISE can compress to HLAC for release |
| Channels | **Mono** for most zones; ** stereo** only when width is intentional (keep L/R phase coherent) |
| Length | One-shots: natural decay or trim tail silence. Looped: **≥ 2× loop length** beyond loop end for crossfade headroom |
| Naming | `Instrument_RootNote_VelLayer.wav` e.g. `ASR_Brass_C3_vel72.wav`, `ASR_Wave_Fs4.wav` |

**Loudness:** aim **−18 to −12 dBFS** peak on raw captures; one **library normalize** pass later (don’t brick-wall). Document gain staging if you add FX offline.

**Editing:** Reaper, Audacity, iZotope RX, etc. — trim DC offset, fades on non-looped samples, de-noise only when needed (pads).

---

## 3. Multisampling strategy

### How many notes?

| Material | Spacing |
|----------|---------|
| Neutral tones (sine-like waves) | Every **12** semitones (octaves) |
| Medium complexity | Every **5–7** semitones |
| Critical timbre (acoustic, formants) | Every **3** semitones (M3) or **1** for solo lead |

HISE **transposes** between zones; wider spacing = smaller library, more detune artifacts at extremes.

### Velocity layers

- **2–4 layers** is enough for rompler v0 (e.g. pp, mf, ff).
- Same note, different velocities → separate files or one file per layer with non-overlapping vel ranges in the map.

### Root key

Set root to the **MIDI note you played** when recording (or intended pitch for one-shots). Wrong root = wrong tuning across the keyboard.

### Loops

1. Find stable sustain; set **loop start** after attack, **loop end** before noise-only tail.
2. Align to **zero crossings** or use **crossfade** (5–50 ms) in the editor/HISE to kill clicks.
3. Export loop metadata: HISE stores loop points in the **sample map** when you save the project.

---

## 4. Folder layout (local disk, before git import)

```text
AsrClass/                          ← HISE project root (future: hise-sketch/AsrClass/)
├── SampleRaw/                     ← your 48k/24-bit masters (optional; can live outside repo)
│   ├── Waves/
│   └── Instruments/Brass/
├── Samples/                       ← HISE-linked files (often copies or relative paths)
├── SampleMaps/
│   ├── ASR_Wave_Basic.xml
│   └── ASR_Brass_Section.xml
├── XmlPresetBackups/
└── Scripts/
```

Keep **SampleRaw** out of git if files are large; commit **SampleMaps + Scripts + small reference WAVs** only when Marketing approves and licensing is clear.

---

## 5. HISE workflow (Phase 1 — Windows + Antigravity lane)

Prereqs: [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md), HISE built from source.

1. **Create instrument** in HISE; save to `XmlPresetBackups/AsrClass.xml`.
2. **Import audio** — drag WAVs into the sampler or use **File → Import samples**.
3. **Assign zones** — spread on keyboard; set **Root note**, **Low/High key**, **Low/High velocity**.
4. **Loop** — select sample → Sample Editor → loop start/end, crossfade; preview held note.
5. **HLAC (optional for size)** — HISE sample compression for shipping; keep lossless masters elsewhere.
6. **Performance preset** — duplicate sampler modules or use **Layers** / multiple samplers in one preset; map MIDI channels or key switches in HiseScript.
7. **FX (ASR B2 stand-in)** — reverb/chorus on insert until JUCE ESP-class block exists; **document preset names** you used for later parity.
8. **Export** — `export_ci` + `batchCompile.bat` per lane doc; smoke in a DAW.

Official reference: [HISE sampler / sample maps](https://docs.hise.dev/).

---

## 6. From your samples → future JUCE engine (Phase 2)

When the port WO lands, the same content should map to a **ZoneMap** JSON (schema TBD in CMake port). Authoring rules stay the same:

- One zone = root, key range, vel range, file path (or embedded blob), loop points, tune cents.
- Preset = `{ profile: "otto5506", layers: [...] }` for ASR.

Avoid HISE-only features that do not export (exotic mod routing) unless you plan to rebuild them in JUCE.

---

## 7. Quality checklist before you call a map “done”

- [ ] No clipped samples; consistent sample rate across the map
- [ ] Root keys verified (tuner plugin on middle vs edge of each zone)
- [ ] Loops click-free at moderate release times
- [ ] Velocity layers blend without 6 dB jumps
- [ ] Stereo files checked in mono for phase cancel
- [ ] Provenance noted (self-recorded / license name / date) in `docs/brief.md` or spreadsheet

---

## 8. What not to do

- Import **Ensoniq OS disks, ROM waves, or reverse-engineered** factory banks into a product.
- Rely on **MAME ROM zips** or hardware dumps as your library.
- Commit **gigabytes** of WAV to git — use HLAC in project + external backup or LFS policy when SKU is approved.

---

## Related

- [ASR_CLASS_ROMPLER.md](ASR_CLASS_ROMPLER.md) · [EPS_CLASS_ROMPLER.md](EPS_CLASS_ROMPLER.md)
- [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md)
