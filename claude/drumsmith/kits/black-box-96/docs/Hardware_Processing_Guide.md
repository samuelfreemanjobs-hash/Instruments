# BLACK BOX 96 — hardware / DAW processing guide

Producer instructions for this zip. Original synthesis only — not a hardware ROM dump.

## What’s in this pack

| Folder | What it is |
|--------|------------|
| `01_CORE_ONESHOTS/` | Clean-ish RAW cores (808s, kicks, snares, claps, hats, cowbells, toms, perc) |
| `02_BITCRUSHED_AND_PITCHED/` | Rate-dropped 808s (−12 / −24 st), pitched snares & cowbells, aliased color shots |
| `03_CASSETTE_AND_TAPE/` | Same hits through cassette desk FX (saturate → tape LPF → wow/flutter → hiss) |
| `04_PARALLEL_PROCESS_BANKS/` | `RAW` / `TAPE` / `LOFI` / `CASSETTE` siblings of the same core drums |
| `05_LOOPS/` | 94 BPM tape-creep + 134 half-time construction loops |
| `06_MATCHED_MINI_KITS/` | Ready-to-load oneshots + BPM-labeled loop per groove |
| `_DOCS/` | This guide + product notes |

## Rate-drop / pitched subs

1. Start from `01_CORE_ONESHOTS/01_808_Subs/` or use ready variants in `02_BITCRUSHED_AND_PITCHED/RateDropped_808s/`.
2. Pitch down hard (−12 to −24 st) or slow playback so the fundamental crawls.
3. Shorten with a fast release / choke so long notes don’t smear the pocket.
4. Add mild drive or soft clip for converter grit; keep mono for center weight.

## Cassette / tape grit

- Soft clip → gentle HF roll-off (~10–12 kHz) → optional hiss from `03_CASSETTE_AND_TAPE/` or parallel `CASSETTE` lane.
- Prefer short decays on snares/hats so cassette compression stays punchy.
- Layer crunchy closed hats over thick 808 beds — two layers, not one ROM.

## Parallel process banks

Use `04_PARALLEL_PROCESS_BANKS/` when you want the **same hit** three/four ways:

- **RAW** — core render, light Memphis engine grit already baked
- **TAPE** — soft saturate + darker LPF
- **LOFI** — extra bitcrush / alias hold
- **CASSETTE** — full desk: drive, tape LPF, wow/flutter, light hiss

A/B RAW vs CASSETTE in your sampler before committing a kit slot.

## Aliased / pitched one-shots

- Use `02_BITCRUSHED_AND_PITCHED/Aliased_One_Shots/` as color, not the whole groove.
- Re-pitch in your sampler; watch for digital trash that fights the kick.

## Loops

- Rhythm loops at **94** BPM (tape creep) or **134** half-time feel under `05_LOOPS/`.
- Prefer pitched long subs as bassline, cowbell motifs, sparse kick, half-time snare, thin hats.
- Mute or filter top loops for arrangement — don’t brick-wall.

## Matched mini-kits

Each folder under `06_MATCHED_MINI_KITS/` is a self-contained groove:

- OneShots (kick, snare, hat, clap, cowbell)
- One BPM-labeled loop

Load the folder, map pads, press play.

## Device notes

Import as generic PCM16 WAV @ 44.1 kHz mono. MPC / Maschine+ / SP-404 / Lofi-12 paths are **UNVERIFIED** until a real device test is logged.

## Honest gaps

- Not a Boss DR-660 / TR-808 / SP-1200 dump.
- `release_ready` stays false until HITL (ear / cover / pricing / license).
