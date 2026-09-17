# Phone PWA → Akai MPC Sample

**Hardware:** [Akai MPC Sample](https://www.akaipro.com/mpc-sample/) — standalone 16-pad sampler, USB-C (power, audio, MIDI, file access), 44.1 kHz import, built-in mic/speaker.

This PWA does **not** replace MPC Sample software; it **feeds** it with 808 hits or future prompt-generated WAVs.

## Recommended paths

### 1 — WAV file (most reliable)

1. In the PWA, use **Export pattern WAV** or per-pad capture (when enabled).
2. On phone: **Share / Save** → Files, Google Drive, or email to yourself.
3. Connect MPC Sample to a computer via USB-C **or** copy to **microSD** and insert in MPC Sample.
4. Import: MPC Sample supports **.wav** (16/24-bit, 44.1/48/96 kHz) — prefer **44.1 kHz, 16-bit** for parity with factory processing.

### 2 — USB-C audio resample (sketch)

1. Play the PWA from a phone or tablet (headphone out or USB-C DAC).
2. MPC Sample: **Sample from Input** (¼" line or USB audio per your cable setup).
3. Use **threshold** or **fixed length** sampling on MPC Sample to grab one-shots or a 1–2 bar loop.

Check Akai’s current firmware notes for **USB audio from Android/iOS** — behavior varies by device.

### 3 — Recall / performance capture

MPC Sample **Recall Recording** (~30 s) can capture pad performance after you finger-drum the PWA via MIDI (desktop) or resampled hits.

## SaaS + niche loops (future)

Generated packs should ship as:

- Up to **16 files** named `BD.wav` … `MA.wav` (or GM note in filename)
- **44.1 kHz**, peak-normalized, short tails trimmed for pad memory
- Optional **manifest.json** for MPC Sample batch import on desktop bridge

## Marketing copy (accurate)

> “Make 808 sounds in your browser — **export WAVs built for MPC Sample** (and any pad workflow).”

Avoid claiming one-tap import into MPC Sample from mobile without a tested OS/USB workflow.
