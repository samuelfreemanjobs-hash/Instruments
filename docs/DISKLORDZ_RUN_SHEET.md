# Disklordz project run sheet

Copy per release. Run each square in **Cursor** (or Cloud Agent)—not Ralph, not Agent Zero.

**Kit name:** `________________`  
**Genre / mood:** `________________`  
**Target:** `Drum kit` | `Plugin preset` | `Hybrid`

---

## Square 1 — Discovery

- [ ] Pick lane (competitor kit + 3 reference tracks you want to sit next to)
- [ ] Optional: one row in `market_gaps` or a note in repo
- [ ] Write `spec.json` (or bullet list): BPM range, # of 808s/kicks/snares/hats, LUFS target (−14 integrated), true peak (−0.3 dBFS)

**Cursor prompt:**

> Read `docs/DISKLORDZ_BLUEPRINT.md`. I’m making kit [NAME] for [GENRE]. Draft a one-page spec: sample counts, folder tree, QC targets, and 5 adjectives for the sound.

---

## Square 2 — Synthesis

- [ ] `cd tools && pip install -r requirements.txt`
- [ ] `python render_kit.py --kit-name [NAME]` (or `--mode grid808 --count N`)
- [ ] Optional: `python profile_references.py references/` then re-render for match scores
- [ ] Listen in DAW; tweak `tools/disklordz/synthesis.py` or VSTi params

**Cursor prompt:**

> Adjust [808 phonk | trap | cowbell] in `synthesis.py` to match spec: [paste spec].

---

## Square 3 — Refinement / QC

- [ ] Peak / clipping check (no digital clip)
- [ ] HPF ~20 Hz on subs if needed (DAW or future `qc_master.py`)
- [ ] Fades on one-shots (3–15 ms) if tails click
- [ ] Log metrics: RMS, crest, LUFS (when tooling exists)
- [ ] Reject/regenerate outliers; keep `sample_dna` as source of truth

**Cursor prompt:**

> Propose offline QC steps for these WAVs matching true peak −0.3 dBFS and integrated LUFS −14.

---

## Square 4 — Storefront

- [ ] Folder layout + `README.txt` + `LICENSE.txt`
- [ ] Cover art (static image—no A2UI required)
- [ ] Listing title, 5 bullets, price, scarcity note if any
- [ ] ZIP: `[KIT]_v1.0.zip`
- [ ] Optional: `DISKLORDZ_SLACK_WEBHOOK` on final batch

**Cursor prompt:**

> Write Gumroad-style listing copy for [KIT]. Tone: producer-native, phonk/trap. Include specs line from our QC targets.

---

## Sign-off

- [ ] Human listened to full kit  
- [ ] ZIP uploaded  
- [ ] `product_ledger` updated manually when live (optional)
