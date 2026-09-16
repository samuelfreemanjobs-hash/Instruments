# Disklordz factory — master plan (active)

Single map of the **whole factory** as documented across this repo. Status: **running now** (Square 2+); PM tracks progress in Airtable.

---

## North star

**OG Memphis phonk + gritty trap** one-shots under **Disklordz**, produced by:

1. **Math factory** (`tools/`) — batch WAV + `sample_dna` in SQLite  
2. **Hardware capture** (optional layers) — [DISKLORDZ_CAPTURE_LIST.md](DISKLORDZ_CAPTURE_LIST.md)  
3. **DAW/plugin** (Phase 2) — Disklordz Drum VSTi from `MyFirstSynth`  
4. **Release** — [DISKLORDZ_RUN_SHEET.md](DISKLORDZ_RUN_SHEET.md) Square 3–4  

**Kit v1 codename:** `GRAVEYARD_SHIFT` (OG Memphis phonk).

**Not in factory:** Agent Zero, Ralph, A2UI, autonomous swarms (see blueprint ditch list).

---

## Architecture

```
Discovery (spec) → Synthesis (render_kit + capture) → QC (audit + future LUFS)
       → Package (folders/LICENSE) → Store (manual/Gumroad)
              ↑
       SQLite disklordz.db + PM Agent (Airtable)
```

| Layer | Doc / path |
|--------|------------|
| Brand & sonic targets | [DISKLORDZ_SAMPLE_PACK_BRAND.md](DISKLORDZ_SAMPLE_PACK_BRAND.md) |
| DSP spec (808, SP-1200, cowbell, trap) | [DISKLORDZ_BLUEPRINT.md](DISKLORDZ_BLUEPRINT.md) § DSP |
| 4-Square ops | [DISKLORDZ_RUN_SHEET.md](DISKLORDZ_RUN_SHEET.md) |
| Python implementation | `tools/disklordz/{synthesis,audit,db,references}.py` |
| CLI | `tools/render_kit.py`, `tools/profile_references.py` |
| Schema | `tools/schema.sql` |
| Ops / portfolio | [pm-agent/DISKLOrdZ_AUDIO_PM_AGENT.md](pm-agent/DISKLOrdZ_AUDIO_PM_AGENT.md) |

---

## Phase checklist (factory)

### Phase 1 — Memory & factory (**active**)

| Step | Action | Command / artifact |
|------|--------|-------------------|
| 1.1 | Dependencies | `cd tools && pip install -r requirements.txt` |
| 1.2 | Starter kit | `python render_kit.py --kit-name GRAVEYARD_SHIFT --seed 1` |
| 1.3 | 808 grid for pack body | `python render_kit.py --mode grid808 --count 20 --kit-name GRAVEYARD_SHIFT_808S` |
| 1.4 | Reference DNA (when you have WAVs) | Drop kicks in `tools/references/` → `python profile_references.py references/` |
| 1.5 | Grit pass | `python render_kit.py --kit-name GRAVEYARD_SHIFT --refine-grit` |
| 1.6 | Listen & tweak DSP | Edit `tools/disklordz/synthesis.py` |
| 1.7 | Catalog | `tools/data/disklordz.db` → table `sample_dna` |

### Phase 1 — Square 1 (discovery) for GRAVEYARD_SHIFT

- Genre: OG Memphis phonk, SP-1200/TR-808 knock  
- Target: ~100–150 pieces (build in waves: 6 starter → 20 808s → capture → loops later)  
- QC targets: design −3 to −6 dBFS; master ≤ −0.3 dBFS TP; LUFS ~−14 integrated at release  
- Folders: `01_Kicks` … `06_FX` per brand doc  

### Phase 2 — VSTi (queued)

- Fork `MyFirstSynth` → Disklordz Drum; APVTS per blueprint  
- Align params with `sample_dna` columns  

### Phase 3 — Commerce (later)

- ZIP, Gumroad/BeatStars, scarcity on `product_ledger`  
- Slack: `DISKLORDZ_SLACK_WEBHOOK` on batch complete  

---

## `render_kit.py` modes

| Mode | Use |
|------|-----|
| `starter` (default) | 6 voices: 808 phonk/trap, cowbell, hat, snare, obsidian kick |
| `grid808` | N random 808 phonk variants for pack filler |

---

## QA (today vs next)

| Today (`audit.py`) | Next (`qc_master.py` TBD) |
|--------------------|---------------------------|
| RMS, crest, ZCR, sub peak 38–52 Hz | LUFS-I, true peak, trim, fades |
| `passes_basic_qa`, optional `--refine-grit` | `qc.json` per batch (3361 runbook targets) |

---

## PM linkage

Project name in Airtable: **Disklordz phonk factory & pack v1**  
See [pm-agent/PORTFOLIO_SEED.md](pm-agent/PORTFOLIO_SEED.md) for first-run `PM ADD` payloads and briefing mirror.
