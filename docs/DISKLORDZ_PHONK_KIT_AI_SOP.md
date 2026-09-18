# Disklordz — AI phonk drum kit SOP (living document)

**Status:** Basic SOP for launch and day-to-day kit work. Refine as you learn; do not treat vendor marketing as gospel.  
**Audience:** You, Cursor agents, capture sessions, Marketing.  
**Related:** [DISKLORDZ_MASTER_LAUNCH_CANVAS.md](DISKLORDZ_MASTER_LAUNCH_CANVAS.md) · [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) · [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) · [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md) · [disklordz/sound-factory/README.md](../disklordz/sound-factory/README.md)

---

## 1. What “we do” in one paragraph

Disklordz sells **underground phonk / Memphis / drift** drums and related SKUs (sample packs, plugins, funnels). Kits are built on **three rails** that can combine in one product:

| Rail | Role | Where it lives |
|------|------|----------------|
| **A — In-house factory** | Prompt + preset → WAV + manifest + ZIP (fast, automatable, your IP) | `disklordz/website/src/lib/generation/` · `sound-factory/` |
| **B — Hardware + SP-1200 chain** | Soul, aliasing, tape; slower, highest perceived value | Capture list / Sector 01 (historical docs in git) |
| **C — External AI tools** | One-shots, loops, stem theft recovery, inspiration | Emergent Drums 2, Moises, Udio, etc. (see §4) |

**Default SOP:** Use **A** for volume and SaaS; use **B** for flagship packs (e.g. **Memphis Vault** lineage, codename **Graveyard Shift**); use **C** when A/B need a specific texture you cannot parametrically hit yet—**always export WAV, document provenance, 100% redistribution rights**.

---

## 2. Standard kit spec (every pack)

| Rule | Value |
|------|--------|
| Format | 24-bit WAV, 44.1 kHz (one rate per pack) |
| Folders | `01_Kicks`, `02_808s` (note key in filename), `03_Snares_Claps`, `04_Perc`, `05_Loops` (optional, label BPM), `06_FX` |
| Naming | `DISKLORDZ_<KIT>_<TYPE>_<NNN>.wav` or `BPM_KEY_Name.wav` for chops |
| Size | **Quality > count** — ~100–150 pieces for a pro phonk/trap kit |
| Legal | Original or licensed; **LICENSE.txt** — royalty-free for beats, no raw redistribution |
| QC | Peaks ~−3 to −6 dBFS while designing; master for sale ≤ −0.3 dBFS true peak |

**Memphis power words for prompts (factory + external):** saturated, bit-crushed, dusty, tape-warped, lo-fi, clipped · 808 glide, Memphis cowbell, 909 snare · dark, aggressive, hypnotic, underground.

---

## 3. SOP — Build a phonk kit (repeatable)

### Phase 0 — Pick lane and SKU name

- **Lane:** OG Memphis (80–100 BPM slump) vs drift phonk (140–160 BPM).
- **Name:** Align with launch canvas (e.g. **Memphis Vault Vol. 2**, SaaS preset **Midnight Circuit** / DL002).
- Log kit id in Airtable / WO title.

### Phase 1 — Generate core one-shots

**Option A — Product factory (preferred for automation)**

1. Open SaaS `/` or call `POST /api/generate` with preset + spec ([ILLUGEN-shaped body](DISKLORDZ_ILLUGEN_RESEARCH.md)).
2. Batch variations; pick best 6–12 hits per category.
3. Export ZIP via `/api/download`; keep `manifest.json` (SHA-256 provenance).

**Option B — Emergent Drums 2 (or similar neural drum plugin)**

1. 16-pad layout; **Crunchy** model for phonk.
2. Per pad: Kick / Snare / Perc (cowbell) — Generate → clip/muffle highs for tape feel.
3. **Similarity / variation slider** on a reference hit → 10–50 cousins (analog spread).
4. Drag WAVs to `01_`–`04_` folders; rename to Disklordz convention.

**Option C — Synth raw → SP-1200 chain (Memphis ring)**

1. Generate or draw clean 808, snare, cowbell (Serum/Vital square cowbell + bandpass, or ED2).
2. **Resample hack:** pitch **+7 to +12 st** → run **RX1200 / S12X / Amigo** at 12-bit → pitch **down** same amount.
3. Export one-shots; optional parallel grit on 808 tail (bit-crush 12-bit, not 8).

### Phase 2 — Loops and bounce (optional)

- **Do not** ship full copyrighted loops from Suno/Udio as kit loops unless terms allow strip + royalty-free export.
- **Do use** Suno/Udio/BandLab for **reference bounce**, then recreate hits in A/B or MIDI → one-shots only in the pack.
- Prompt example (reference only): *160 BPM drift phonk drum loop, aggressive distorted 808, rapid hats, Memphis cowbell, lo-fi grit, no melody.*

### Phase 3 — Stem isolation (recovery, not primary source)

1. Upload track to **Moises / LALAL.AI** → **Drums** stem.
2. Slice in DAW; **only use if legally yours or public-domain source**.
3. Prefer re-synthesizing similar timbre via factory or ED2 instead of redistributing isolated hits from commercial tracks.

### Phase 4 — FX and vocal chops

See §5. Place in `06_FX` or separate **Freebie** stem pack (launch canvas vocal pipeline).

### Phase 5 — Glue and master

- Bus: light tape (SketchCassette / RC-20 class), **12-bit** crush on aux, slight clip on kicks/808.
- One demo beat: **only this kit’s sounds**, ~60 s, for Marketing.

### Phase 6 — Package and store

1. ZIP + README + LICENSE + cover (phonk buys with eyes — match **Memphis Vault** cassette lane).
2. Upload to R2 / Gumroad / Stripe product; set env URLs on `/launch` if part of funnel.
3. Optional: stub kit in `disklordz/launch-assets/<kit-id>/`.

---

## 4. External tool reference (when to use)

| Goal | Tool | Use Disklordz when… |
|------|------|---------------------|
| Unique one-shots | **Emergent Drums 2** | Factory snares/kicks not gritty enough; budget for ~$49–249 plugin; OK with online generation |
| SP-1200 character | **RX1200, S12X, SP950, Amigo** | Every Memphis/O G pack after raw sound exists |
| Full track / chop reference | **Udio** | Need muffled 90s vocal *reference*; materialize as licensed chops (§5) |
| Controlled loops / 808 slides | **Soundraw** phonk mode | Reference bounce; export policy checked |
| Fast meme phonk | **Fotor / BandLab SongStarter** | Teasers only; not flagship SKU source |
| Stem split | **Moises, LALAL.AI** | Your beats, PD archives, or licensed sources only |

**Anti-patterns:** Building the whole business on ED2 subscriptions + manual drag-drop; scraping commercial phonk without rights; 8-bit crush everywhere (too chiptune for Memphis).

---

## 5. Vocal chops & street FX — ideas + materialization

### Idea bank (OG Memphis / phonk FX)

| Type | Examples (generate or source legally) |
|------|----------------------------------------|
| Stabs | Short “yeah”, “what”, “hold up”, pitched −3 to −7 st, bandpass 300 Hz–3.5 kHz |
| Menace | Whispered threats, countdowns, “ride out”, lo-fi phone filter |
| Cult / horror | Reverse reverb one-shots, graveyard ambience (field record your own) |
| Memphis meta | “Smokin’”, “break yourself”, sermon fragments — **public domain or original recordings only** |
| Movie dialogue | Only if **license allows** sample pack use; default: **skip** for v1 kits |
| Sirens / FX | Synthetic police siren (Serum noise + pitch); gunshot = layered clap + noise burst (original) |

### Materialization pipeline (matches launch canvas)

1. **Source:** Public-domain sermons, mixtapes you own, or **original VO** (record 20 lines in one session).
2. **Isolate:** Demucs v4 / UVR MDX-Net if stems needed.
3. **Tune:** Lock to pack key (F♯ min, C♯ min, A min common).
4. **Edit:** Zero-cross fades; de-click; optional Cassette Smash / tape chain.
5. **Deliver:** `BPM_KEY_VocalChop_Name.wav` in ZIP; freebie gate on `/launch/vocals`.

---

## 6. Should we build our own “Emergent Drums 2”?

### Short answer

**Yes as a phased product-factory engine—not as a $249 VST clone on day one.** Your moat is **prompt → SKU → ZIP → funnel → ledger**, not winning Audialab on neural timbre in v1.

### What ED2 actually is

- Cloud-backed **neural drum generator** + 16-pad UX + variation from reference samples.
- Heavy R&D: model training/hosting, latency, GPU cost, plugin certification.

### What Disklordz already has (better fit for *your* business)

| Capability | Today | ED2-like upgrade |
|------------|--------|------------------|
| Text → drums | Parametric synth + prompt params | Hosted audio model (ILLUGEN § gateway) |
| Variations | 2–3 per batch | N variations + “generate more” |
| Provenance | SHA-256 in manifest | Same + model id / seed |
| Automation | API routes, credits, Stripe | Batch jobs, Airtable WOs |
| Memphis character | Presets + SP-1200 post chain | Train/fine-tune on **your** RX1200 exports |

### Recommended roadmap

| Phase | Build | Outcome |
|-------|--------|---------|
| **P0 (now)** | Factory + SP-1200 post template + folder pack export | Ship kits weekly; **Memphis Vault** sequels |
| **P1** | Async job queue + batch API; RAG prompt suggest (`/api/rag/suggest`) | “10 kits overnight” without ED2 |
| **P2** | Reference-sample **variation** (DSP + small model or licensed API)—*not full ED2* | Cowbell families from one holy grail |
| **P3** | Optional **JUCE pad plugin** that calls **your** generation API | Brand plugin; you own stack |
| **Skip for now** | Training foundation model from scratch | Poor ROI vs P1–P2 |

**Verdict for product factory:** Building **your** generator is a **good move** if scoped as **factory automation + Memphis DNA**, not “clone ED2 UI.” ED2 remains a **valid Phase 1B tool** for hand-curated flagship packs.

---

## 7. Marketing handoff — “Do we have a good team?”

Use this section verbatim in a Marketing sync.

### What we sell (2026 stack)

1. **Legacy catalog** — **Memphis Vault Vol. 1** (phonk drums; was **Graveyard Shift** in planning).
2. **Launch funnel** — free vocal vault → tripwire plugin/IR → flagship tape FX (`/launch`).
3. **SaaS factory** — prompt → preview → ZIP (growth SKU, Stripe Pro path).
4. **Future plugins** — Cassette Smash, Memphis Tape FX (Track E).

### What is already built (engineering truth)

| Asset | Status |
|-------|--------|
| Next.js app + deploy path | Shipped |
| Parametric kit generation + manifest/ZIP | Shipped |
| Auth, saved kits, credits, Stripe webhook | Shipped (needs prod env) |
| Launch soundboard + opt-in + thank-you | Shipped (PR #33) |
| RAG prompt assist | Shipped (keyword v0) |
| Email 4-day sequence, MoR checkout, R2 entitlements | Planned / stub |
| Flagship WAV packs on CDN | Waiting on asset upload |

### Agent / human lanes (from [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md))

| Role | Owns |
|------|------|
| **Cursor Cloud** | Website, factory API, launch pages, docs |
| **Marketing** | Copy, teasers, demo beats, social scripts, cover art briefs |
| **Business Planner** | Env secrets, merge gates, SKU pricing |
| **A&R / artists** | Preset lanes DL001/002/004/006, reference sessions |
| **Antigravity** | HISE / plugin sketches (parallel) |
| **You** | Final ears, legal on samples, brand calls |

**Team assessment:** You have a **credible split**—engineering and factory MVP are real; Marketing is the **critical path** for launch week (demo video, Kern emails, tripwire pages with real URLs). Missing pieces are **ops** (email + paid delivery), not “can we make sounds.”

### Ask Marketing to deliver (30-day)

1. One **60 s demo beat** per SKU (Vault Vol. 1 refresh, Cassette Smash A/B, free stem tease).
2. Frank Kern **4-day email** copy pasted into Resend/Loops (or our DB sequence later).
3. Storefront URLs for `LAUNCH_*` and **Memphis Vault** bundle story.
4. TikTok/Reels script from launch canvas (3-second transformation hook).

### One-line positioning for campaigns

> **Disklordz** — hardware-soul phonk drums and tape destruction: try the effect in the browser, grab the stems, own the same 12-bit knock we use in the factory.

---

## 8. Changelog

| Date | Change |
|------|--------|
| 2026-09-17 | Initial SOP from launch + AI kit research thread; linked to factory and launch canvas |

**Next refinements:** Add QC checklist JSON; link ED2 export folder template; Supabase `kit_catalog` when commerce module lands.
