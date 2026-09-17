# Plugin spec — MVP (TR-808 class)

## Shipping name (pick one — Creative Director)

| Name | Rationale |
|------|-----------|
| **NovaDrum** | Pairs with **Junova-X** (recommended default) |
| Circuit Eight | Descriptive; less brand-linked |
| OctaRhythm | Generic |
| EightCircuit | Technical |

**Recommendation:** **NovaDrum** — same “Nova*” family, clear drum role, store-friendly.

## MVP promise

Sixteen **808-class voices** via **circuit engines** (not samples), playable from **MIDI**, with **kit prev/next** and **closed-hat / open-hat choke**.

## In scope (v1)

- Voice build order: **BD → SD → CH → OH → CP**, then remaining voices per blueprint
- Four engine classes (see [architecture-blueprint.md](architecture-blueprint.md))
- MIDI note map (standard-ish 808 layout — document in implementation)
- Kit presets (minimum 1 factory kit + save/load)
- Windows VST3 x64 build

## Out of scope (v1)

- Onboard step sequencer
- 4×4 pad grid UI
- Sample import / ROM playback
- macOS AU (until Windows ship gate passes)

## Acceptance criteria (Airtable WO template)

1. BD, SD, CH, OH, CP audible via MIDI in host smoke test.
2. CH/OH choke verified (OH cut when CH fires).
3. Kit prev/next switches parameter sets without crash.
4. No sample files required for playback (engine-only).
5. README lists known gaps vs full 16-voice target.

## Grok / Cursor split

- **Grok Plugin team:** voice schematics, parameter tables, UI wireframes.
- **Cursor:** `vst-tr808/plugin/` implementation after Junova-X host milestone.

Paste additional **voice-by-voice** sections below as they are drafted; engine classes may start before all schematics are complete.

---

### Voice backlog (fill from DSP Eng)

| Voice | Engine class | Status |
|-------|--------------|--------|
| BD | | Spec |
| SD | | Spec |
| CH | | Spec |
| OH | | Spec |
| CP | | Spec |
| … | | |
