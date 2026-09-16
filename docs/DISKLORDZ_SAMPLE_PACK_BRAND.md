# Disklordz sample pack brand (from chat transcript)

Source: *Building the Disklordz Sample Pack Brand* chat. Everything else from January (swarms, Ralph, A2UI) stays **out**—see [DISKLORDZ_BLUEPRINT.md](DISKLORDZ_BLUEPRINT.md).

---

## Keep — this is the brand

### Identity

| Field | Decision |
|--------|----------|
| **Name** | **Disklordz** |
| **Lane** | **OG Memphis phonk** + **hard, gritty trap** |
| **Hero sound** | TR-808 character through **12-bit** (SP-1200 / MPC class): knock, hiss, aliasing, tape floor |
| **Audience** | Producers who want **hardware soul**, not polished digital-only packs |
| **Tone** | Underground, authoritative, crate-digger—no corporate marketing speak |

### Sonic blueprint (what to design toward)

- **Kicks:** Short, boxy punch; saturation on tail; “Memphis thump” (DR-660 style clipping optional on capture kits).
- **808s:** Trap = short/punchy; phonk = long sustains + glides; **parallel grit** so sub stays felt while mids scream.
- **Snares/claps:** Layered, bit-crushed, Tommy Wright–style bark on OG kits.
- **Cowbells:** 808 cowbell; tuned (e.g. to C); freq shift + crush for phonk leads.
- **Hats:** Sizzle + sample-rate reduction / metallic sampler texture.
- **Textures:** Tape hiss, vinyl floor, cassette noise beds (folder of FX, not every kit needs vocals).

### Technical delivery

| Rule | Notes |
|------|--------|
| **Format** | **24-bit WAV**, 44.1 kHz (48 kHz OK if consistent) |
| **One-shots** | Peaks around **−3 to −6 dBFS** while designing; **master** for sale at **≤ −0.3 dBFS true peak** (run sheet / future QC) |
| **Curation** | **Quality > count**; ~**100–150** pieces is a solid *pro* phonk/trap kit, not 500 fillers |
| **Folders** | `01_Kicks`, `02_808s` (key in filename), `03_Snares_Claps`, `04_Perc`, `05_Loops` (140–160 BPM if included), `06_FX` |
| **Legal** | **100% original**; no re-selling processed third-party kit audio; ship **LICENSE.txt** (royalty-free for beats, no redistribution) |

### Two production paths (both valid)

1. **Hardware capture** — your gear list + Sector 01 checklist → [DISKLORDZ_CAPTURE_LIST.md](DISKLORDZ_CAPTURE_LIST.md)  
2. **Math factory** — `tools/render_kit.py` for SP-1200-ish 808s, cowbells, hats (same DNA in SQLite)

Flagship kits can combine: **synthesized core + hardware-texture layers** you record.

### OG processing recipes (keep)

**45 RPM resample trick**

1. Pitch hit up +7 to +12 semitones  
2. Bitcrush **12-bit / ~26 kHz** (SP-1200)  
3. Bounce  
4. Pitch back down for aliasing / dust  

**VST chain ideas** (when not using outboard): RX1200 / Decimort, RC-20 / tape, Decapitator / Saturn for drive.

### Go-to-market (keep, manual)

| Tactic | Use |
|--------|-----|
| **Teaser / lite kit** | 10–15 sounds for email list |
| **Demo track** | ~1 min, **only** this kit’s sounds |
| **Cover** | Strong visual—phonk often buys with eyes; **pick one line:** Xerox/cassette B&W *or* obsidian/gold, not both in one pack |
| **Channels** | Start **Gumroad/Shopify** (margin) or **BeatStars**; Splice/Loopmasters/ADSR only if you want marketplace reach (they take a cut) |

### First kit name ideas (from chat)

- **GRAVEYARD SHIFT** — OG Memphis  
- **BLOODLINE 808s** — 808-focused trap  
- **CRYPTID: Raw Voltage** — modern industrial trap  

Pick one for Sector / factory v1.

### Brand prompt for other AI (trimmed)

Save as `docs/DISKLORDZ_BRAND_PROMPT.txt` or paste into Cursor:

```text
You are a brand/creative partner for Disklordz—underground Memphis phonk and gritty trap sample packs.

Identity: Raw, OG, 12-bit crunch, SP-1200/MPC-style 808s, tape hiss, aggressive knock.
Audience: Producers who want hardware character over sterile digital kits.
Visual: High-contrast, 90s cassette/underground OR minimal obsidian (match the kit).
Voice: Crate-digger, studio veteran—no fluff.

Task: [INSERT TASK]
```

---

## Ditch — from this transcript (don’t operationalize)

| Chat suggestion | Why ditch / defer |
|-----------------|-------------------|
| “AI as production team” that makes kits for you | **You** capture / approve; Cursor helps spec, code, copy—not autonomous packs |
| **Memphis vocal stabs** in v1 | Different legal/source workflow; add only with clear rights |
| **Loops as required** for kit v1 | Optional folder; phonk one-shots often sell without loops |
| **Marketplace-first** (Splice etc.) | Optional; not the default ops model |
| **Conflicting visual briefs** (Xerox B&W vs luxury obsidian) | Choose per **kit line**, not one swarm UI |
| Generic “how to start a sample business” without Disklordz lane | Already decided: phonk/trap + Disklordz |
| Listing every VST by name as **requirements** | Recommendations only; use what you own |

---

## Repo hooks

| Doc / tool | Role |
|------------|------|
| [DISKLORDZ_RUN_SHEET.md](DISKLORDZ_RUN_SHEET.md) | Release checklist |
| [DISKLORDZ_CAPTURE_LIST.md](DISKLORDZ_CAPTURE_LIST.md) | Hardware session checklist |
| `tools/render_kit.py` | Synthetic Sector parallel |
| Disklordz Drum VSTi (planned) | Playable same DNA in DAW |

---

## Your lane (filled from transcript)

- **Primary genre:** OG Memphis phonk + hard gritty trap  
- **First kit codename:** _pick: GRAVEYARD SHIFT / BLOODLINE 808s / CRYPTID_  
- **Price band:** _you set_  
- **Store:** _Gumroad / BeatStars / Shopify_
