---
name: "kit-product-spec"
description: >
  Designs a drum kit as a product: concept, hit list with counts, folder tree, file naming,
  device export plan, provenance record and channel notes. Use when starting a new kit, auditing
  a kit's structure, planning exports for FL Studio/MPC/Maschine/MV-1/Lofi12, or preparing a
  listing. Do NOT use for DSP voice design (drum-voice-recipes) or WAV checks (sample-delivery-qa).
version: "1.0.0"
scope: "project"
author: "Sam / Freeman Intelligence"
---

## Must-read constraints

1. Required inputs: style target, BPM range, sample rate, target devices. Missing -> BLOCK.
2. Hit counts below are **[A] starting presets**, not market facts. Confirm with Sam.
3. Device profiles are UNVERIFIED until confirmed from a source or from Sam's working export (disklordz_slicer.py). UNVERIFIED blocks "ready".
4. "Original" needs docs/provenance/<kit>.md. Price, licence wording and publishing are Sam's decisions (HITL); licence text carries the legal disclaimer.

## Process
1. **Concept:** name, style, BPM range, mood, one-line promise a buyer can check in ten seconds.
2. **Hit list** from the preset table; cut categories the style does not use.
3. **Folder tree and names** (below).
4. **Proof per promise:** every promise gets a check (24-bit, peak, in-key, loop length).
5. **Device plan:** per target device, format, folders, naming limits, status CONFIRMED/UNVERIFIED.
6. **Provenance record:** one line per sound category: synthesized / sliced / recorded / mixed, tool, date.
7. **Channel notes:** Beatstars, Gumroad, Etsy, own storefront. Present trade-offs; Sam decides.

## Hit-list presets [A]
| Category | Start count | Notes |
|----------|-------------|-------|
| Kicks | 20 | layer-ready, short and long |
| 808s | 20 | each in a labeled key; pitch checked |
| Snares | 15 | |
| Claps / rims | 15 | |
| Hats (closed/open) | 25 | vary decay |
| Cowbells | 15 | each in a labeled key; ratio locked |
| Percs / FX | 15-20 | |
| Loops (optional) | 10-20 | BPM and bars in the filename |

## Folder tree and naming
```
Disklordz_<KitName>/
  01_Kicks/        Kick_01.wav
  02_808s/         808_F1_01.wav        (key token in name)
  03_Snares/  04_Claps/  05_Hats/  06_Cowbells/  07_Percs_FX/
  08_Loops/        Loop_74bpm_4bar_01.wav   (bpm and bars tokens drive loop-length check)
  README.txt  LICENSE.txt  Cover.jpg
```
Names sort numerically, avoid spaces and special characters (device filename limits are UNVERIFIED per device), and keep the key token on every pitched file.

## Device export plan table
| Device | Format | Folder rule | Status |
|--------|--------|-------------|--------|
| FL Studio | WAV 24/44.1 | any | UNVERIFIED |
| MPC 500 | confirm limits | confirm | UNVERIFIED |
| MPC Sample | confirm | confirm | UNVERIFIED |
| Maschine+ | confirm | confirm | UNVERIFIED |
| Roland MV-1 | confirm | confirm | UNVERIFIED |
| Liven Lofi12 | confirm | confirm | UNVERIFIED |
Fill from a cited source or Sam's disklordz_slicer.py per-device packaging; do not guess.

## Output schema
`kit_spec` with payload: concept, hit_list, folder_tree, naming, checks_per_promise, device_plan, provenance_plan, channel_notes, open_questions. `open_questions` must be empty before a ship claim.

## Known limits
No market data is bundled. Pricing is not recommended here; Sam's earlier plan priced the Iron Cowbell pack at $20-35 and that is his number, not a benchmark.
