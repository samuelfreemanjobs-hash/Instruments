# Research: DJ Paul / Juicy J snare character + Memphis phonk groove (YouTube ear refs)

**Question:** What snare / drum qualities define DJ Paul and Juicy J’s Memphis sound, and what groove/MIDI patterns do top Memphis phonk YouTube mixes imply for Black Box snares (pitched-down + snap)?  
**Success:** Tagged claims; YouTube mix shortlist; original MIDI groove sketches (not tape clones); DSP knobs for “pitched down + snap.”  
**Non-goals:** Cloning copyrighted samples; artist-named storefront SKUs; claiming exact MIDI of any release.

**Verdict:** Paul-lane snares read as **hollow, pitched-down membrane + delayed clap/flam snap** over sparse half-time grids; Juicy-lane bounce keeps the same Memphis half-time pocket but with **thicker body punch and slightly busier tops**. Weakest tag on the verdict: **[assumed]** for exact Hz/st numbers (technique translation); historical gear/scene claims **[inspected]** from prior CNV research + secondary production guides.

---

## Findings

| Claim | Tag | Source |
|-------|-----|--------|
| Paul / Juicy used SP-1200 / W-30 / MPC orbit; Paul flanger on vocal samples; cassette chain common | inspected | CNV `RESEARCH_MEMPHIS_TOP20.md` → Loaded Samples 90s guide (secondary) |
| Internal **Horror Snap** = Paul-era energy; **Club Bounce** = Juicy/Three 6 club energy | inspected | `recipes/MEMPHIS_ARCHETYPES.md` |
| Acoustic triad: membrane 180–350 Hz, wires BP 1.2–5.5 kHz, flam 2–8 ms | inspected | `docs/GEM_MEMPHIS_DR660_SNARE.md` |
| “Pitched down + snap” for Paul/Juicy ear: lower membrane (~145–196 Hz) + short wire snap + 4–6 ms flam | **assumed** | Technique synthesis from GEM + archetype + listener request |
| Phonk is 2010s revival of 1990s Memphis aesthetics | inspected | Wikipedia Phonk (via prior research) |
| YouTube dark Memphis mixes feature horrorcore/Triple Six DNA, often half-time menace | inspected | Video titles/snippets (Weedim mix, underground Memphis mixes) |
| Exact per-track snare MIDI from named tapes | **assumed / not measured** | No audio transcription this turn |

### Conflict / dissent

- **Vendor kits vs journalism:** YouTube “DJ Paul Type Beat” / drum-kit demos are marketing ear candy — useful for *feel*, not primary production history. Prefer HoF/RBMA/Flyer for *who*; GEM + archetypes for *DSP numbers*.  
- **Drift phonk vs 90s Memphis:** Many “Memphis phonk mix” uploads run 130–160 BPM cowbell gym energy; classic Paul/Juicy pocket is closer to **~90–98 crawl or 130–140 as half-time**. Use both grids; don’t force modern trap rolls.

---

## YouTube ear shortlist (groove / mix references)

Use for **listening**, not for sampling. Original synthesis only in-repo.

| Mix / video | Why | URL |
|-------------|-----|-----|
| DARK MEMPHIS PHONK MIX — Weedim (1h, Triple Six / Zirk / Freddie Dredd) | Long dark Memphis/horrorcore phonk continuity | https://www.youtube.com/watch?v=ScnOmK9mMU8 |
| underground memphis rap mix you needed — mkboi (~52m) | 90s underground Memphis atmosphere | https://www.youtube.com/watch?v=GeLum7sxj6Y |
| MEMPHIS / PHONK / SYNTHBASS MIX vol. VII (~1h24) | Broader modern Memphis/phonk/synthbass | https://www.youtube.com/watch?v=3-fIfNqdtWo |
| DJ Paul & Juicy J — Smoked Out Vol. 2 Da Exorcist | Classic Paul/Juicy tape-era energy (historical) | https://www.youtube.com/watch?v=aKPl3mi62fI |
| Bad Gear — The Memphis Rap Machine | Hardware context (DR-5 / scene machines) | https://www.youtube.com/watch?v=q322y4evhNA |
| HARDWARE PHONK — Memphis on DR-660 / DR-5 (Toilet Zone) | Live hardware groove demos | https://www.youtube.com/watch?v=-TzXYKULPL8 · https://www.youtube.com/watch?v=J9wkHO0zeW4 |
| How To Make MEMPHIS BEATS — Navie D | Contemporary producer explanation of pocket | https://www.youtube.com/watch?v=-0dWOKUp32w |

---

## Groove / MIDI language (original sketches)

Aligned with existing blueprints; **not** transcriptions of the mixes above.

| Lane | BPM | Snare pocket | Pattern notes |
|------|-----|--------------|---------------|
| Horror Snap (Paul) | 94 | Steps 8 & 24 on 16ths-per-2-bars (= backbeat 2 & 4 in half-time) | Sparse hats (quarter-ish); clap soft under snare; FX stab off-grid; leave space for double-time bars | `cnv-arch-horror.json` `[inspected]` |
| Club Bounce (Juicy) | 134 half-time feel | Same 2/4 snare; slightly higher snare vel; busier hats | Punchier K/S; still not trap rolls | `cnv-arch-bounce.json` `[inspected]` |
| Lab 4-bar (DR-660) | 91, swing 57 | Snare on 16th-grid indices 4 & 12 per bar | Ghost rims; cowbell clave melody; hat ratchets 3–4 on fills | `memphis_4bar_pattern.json` `[inspected]` |

**16th-grid mental model (1 bar, half-time):**

```
Kick:  X . . . . . x . X . x . . . x .
Snare: . . . . S . . . . . . . S . . .
Hat:   thin / lazy — delay odd 16ths for Memphis swing
```

---

## DSP knobs (this change)

| Knob | Before (BB96-ish) | After (Paul/Juicy snap) | Why |
|------|-------------------|-------------------------|-----|
| Membrane roots `SNR_HZ` | 180–262 Hz | ~145–196 Hz | Pitched-down body |
| `snare_pitch_base` (BB96) | 22 st | 14 st | Less chirpy pitch env |
| Flam | ~4–6 ms | ~5–7 ms default | Clap snap after body |
| Wires / clip | moderate | hotter wires, shorter noise on snap tags | “Snap” without bright tiny body |

Artist names remain **internal research only** — never in WAV filenames or store titles.

---

## Method

1. Repo: `RESEARCH_MEMPHIS_TOP20.md`, `MEMPHIS_ARCHETYPES.md`, `GEM_MEMPHIS_DR660_SNARE.md`, loop JSONs, `voices.py` / `phonk_snare.py`.  
2. Memory: L-0017 elite phonk parallel banks.  
3. Web: YouTube search (Alexandria) for Memphis phonk / Paul–Juicy mixes; prior Loaded Samples / RBMA citations via CNV research.

---

## Open questions / HITL

- Ear-pass new snares vs Weedim mix / Smoked Out tape feel.  
- Whether BB98 should stay brighter (trunk crack) vs also pitch-down.  
- Measure spectral centroid of new vs old Aggro snare `[executed]` after regen.

---

## Next actions

1. **audio-dsp-engineering** — Apply membrane / pitch / flam / wire changes; regen BB listen snares.  
2. **product-design** — Optional “Horror Snap” / “Club Bounce” snare folders (no artist names).  
3. HITL listen booth after publish.
