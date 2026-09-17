# DiskLordz YouTube network — channel brands

Four **separate YouTube brands** under DiskLordz (not one generic upload channel). Each has a distinct **audience job**, **sonic lane**, **visual system**, **programming format**, and **commerce funnel** (Records → Supply → Labs).

**Policy:** Market Intel and A&R do not greenlight batches or name these in Slack until each brand has a sourced `DL-OPP-*` record — see [FACTORY_RESEARCH.md](FACTORY_RESEARCH.md).

## Brand index (locked genres)

| # | Primary channel | Genres (HQ) | Artist lane | BPM | Audience job |
|---|-----------------|-------------|-------------|-----|----------------|
| 1 | **Chrome Circuit** | French touch / electro-funk / nu-disco | DL001 | 118–124 | Deep work, night cruise, creative session |
| 2 | **GhostDrive 808** | **Drift phonk · wave phonk** | DL002 | 140–160 | Gym, night drive, FPS |
| 3 | **Dust & Tape** | **Lo-fi · DJ Screw · 90s phonk** | DL006 | 60–95 | Slow study, chop & screw sessions, late-night focus |
| 4 | **Neon Terminal** | **Cyber funk** (coding) · *vaporwave aesthetic layer* | DL004 | 112–128 | Coding flow, build sessions, terminal deep work |

---

## 1. French touch — **Chrome Circuit** ✓

**Alt names:** Boulevard 84, Vanguard Disco, Neon Mirage

**World:** Late-1990s Parisian retro-futurism + sci-fi anime (Discovery / Interstella 5555). Vintage sports car through a lit city at 1:00 AM; high-tech robotic sound lab.

**Visual:** Chrome robot helmets, LED visors, gold/silver metal, sunset gradients (magenta, electric violet, gold), ’80s cockpit dashboards. Loops: reactive EQ on robot console, highway perspective lines, mirrorball light sweeps.

**Sonic:** 118–124 BPM. Sidechained filter sweeps, talkbox/vocoder, 909 four-on-the-floor, 707 percussion, compressed analog bass.

**Funnel:** French Touch sample packs, 909/707 MPC kits, analog bass loops, vocoder chops.

---

## 2. Drift phonk · wave phonk — **GhostDrive 808**

**Alt names:** Phantom Grip, Kuro Drift, Midnight Circuit

**World:** Late-night Tokyo / Memphis — drift culture, rain asphalt, CRT, cyberpunk alleys.

**Visual:** High-contrast mono + UV / crimson / toxic green. RX-7, R34, AE86, tire smoke, VHS scan lines, CCTV timestamps, glitch. Loops: headlights in fog, rain on lens, tachometer pulse.

**Sonic:** 140–160 BPM. **Drift phonk** saturation + **wave phonk** melodic/driving top lines where appropriate. 808 sub glides, Memphis vocal chops, cowbell arps, tape saturation.

**Audience job:** Gym PRs, high-speed night drive, intense gaming / FPS.

**Funnel:** 808 banks, wave-phonk lead/stab packs, cowbell fonts, stream-safe audio tiers.

**Research (`DL-OPP`):** drift vs wave phonk search split, gym Shorts competitors — *sources required*.

---

## 3. Lo-fi · DJ Screw · 90s phonk — **Dust & Tape**

**Alt names:** Analog Solitude, Static Reverie, Late Night Terminal

**World:** Chopped, slowed tape culture — 3:00 AM screw sessions, amber sampler screens, Houston/South slow vibe meets **90s phonk** tape hiss, rain on glass.

**Visual:** Film grain, warm desk (MPC / SP-808 / cassette deck), half-empty cup, rain window. Loops: cassette spools, pitch-drop visual, rain on glass.

**Sonic:** 60–95 BPM (screw-friendly range). Unquantized or heavily swung drums, **DJ Screw–style** pitch/speed manipulation, vinyl crackle, detuned keys, 90s phonk sample vocabulary at **slow** tempos — not gym phonk.

**Audience job:** Long study blocks, slow focus, “ride the tape” listening sessions.

**Funnel:** Screw/phonk chop kits, SP-404/MPC one-shots, pitched vocal stem packs, RC-20 / cassette chains. **Labs:** SP-1200 bit-crunch authenticity for product demos.

**Note:** Same phonk *family* as GhostDrive but **different channel job** (slow chop vs fast drift/wave). A&R treats these as **non-interchangeable** programming.

**Research (`DL-OPP`):** screw/lo-fi study retention, 90s phonk sample demand — *sources required*.

---

## 4. Cyber funk for coding — **Neon Terminal**

**Working primary name:** Neon Terminal *(rename OK — genre lock is cyber funk + coding)*

**Alt names:** Stack Trace, Cipher Drive, Grid Runner

**World:** Clean-room cyberpunk — developer at 2:00 AM, neon grid city outside floor-to-ceiling windows, retro-future UI, synth funk for **focus not hype**.

**Visual:** Terminal green / cyan on charcoal, monospace overlays, wireframe grids, isometric cityscapes, subtle matrix rain, oscilloscope sweeps (not gym/drift imagery).

**Sonic:** 112–128 BPM. **Cyber funk** — rubbery digital bass, FM/stab chords, tight electronic drums (909/707 hybrid feel), minimal vocals, sidechain pump optional but **cleaner** than French Touch filter drama.

**Audience job:** **Coding flow**, build streams, technical deep work (pairs with but does not duplicate Chrome Circuit’s Paris robot story).

**Funnel:** Cyber funk MIDI packs, stab/synth preset banks, “coding session” long mixes → **Labs** plugin presets (JD Upgraded / future DiskLordz synths).

**Artist lane:** DL004 — cyber disco / cyber funk IP.

**Research (`DL-OPP`):** “music for coding” cyber/synth competitor set, BPM band validation — *sources required*.

---

## Factory operations (×4 brands)

```text
DISCOVER → one sourced DL-OPP per YT-DL-001…004
    → IDEATE → A&R greenlight per channel (no cross-brand batch mush)
    → GENERATE / PRODUCE / HUMANIZE
    → PACKAGE → SKU prefix per brand (chrome-*, ghost-*, dust-*, neon-*)
    → PUBLISH → programming matrix per channel
    → MEASURE → watch time + product attach by brand
```

**Night shift:** no `artist_id` on assets until that brand’s opportunity is greenlit.

## Ingest (after research links)

One `POST /research/opportunities` per row in [`youtube_network_seed.json`](../disklordz-factory/database/youtube_network_seed.json), then `POST /research/aar-decisions` binding `DL001`, `DL002`, `DL006`, `DL004`.

## Related

- [FACTORY_RESEARCH.md](FACTORY_RESEARCH.md)  
- [disklordz-factory/docs/BUSINESS_ENGINES.md](../disklordz-factory/docs/BUSINESS_ENGINES.md)  
- [AGENTS.md](../AGENTS.md)
