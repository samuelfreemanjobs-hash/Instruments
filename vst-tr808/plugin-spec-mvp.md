# Plugin spec — MVP (TR-808 circuit recreation)

## Shipping name

**TBD** — Business Planner + Marketing. **NovaDrum is deprecated** as a placeholder. See [docs/PRODUCT_NAMING.md](docs/PRODUCT_NAMING.md).

## MVP promise

Sixteen **808-class voices** via **circuit engines** (not samples), playable from **MIDI**, with **kit prev/next** and **closed-hat / open-hat choke**.

Long-term vision (sequencer, chromatic BD, multi-out, per-voice saturation) is in [docs/circuit-emulation-spec.md](docs/circuit-emulation-spec.md); **not** all in MVP.

## In scope (v1)

- Voice build order: **BD → SD → CH → OH → CP**, then remaining voices per blueprint
- Four architectural classes (bridged-T, Schmitt cluster, noise/transient, hybrid) — see blueprint
- MIDI note map (document in `docs/MIDI_MAP.md` when implementation starts)
- Kit presets (minimum 1 factory kit + save/load)
- Windows **VST3** x64; **CLAP** per factory WO after host green
- State retention on retrigger (no machine-gun zeroing) — design requirement from day one for BD/metals

## Out of scope (v1)

- 64-step internal sequencer, probability, ratchets (Phase 5 in design spec)
- 16 discrete multi-outs (design target; MVP may be stereo master only — document in WO)
- Extended chromatic BD / infinite drone (Extended engine range in spec)
- macOS AU until Windows ship gate passes
- Sample import / ROM playback

## Acceptance criteria (Airtable WO template)

1. BD, SD, CH, OH, CP audible via MIDI in host smoke test.
2. CH/OH choke verified (OH cut when CH fires).
3. Kit prev/next switches parameter sets without crash.
4. No sample files required for playback (engine-only).
5. README lists known gaps vs full 16-voice target.
6. PR / WO title uses **`[Plugin][TR808]`** prefix (not NovaDrum).
