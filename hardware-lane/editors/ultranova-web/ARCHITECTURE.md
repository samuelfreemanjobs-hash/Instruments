# UltraNova Web Editor

## Purpose

Team-owned **browser editor** for the maintainer’s **Novation UltraNova**: live program tweaks over **Web MIDI** (NRPN/CC), SysEx file import/send, and dump requests. Complements Novation’s desktop Librarian; does not redistribute factory `.syx` banks.

## Build & run

```bash
cd hardware-lane/editors/ultranova-web
npm ci
npm run dev    # http://localhost:5174
npm run build  # static output in dist/
npm test       # NRPN / SysEx unit tests
```

Or from repo root:

```bash
./hardware-lane/scripts/ultranova-web-dev.sh
```

**Browser:** Chrome or Edge (Web MIDI + `sysex: true`). **Hardware:** UltraNova on USB; enable SysEx receive; write protect off.

## Data flow

```text
User sliders (React state)
  → NRPN MSB=0 + LSB + data entry (CC 98/6) or CC
  → Web MIDI output → UltraNova edit buffer / current program

.syx file → parse F0…F7 messages → preview hex → optional send to synth
Synth dump request → SysEx out → MIDI in → log + best-effort patch name scan
```

**Not yet:** full program **binary** decode/encode (field-level `.syx` round-trip). Parameter map follows Novation MIDI implementation + MiniNova NRPN chart (same engine family).

## Threading / realtime

N/A (browser UI). Avoid flooding MIDI: live send is per-slider; use “Send all” sparingly.

## Key modules

| Path | Role |
|------|------|
| `src/data/parameters.ts` | Parameter catalog (sections, NRPN LSB, CC) |
| `src/midi/nrpn.ts` | NRPN message construction + send |
| `src/midi/sysex.ts` | Novation header, dump requests, `.syx` parse |
| `src/midi/webMidi.ts` | `requestMIDIAccess({ sysex: true })` |
| `src/App.tsx` | UI: MIDI ports, tabs, sliders, SysEx panel |

## Extension points

- Add NRPN rows from [UltraNova MIDI PDF](https://downloads.novationmusic.com/novation/synthesisers/ultranova) (program parameters).
- Port KnobKraft-style program dump layout for true preset save/load in-browser.
- Export JSON preset for agents (`instrument.map.json` / capture-plan metadata).
- Web MIDI **input** learn mode to sync sliders from hardware dumps.

## Related docs

- [../../ARCHITECTURE.md](../../ARCHITECTURE.md)
- [../../../docs/HARDWARE_MIDI_EDITORS.md](../../../docs/HARDWARE_MIDI_EDITORS.md)
- [../../../docs/HARDWARE_CAPTURE_RACK.md](../../../docs/HARDWARE_CAPTURE_RACK.md)
