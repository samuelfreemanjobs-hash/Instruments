# DiskLordz YouTube network — channel brands

Four **separate YouTube brands** under DiskLordz (not one generic upload channel). Each has a distinct **audience job**, **sonic lane**, **visual system**, **programming format**, and **commerce funnel** (Records → Supply → Labs).

**Policy:** Market Intel and A&R do not greenlight batches or name these in Slack until each brand has a sourced `DL-OPP-*` record — see [FACTORY_RESEARCH.md](FACTORY_RESEARCH.md).

## Brand index

| # | Primary channel | Factory artist lane (proposed) | BPM | Supply funnel |
|---|-----------------|----------------------------------|-----|----------------|
| 1 | **Chrome Circuit** | DL001 — robot funk / French house / electro | 118–124 | French Touch kits, 909/707 MPC, talkbox chops |
| 2 | **GhostDrive 808** | DL002 — dark phonk / Memphis / 808 | 140–160 | 808 banks, cowbell, stream-safe licenses |
| 3 | **Dust & Tape** | DL006 — digital soul / lo-fi lane | 72–85 | SP-404/MPC one-shots, Rhodes loops, RC-20 chains |
| 4 | *TBD* | *Reserve DL003–DL005 or new ID* | — | — |

---

## 1. French Touch / Electro-Funk / Nu-Disco — **Chrome Circuit**

**Alt names:** Boulevard 84, Vanguard Disco, Neon Mirage

**World:** Late-1990s Parisian retro-futurism + sci-fi anime (Discovery / Interstella 5555). Vintage sports car through a lit city at 1:00 AM; high-tech robotic sound lab.

**Visual:** Chrome robot helmets, LED visors, gold/silver metal, sunset gradients (magenta, electric violet, gold), ’80s cockpit dashboards. Loops: reactive EQ on robot console, highway perspective lines, mirrorball light sweeps.

**Sonic:** 118–124 BPM. Sidechained filter sweeps, talkbox/vocoder, 909 four-on-the-floor, 707 percussion, compressed analog bass.

**Audience job:** Deep work, coding flow, night cruise, creative session.

**Funnel:** Producers + hardware fans → French Touch sample packs, 909 MPC kits, analog bass loops, vocoder chops.

**Research to file (`DL-OPP`):** YouTube search demand for “French touch mix,” competitor channel RPM/format, Splice/Loopmasters French house pack gaps. *Sources required before A&R greenlight.*

---

## 2. Drift Phonk / Gym Phonk — **GhostDrive 808**

**Alt names:** Phantom Grip, Kuro Drift, Midnight Circuit

**World:** Late-night Tokyo / Memphis drift culture — rain asphalt, CRT, cyberpunk alleys, underground racing.

**Visual:** High-contrast mono + UV / crimson / toxic green. RX-7, R34, AE86, tire smoke, VHS scan lines, CCTV timestamps, glitch. Loops: headlights in fog, rain on lens, tachometer pulse.

**Sonic:** 140–160 BPM. Saturated 808 subs, chopped Memphis vocals, cowbell arps, tape saturation.

**Audience job:** Gym PRs, night drive, FPS / gaming intensity.

**Funnel:** Viral gym/car communities → 808 banks, cowbell fonts, pre-cleared streamer audio tiers, darkwear (if brand extends).

**Research to file:** Gym phonk retention benchmarks, competitor Shorts ratio, sample market pricing, ad RPM vs product conversion. *Attach citations; do not claim “high viral” in Slack without sources.*

---

## 3. Lo-Fi / Ambient Hip-Hop — **Dust & Tape**

**Alt names:** Analog Solitude, Static Reverie, Late Night Terminal

**World:** 3:00 AM studio solitude — tape decks, rain on glass, amber sampler screens, MPC/SP-808 corner.

**Visual:** Film grain, warm desk scene (MPC/SP-808, coffee, cat, rain window). Loops: cassette spools, coffee steam, rain on glass.

**Sonic:** 72–85 BPM. Swung unquantized boom-bap, vinyl crackle, detuned Rhodes, upright bass, tape flutter, room/rain bed.

**Audience job:** Study, calm coding, anxiety relief, long-session focus.

**Funnel:** Long watch time → streaming catalog (Spotify/Apple) + SP-404/MPC one-shots, piano loops, RC-20/cassette preset chains. **Labs:** SP-1200 / sampler authenticity story where relevant.

**Research to file:** Lo-fi study channel session length studies, streaming vs AdSense split, SP-808 hardware search trend. *Sources required.*

---

## 4. Fourth brand — placeholder

You outlined **four** network brands earlier; paste brand #4 (name, vibe, BPM, funnel) to complete this doc and open `DL-OPP-000004`.

Candidates from the wider DiskLordz roster not yet assigned a flagship channel:

- **DL003** — Detroit electro  
- **DL004** — Cyber disco  
- **DL005** — Industrial trap  

---

## Factory operations per brand

```text
DISCOVER (sourced DL-OPP per channel)
    → IDEATE (A&R: one batch slot per greenlit brand)
    → GENERATE / PRODUCE / HUMANIZE
    → PACKAGE (channel-specific kits + metadata)
    → PUBLISH (programming: mixes vs singles vs Shorts ratio per brand)
    → MEASURE (watch time, CTR, product attach per Chrome Circuit vs GhostDrive vs Dust & Tape)
```

**Content multiplier:** one master track → full upload + Shorts + Spotify + **brand-specific** kit SKU (not shared generic naming).

**Night shift:** do not assign `artist_id` on assets until `GET /research/status` shows greenlight for that brand’s `opportunity_id`.

## Ingest example (after you have links)

```bash
# Chrome Circuit — replace sources with real URLs from your research backup
curl -s -X POST http://127.0.0.1:8787/research/opportunities -H 'Content-Type: application/json' -d '{
  "title": "Chrome Circuit — French Touch programming line",
  "genre": "french touch",
  "subgenre": "electro-funk",
  "bpm_min": 118,
  "bpm_max": 124,
  "aesthetic": "chrome robot / Interstella highway night",
  "audience": "deep work, coding flow",
  "use_case": "long-form mix + Shorts hooks",
  "search_demand": "medium",
  "competition": "medium",
  "score": 0,
  "sources": ["PASTE_YOUTUBE_OR_AIRTABLE_URL_HERE"]
}'
```

Repeat for GhostDrive 808 and Dust & Tape; set `score` only after you score from evidence; then `POST /research/aar-decisions` to bind `DL001`, `DL002`, `DL006`.

## Related

- [FACTORY_RESEARCH.md](FACTORY_RESEARCH.md)  
- [disklordz-factory/docs/BUSINESS_ENGINES.md](../disklordz-factory/docs/BUSINESS_ENGINES.md)  
- [AGENTS.md](../AGENTS.md)
