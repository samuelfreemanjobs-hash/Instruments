# SP-1200-style standalone sampler — product spec (v0)

Planning document for a **standalone** performance sampler/sequencer modeled on the **E-mu SP-1200** workflow, UI, and sonic constraints, with deliberate **extended** limits (memory, voices, pads). Implementation target: new product in the Instruments monorepo (likely **JUCE standalone**); not started in code yet.

**Concept UI reference:**

| Screen | Role |
|--------|------|
| Main console | Module grid, LCD/keypad, 16 faders, 16 pads (2×8), banks, transport |
| **MOD 11 — Chop & Truncate** | Waveform editor modal; commit to RAM |
| **MOD 20 — Piano roll** | 16-lane “drum roll” editor + per-step stacks (velocity, tune, pan, filter) |
| **GUI blueprint / spec sheet** | Chassis topology, function directory (01–34), signal flow, BOM (Rev 2.4) |

**Blueprint vs product:** Spec sheet drawings show **8 faders / 8 pads** and **~10 s RAM**; shipping product remains **16×16** and **7:00** (decision log). UI layout **scales 2× vertically**; labels and filter roles follow the same per-channel pattern on CH 9–16.

---

## 1. Product intent

| Goal | Detail |
|------|--------|
| **Feel** | SP-1200 **module + LCD + keypad** workflow; not a general-purpose DAW. |
| **Sound** | **12-bit linear**, **26.040 kHz** working rate; optional **SSM2044-style bus filter** (Module 15). |
| **Performance** | **16 pads** (2×8), **16-voice** polyphony, **SQ-1** as MIDI controller only. |
| **Memory** | **Up to 7 minutes** mono sample time per project (embedded). |
| **Projects** | **Single file** with **embedded** converted sample data + patterns/songs/maps. |
| **Input** | **Audio input** for sampling; **mono sum** on record/import. |

### Extended vs classic SP-1200

| Parameter | Original SP-1200 | This product |
|-----------|------------------|--------------|
| Sample RAM | ~10 s total | **7 min** mono @ 26.04 kHz 12-bit (hard cap per project) |
| Polyphony | 8 voices | **16 voices** |
| Front-panel pads | 8 | **16** (2 rows × 8) |
| Disk | 3.5″ floppy | **Single project file** (“diskette” metaphor in UI) |

Workflow and editing limitations (truncate, combine, assign, tune, decay, pattern/song sequencing) remain **SP-like**. **Module 20** adds a **pattern piano roll** (16 lanes, SP swing, step stacks) — an **extended** editor, not unlimited multi-track DAW timeline. No audio warp / elastic time-stretch.

### UI display constants (all screens)

User-visible copy uses **fixed strings** (never blueprint **10.07 s**, **26.041 kHz**, or **2.5 s free** from concept art):

| Element | Format |
|---------|--------|
| Sample rate | **`26.040 kHz`** (three decimals; also `26.040 kHz // 12-BIT` where compact) |
| Memory total | **`7:00 TOTAL`** or `MEMORY: mm:ss / 7:00` |
| Engine engaged banner | e.g. **`12-BIT 26.040 kHz ENGAGED`** |
| Footer / status | e.g. **`SAMPLE RATE: 26.040 kHz 12-BIT LINEAR PCM`** and **`… / 7:00 TOTAL`** |

Internal DSP sample rate = **26,040 Hz** (same value; UI always shows **26.040 kHz**).

---

## 2. Audio engine

### 2.1 Sample format (internal)

- **Sample rate:** 26,040 Hz (all playback and internal buffers).
- **Bit depth:** 12-bit linear (store efficiently packed; decode to float/int for DSP).
- **Channels:** **Mono** (stereo sources **summed to mono** on import and on **audio input** record).
- **Memory cap:** **420 s (7:00)** mono maximum per project; UI shows used/remaining (e.g. `MEMORY: 2:14 / 7:00 @ 26.040 kHz 12-BIT`).

**Rough storage (planning):** ~157 KB/s mono 12-bit @ 26.04 kHz → ~66 MB for 7 minutes uncompressed packed; project file may use compression for embedded blobs (implementation choice).

### 2.2 Voices

- **16 simultaneous voices**; define **voice-steal** policy (oldest, quietest, same-pad retrigger).
- One pad may retrigger its own voice per SP-style behavior (document in implementation).

### 2.3 Import and sampling paths

Both paths converge to the same internal format:

1. **WAV import** (and common formats): decode → **mono sum** → resample to 26.04 kHz → quantize 12-bit → allocate from **7 min** pool → segment in bank/pad workflow.
2. **Audio input record** (Module 11): device/channel selection, input gain, monitoring → record → same conversion → truncate/assign.

**Vinyl resample trick (UI toggle):** optional workflow to record/import at intentional rate mismatch then tune in Module 12 — not general timestretch.

### 2.4 Pitch playback (phase accumulator)

- **Truncated / drop-sample phase accumulator** for pitch change (not high-quality resampler).
- Transpose by **sample skip/repeat** at 26.04 kHz to produce **SP-style foldover / sideband aliasing** (“metallic” upper partials).
- Range **±12.00 semitones** per pad (Module 12 / 21); finer steps via LCD scrub (**0.1 st** in console mockup).
- **Faders / SQ-1 knobs** in **PITCH TUNE** mode = continuous tuning offsets per channel (spec sheet: sliders 1–8 default to tune; product **1–16**).

### 2.5 Filter and channel topology (Module 15 — v1)

**Spec sheet (per-channel roles, 8-wide pattern → repeat for CH 9–16):**

| Channel | Role (blueprint) | DSP intent |
|---------|------------------|------------|
| **1–2** | **SSM2044** 4-pole LP (VCF analog stage) | Primary **SSM2044** model on main musical voices (e.g. kick/snare) |
| **3–4** | **RC / hi-filter** | High-pass or brightness trim (hat/rim paths) |
| **5** | **CHOKE-1** | Choke group leader (open hat path) |
| **6–8** | **FLAT-PASS** | Minimal shaping |
| **9–16** | *Extended* | Repeat 1–8 filter **roles** on second pad row (not 16 full VCFs) |

- **Master bus** still passes through **SSM2044** stereo mix (piano-roll **SSM FILTER CUTOFF** stack = offset on bus + per-channel sends as designed).
- **AD7541-style** 12-bit DAC characterization on output path (optional saturation curve with 12-bit quantize).

### 2.6 Vinyl resample (33 → 45 RPM)

Document in UI (spec sheet table):

- Record/import at **45 RPM** (or equivalent sample-rate ratio **~+35% pitch**) then tune down in Module 12 to restore musical pitch.
- **Effective bandwidth** follows SP math, e.g. display \(f_\text{eff} = 26040 \times 2^{(\Delta semitones/12)}\) for Nyquist readout (~**16.4 kHz** at stated down-pitch); base rate label remains **26.040 kHz**.
- Harmonic foldback below ~**9.2 kHz** region — **character**, not bug; no pristine warp.

---

## 3. Memory and banks

- **Four banks A–D** (UI from mockup); **7 minutes** divided for display/planning as **1:45 (105 s) per bank** unless user moves segments freely within total cap.
- Segments: start/end, assign to pad, **combine**, **reverse** where SP workflow allows.
- **No** streaming from disk during performance — play from **project memory** only.

---

## 4. UI architecture (from concept art)

### 4.1 Main surface (hardware GUI / chassis blueprint)

**Shell navigation (spec sheet Rev 2.4):** **Specification sheet** | **Hardware GUI** | **Signal flow & arch** | **Sub-assembly matrix** (in-app docs / dev overlays).

**Display:** **16×2 character LCD** (blueprint) — **VF/LCD directory** listing function codes; status line e.g. `BANK A`, `SAMPLE-TRACKING`, segment name (`VINYL CHOP`).

**Function directory (E-MU silkscreen — map to soft keys + numeric entry):**

| Group | Codes (blueprint) | Maps to |
|-------|-------------------|---------|
| **Setup** | Disk, Format, Tape sync, MIDI CH, … | Module **10** |
| **Sample** | ARM, Thresh, Truncate, Loop point, … | Module **11** |
| **Program** | Pitch/tune, Decay, Env, Mix level, … | **12–14**, **21** |
| **Special** | Swing, Time sig, Tempo, Multi-pad, … | **20**, **30** |

(Full **01–34** list fixed at implementation from blueprint; avoid orphan codes without LCD path.)

- **Module select** (soft keys): 10 SETUP, 11 SAMPLE, 12 PITCH, 13 DECAY, 14 MIX, 15 FILTER, 20 SEQ, 24 SONG, 30 SPECIAL.
- **LCD** + **value scrub** + **±1** + **numeric keypad** + **CANCEL/ENTER** + **NO·BACK / YES·EXEC**.
- **3.5″ diskette** metaphor: Save/Load **project**.
- **16 channel fader strips** in **2 rows of 8**, aligned above **16 pads** (2×8).
- **Fader mode:** **VOLUME | PITCH TUNE | DECAY** — sets which parameter all 16 faders (and SQ-1 knobs) edit.
- **Pads:** 2×8, labels user-assignable (defaults may show drum names).
- **BANK A–D**, **MULTI-PITCH**, **MULTI-LEVEL**, **REPEAT**, **AUTO CORRECT**.
- **Transport:** rewind, stop, play, record; **BPM**, **SWING**, **BAR** position.
- **Power / vinyl resample** toggles per mockup.
- **Linear faders:** **60 mm** travel, **VOL / TUN** dual role (same as **fader mode** + spec sheet default **tune on sliders**).
- **Pads:** **logarithmic velocity** response; **exclusive choke groups** (blueprint: e.g. pads **4 & 5** hats, **7 & 8** perc — configurable per project).
- **Multi-pitch mode:** one segment shared across pads; **16 capped pitch steps** (pad *i* → fixed semitone offset from table, SP-style — not a free keyboard).
- **Chromatic SSM (roll):** same cap as multi-pitch — per-step pitch must be one of the **16 defined offsets** for that segment (or ±12 st quantized to multi-pitch slots), not unlimited MIDI range.
- **Timing readout:** **Repeat resolution** (e.g. **1/16 triplets**); **Swing template** e.g. **59% SP-GROOVE**; engine **96 PPQN** with **+12 tick** micro-delay on swung steps (spec sheet “SP-groove logic”).

### 4.2 State machine

Single source of truth drives LCD and keypad:

- `currentModule`, `currentBank`, `selectedPad`, `editField`, `faderMode`, `confirmPending`.

Module button highlights and LCD module name must stay synchronized.

### 4.3 MOD 11 — Sample setup: chop & truncate (modal)

Opened from **11 SAMPLE** (title e.g. `MOD 11 | SAMPLE SETUP: CHOP & TRUNCATE`). Operates on the **current segment** in RAM before assign/commit.

**Header / status**

- Bank, segment id, **26.040 kHz // 12-BIT**, segment length (seconds + sample count).
- **Bank quota display:** e.g. `32.0% BANK A (1:15 REM)` — scales to **105 s/bank** or **global 7:00** pool (implementation: show both total and bank %).
- **Zoom** and **playback head** over selection.

**Waveform**

- Oscilloscope-style view; **START** (yellow) and **END / truncate** (red) markers.
- Draggable markers + numeric fields (sample-accurate at 26.04 kHz).

**Numeric scrub (SP-style steps)**

| Field | Nudge buttons |
|-------|----------------|
| Start offset | −100, −10, +10, +100 (samples or ms — pick one primary, show both) |
| Truncate / end point | same |

**12-bit sample edit operations**

| Action | Behavior |
|--------|----------|
| **Truncate discard** | Drop audio outside START–END; reclaims RAM |
| **Auto-chop (N slices)** | **Transient-based** (see §4.3.1); slice count **8** or **16** |
| **Normalize peak** | To SP headroom rules (no float “hot” master) |
| **Reverse sample** | Reverse current segment |
| **Vinyl resample** | e.g. 33→45 RPM hi-pitch preset; ties to global vinyl trick + Module 12 tune |

**Audition**

- **Pad slice buttons 1–16** (mockup shows 8; product uses **16** aligned with pad rows).
- **Play audition selection** — hear START–END loop.
- **Execute / commit to RAM** — destructive apply; updates memory accounting; closes or returns to Module 11 assign flow.

All commits write **embedded project memory**, not external files.

#### 4.3.1 Auto-chop by transients (required)

Runs **offline** on the current RAM segment (waveform visible); does not use silence-only splitting.

**Algorithm (v1):**

1. **Preprocess:** mono 26.04 kHz buffer; optional gentle HP (~80 Hz) for kick-heavy material; **12-bit quantize** optional pass so chops match commit sound.
2. **Onset detection:** **spectral flux** or **energy-derivative** transients on fixed STFT windows (e.g. 512–1024 samples); merge peaks closer than **min gap** (default **~30–80 ms**, user-adjustable in MOD 11).
3. **Slice count:** user picks **8** or **16** — select **N−1** strongest onsets between START and END, or **auto-N** if fewer transients (pad remaining slices empty).
4. **Boundaries:** forced **START** at first marker; **END** at last; internal cuts at transient indices (sample-aligned).
5. **Preview:** map slices to **audition pads 1–N**; **Play audition** per slice.
6. **Commit:** **Execute / commit to RAM** creates **N new segments** (or one multi-slice assignment table if “single segment multi-pad” mode — default **N segments** for SP-style memory accounting).

**Reject** if segment shorter than **N × min slice length**; show LCD-style error, no commit.

**Module 11 sampling (ARM):** auto-threshold **−48 dB to −6 dB** or manual; **VU on LCD** during arm/record (spec sheet).

### 4.5 Signal flow (spec sheet)

```text
Input preamp → 12-bit ADC → DRAM (project pool, 7:00 max)
  → per-voice phase (drop-sample) → channel shaper (SSM / RC / choke / flat)
  → SSM2044 bus → stereo out (×16 direct outs optional future)
```

Display header: **`12-BIT 26.040 kHz ENGAGED`**; memory line: **`MEMORY: … / 7:00 TOTAL`** (see **UI display constants**).

### 4.4 MOD 20 — Piano roll (“12-bit drum roll engine”)

Full-screen or tab view (mockup tabs: **10 CONSOLE | 11 WAVE CHOP | MOD 20 PIANO ROLL | 24 SONG | SSM2044 VCF**).

**Scope:** One **pattern** at a time; **16 lanes** fixed (lane *i* ↔ pad/voice *i* ↔ sample assignment). Not arbitrary track count.

**Transport / header**

- RTZ, Play, Stop, **Rec overdub**, click on/off.
- LCD block: pattern name, BPM, bar (e.g. 02/04), **SWING % (SP)**, quantize **1/16 @ 96 PPQN** (or SP-equivalent grid).
- Knobs: **Tempo**, **SP-Swing**, **Analog gain** (bus trim pre-SSM2044).

**Edit tools**

- Tool palette: pencil, eraser, select (minimal set).
- **Snap:** 1/4, 1/8, **1/16**, 1/16T, 1/32.
- **Map mode:** **SP drum map** (lanes = pads, pitch fixed per lane) vs **Chromatic SSM** (**capped like multi-pitch** — see below).

**Grid**

- Horizontal: bars/beats/substeps for active pattern length.
- Vertical: 16 rows with sample names (e.g. `01 BD`, … `16 RH`).
- Notes as blocks; optional pitch label in chromatic mode.

**Per-step stacks (bottom panel)**

Toggle one lane at a time or show active lane; modes mirror SP performance params:

| Stack | Range / notes |
|-------|----------------|
| **Velocity (hit)** | Per-step velocity |
| **Pitch tune** | ±12 semitones (finer in Module 12) |
| **Pan (L/R)** | Per-step pan ( stereo output; mono samples ) |
| **SSM filter cutoff** | Per-step bus filter offset or send (keep DSP on bus model) |

**Footer status**

- `MEMORY: mm:ss / 7:00 TOTAL` (remaining or used per layout).
- `SAMPLE RATE: 26.040 kHz 12-BIT LINEAR PCM`.
- Choke groups (e.g. **EXCL 1** open/closed hat).
- MIDI sync: internal / external.

**Relation to classic SP:** Real SP-1200 uses **step entry on pads**, not a piano roll. Module 20 is the **extended** visual editor; **pad record + SQ-1** must still write the same underlying pattern data as pencil entry.

---

## 5. Control — unified action map

All inputs invoke the same **named actions**:

- Modules, transport, banks, pads 1–16, faders 1–16, enter/cancel, yes/exec.

### 5.1 Computer keyboard (defaults — remappable in SETUP)

| Action | Default |
|--------|---------|
| Play | Space |
| Stop | Escape |
| Record | R |
| Rewind | Home |
| Bank A–D | 1–4 |
| Pads 1–16 | Two rows (e.g. Q-P / A-;) — final layout TBD in implementation |
| Enter / Cancel | Enter / Backspace |
| Fader mode Volume / Pitch / Decay | Cycle key (e.g. Tab) or dedicated keys |

Full table lives in implementation config; **MIDI Learn** and **key Learn** in Module 10.

### 5.2 Korg SQ-1 (MIDI controller only)

**Pads**

| SQ-1 | MIDI (default) | Action |
|------|----------------|--------|
| Row A buttons 1–8 | Ch 10, notes 36–43 | Pad 1–8 |
| Row B buttons 1–8 | Ch 10, notes 44–51 | Pad 9–16 |

**Knobs above buttons → GUI faders**

| SQ-1 | MIDI (default) | Action |
|------|----------------|--------|
| Knobs above row A (cols 1–8) | Ch 10, CC 20–27 | Fader 1–8 (follows **fader mode**) |
| Knobs above row B (cols 1–8) | Ch 10, CC 28–35 | Fader 9–16 (follows **fader mode**) |

**Hardware note:** Stock SQ-1 may not expose 16 distinct encoders in all modes; **MIDI Learn per fader** is required. Document fallback (8 CCs + fader page / shift) if only eight continuous sources are available.

**Transport / mode:** Available on **computer keyboard** and **SQ-1** where mappable (notes/CC on second channel or shift layer); SQ-1 does not replace on-screen keypad for numeric entry.

### 5.3 GUI

Mouse/touch on pads, faders, modules, keypad — same actions as hardware maps.

---

## 6. Sequencer and sync

### 6.1 Patterns (Module 20)

- **Pattern length:** user **1–4 bars** per pattern (default **2**).
- **Pattern count:** **multiple patterns** (storage for **99** like SP; UI may page in batches).
- **Grid:** **16 lanes** (pads), **1/16** steps → `bars × 16` steps per pattern.
- **Timing:** **96 PPQN**, **SP swing %**; pad record, overdub, and piano roll share **one pattern struct**.

### 6.2 Song (Module 24)

- **Song arrangement screen:** ordered list of **pattern numbers** (with repeats TBD); playback runs patterns sequentially then loops song or stops (user option).
- Same data embedded in project file as patterns.

### 6.3 Other

- **Console mode:** step record via **16 pads** without opening piano roll.
- **MIDI clock:** in/out; SQ-1 can master or slave per SETUP.
- Pattern/song data stored inside project file (steps + optional micro-timing within quantize).

---

## 7. Project file format (single embedded bundle)

Working name: `.sp12p` (or product-specific extension).

| Section | Contents |
|---------|----------|
| Header | Format version, product id, total memory used |
| Global | BPM, swing, fader mode, last module (optional) |
| Memory | Segments: packed 12-bit @ 26.04 kHz mono, trim points, bank |
| Assignments | Pad → segment, tune, decay, mix level, multi-pitch/level |
| Patterns / songs | Sequencer state |
| MIDI maps | Optional embedded SQ-1/default maps |

**No** external sample paths required for normal save/open. Optional **export WAV** (extended utility) does not replace embedded storage.

---

## 8. Module reference (behavioral)

| # | Name | Scope |
|---|------|--------|
| 10 | SETUP | Audio/MIDI devices, MIDI Learn, clock, keyboard map, mono input |
| 11 | SAMPLE / WAVE CHOP | ARM/thresh/VU; record/import; **chop & truncate** (§4.3); **transient auto-chop**; assign |
| 12 | TRUNC / LOOP | Start/end/loop in **byte/sample increments** (pairs with §4.3) |
| 12–21 | PITCH / PROGRAM | Per-pad tune ±12 st; **phase accumulator** playback (§2.4) |
| 13 | DECAY | Per-pad decay |
| 14 | MIX | Per-pad levels (faders in VOLUME mode) |
| 15 | FILTER | Bus SSM2044 |
| 20 | SEQ / PIANO ROLL | Patterns; 16-lane roll + stacks (§4.4); pad overdub |
| 24 | SONG | Arrangement |
| 30 | SPECIAL | Combine, utilities, vinyl-related tools |

---

## 9. Implementation phases

| Phase | Deliverable |
|-------|-------------|
| **P0** | Engine: 26.04 kHz / 12-bit / 16 voices / 7 min pool; WAV + input record; mono sum; project save/load |
| **P1** | Main UI shell; 16 pads + 16 faders; keyboard + SQ-1 default maps; transport; Module 11/12/14 basics |
| **P2** | MOD 11 chop/truncate modal; pattern data model; console step record |
| **P2b** | MOD 20 piano roll + per-step stacks; SONG tab; choke groups |
| **P3** | Module 15 filter + SSM tab; Module 30; polish LCD/keypad state machine |
| **P4** | QA against SP workflow checklist; optional plugin export (non-goal for v1 unless requested) |

When code starts: add product `ARCHITECTURE.md` and index row in root `/ARCHITECTURE.md`.

---

## 10. Open items (non-blocking)

- Voice-steal algorithm details.
- Project compression codec for embedded samples.
- Exact SP-1200 module menu parity checklist (pattern count, song steps).
- Product name and branding (avoid unlicensed “E-mu” in shipping SKU unless licensed).
- Linux support vs macOS/Windows v1.

---

## 11. Decision log

| Date | Decision |
|------|----------|
| — | Standalone; SP workflow; WAV + **audio input** |
| — | **16 voices**, **7 min** mono, **16 pads** (2×8), **16 faders** (2×8) |
| — | SQ-1: **16 buttons → pads**, **knobs → faders** (mode = VOL/PITCH/DECAY) |
| — | Transport/mode: **keyboard + SQ-1**; SQ-1 MIDI only |
| — | **Mono sum**; **filter in v1** (bus SSM2044) |
| — | **Single project** with **embedded** samples |
| — | **MOD 11** chop/truncate + **MOD 20** piano roll (16 lanes) in scope |
| — | **Auto-chop by transients** (8 or 16 slices), not silence-only |
| — | GUI **blueprint / spec sheet** Rev 2.4: directory codes, signal flow, 96 PPQN SP-groove |
| — | All UI: **26.040 kHz** and **7:00 TOTAL** memory (not art-sheet 10.07 s / 26.041) |
| — | **Auto-chop: transient-based** only |
| — | **Chromatic capped** like **multi-pitch** (16 pitch slots) |
| — | Patterns **1–4 bars**, **99** slots; **Module 24 song** arrangement screen |

---

## 12. Concept art → spec traceability

| UI element (mockup) | Spec section |
|---------------------|--------------|
| START/END waveform, truncate discard | §4.3 |
| Auto-chop | §4.3.1 **transient** detection; 8 or 16 slices |
| 8 faders / SSM CH1–2 | §2.5, §4.1 → **16** channels, repeated topology |
| Function directory 01–34 | §4.1 |
| Phase drop pitch / aliasing | §2.4 |
| ARM threshold, VU | §4.3.1, Module 11 |
| 10.07 s memory (sheet) | **7:00** product cap |
| Choke groups, multi-pitch | §4.1 |
| 59% SP-groove, 96 PPQN | §4.1, §4.4, §6 |
| Audition pads, commit to RAM | §4.3 |
| Vinyl resample 33→45 | §4.3, §2.3 |
| 16 track list + piano grid | §4.4 |
| SP swing, 1/16 quant, 96 PPQN | §4.4, §6 |
| Velocity / pitch / pan / filter stacks | §4.4 |
| SP drum map vs chromatic | §4.4 |
| Buffer 2.5 s free (art) | **7:00** total per decision log |

---

## Related

- Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md)
- JUCE factory: [docs/ARCHITECTURE.md](ARCHITECTURE.md), [Wave909/ARCHITECTURE.md](../Wave909/ARCHITECTURE.md)
- HISE sketch lane (optional prototype): [docs/HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md)
