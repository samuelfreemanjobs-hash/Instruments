# MPC 500 Compact Flash layout (stock OS · manual copy)

Designed for **your profile** (locked 2026-11-03):

| Decision | Choice |
|----------|--------|
| OS | **Stock Akai** (no Open500 in v0) |
| CF layout | **This doc** (v1) |
| Lane | **Phonk** (drift / Memphis / 808 weight) |
| Clock | **MPC 500 master** → DIN MIDI out |
| Computer → CF | **Manual copy** (Finder/Explorer); optional Maid later |

Validate folder visibility on your unit after first format **[executed on hardware]**. Akai browsers vary slightly by OS revision; keep names short and FAT-safe.

## Design goals

1. **One glance on the MPC** — samples, programs, and sequences are not mixed in one flat folder.
2. **Phonk-first** — default pad map and naming for 12-pad drift phonk (140 BPM session default).
3. **Manual copy friendly** — copy whole folders from computer staging → CF without renames on the box.
4. **Room to grow** — `BANK_B` for live sets or second genre without touching phonk bank.

## CF tree (on the card)

Use a single root folder so the card is not cluttered at the top level:

```text
/MPC500/
  README.txt          ← optional: copy checklist from repo mpc500/lanes/phonk/COPY_CHECKLIST.txt
  _INBOX/             ← drop new WAVs here; load on MPC then delete or archive
  BANK_A/
    PHONK/
      SAMPLES/        ← *.WAV (mono 44.1 kHz recommended)
      PGM/            ← programs saved FROM the MPC (hardware SAVE)
      SEQ/            ← sequences saved FROM the MPC
  BANK_B/
    LIVE/             ← scratch performances / second style (empty until you need it)
  BACKUP/             ← optional: zip exports from computer, not loaded by MPC
```

**Do not** commit Akai OS images to git. Keep `OS/` or vendor files only on your physical card if required by the manual.

### Naming rules (FAT-safe)

| Rule | Why |
|------|-----|
| **8.3 uppercase** for WAV on CF (`KICK808.WAV`) | Older Akai browsers tolerate this best **[assumed]** |
| No spaces | Use `_` if needed |
| Lane prefix in staging only | Staging may use `P01_KICK808.wav`; CF copy uses short names |
| One shot per file | Phonk v0: no multi-gig loops on card |

## Computer staging (before manual copy)

Repo tool prepares a mirror of what you will drag to CF:

```bash
python3 tools/mpc500/stage_cf_inbox.py \
  --lane phonk \
  --from ~/Music/Disklordz/Inbox/your-kit \
  --out ~/Music/Disklordz/MPC500_STAGING
```

Output:

```text
~/Music/Disklordz/MPC500_STAGING/
  MPC500/BANK_A/PHONK/SAMPLES/*.WAV
  manifest.json
  COPY_CHECKLIST.txt
```

Copy `MPC500/` from staging onto the CF root (merge with existing `BANK_*`).

## Phonk 12-pad map (v1)

Pad numbering follows **MPC Maid / Akai 1–12** (bottom row 1–3, top row 10–12). Map file: [`mpc500/lanes/phonk/pad-map.json`](../mpc500/lanes/phonk/pad-map.json).

| Pad | Role | CF sample (8.3) | Notes |
|-----|------|-----------------|-------|
| 1 | 808 kick | `KICK808.WAV` | Disklordz `midnight-circuit` or `mpc-ready-808` kick |
| 2 | Snare / snap | `SNARE.WAV` | Memphis snap, short decay |
| 3 | Clap layer | `CLAP.WAV` | Stack with snare on 2 |
| 4 | Closed hat | `CHH.WAV` | |
| 5 | Open hat | `OHH.WAV` | |
| 6 | Rim / ghost | `RIM.WAV` | |
| 7 | Cowbell | `COWB.WAV` | Drift phonk signature |
| 8 | FX / vox chop | `FXVox.WAV` | Short memphis-style hit |
| 9 | Sub / 808 tail | `SUB808.WAV` | Longer 808 for holds |
| 10 | Reverse / rise | `FXREV.WAV` | Transition FX |
| 11 | Vinyl / noise | `VINYL.WAV` | Loop or one-shot bed |
| 12 | Perc / tamb | `PERC.WAV` | Fill |

**Session BPM:** **140** default (range 130–160 per Disklordz drift phonk lane). Set on MPC before sequencing; slaved gear follows **MIDI clock out**.

## Manual load workflow (stock OS)

1. Copy staged `SAMPLES/*.WAV` → CF `MPC500/BANK_A/PHONK/SAMPLES/` (or `_INBOX/` for experiments).
2. On MPC: **LOAD sample** (or equivalent browser) from `BANK_A/PHONK/SAMPLES/`.
3. Assign each sample to the pad per table above; tune decay / filter on hardware.
4. **SAVE program** → `BANK_A/PHONK/PGM/PHONK001.PGM` (name on device).
5. **Sequence** at 140 BPM; **SAVE sequence** → `BANK_A/PHONK/SEQ/`.
6. For hybrid rig: connect **MIDI OUT** → slave **MIDI IN**; MPC = **Master clock + transport**.

## MIDI master (MPC → world)

| Setting | Recommendation |
|---------|----------------|
| Clock | **Internal** on MPC |
| MIDI out | Send **clock** (+ start/stop if slaves need it) |
| DIN routing | MPC OUT → interface or hardware IN |
| DAW | DAW as **slave** (external sync) when recording stems — optional |
| SP1200 / synth | Slaves on clock; note data from MPC pads/seq as you program |

Document your cable chain in `manifest.json` `midi.notes` when you lock it **[executed]**.

## Disklordz generation hints

| Preset | Use for phonk lane |
|--------|---------------------|
| **`midnight-circuit`** | Primary — drift phonk, 808 glide, Memphis snap |
| **`mpc-ready-808`** | Neutral one-shots when you want to map pads yourself |

After generate: `disklordz/daw-inbox` → `stage_cf_inbox.py` → manual CF copy.

## Revision history

| Version | Change |
|---------|--------|
| v1 | Initial layout: stock OS, phonk bank, manual copy, MPC master |
