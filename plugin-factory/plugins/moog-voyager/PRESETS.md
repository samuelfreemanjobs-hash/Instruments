# V Voyager — Moog factory bank (300 programs)

**300** Voyager / Moog-style factory programs ship with the plugin. Programs **001–024** are hand-curated (Init, G-Funk, pads, FX). **025–300** are deterministic variants built from Moog archetypes (Model D, Mini, Memorymoog, ladder leads, West Coast funk, unison stacks, etc.).

Regenerate the bank:

```bash
python3 plugin-factory/plugins/moog-voyager/scripts/generate_factory_presets.py
```

## How to browse

- **DAW:** host program/preset list (300 entries)
- **UI:** **Factory preset** dropdown (scroll; type first letters on some hosts)

## Curated highlights (001–024)

| # | Name | Role |
|---|------|------|
| 001 | Voyager Init | Balanced starting point |
| 003 | G-Funk Lead | West Coast lead |
| 007 | G-Funk Bass | Funk sub line |
| 012–014 | Silk / Dream / Dark Atlas | Pads |
| 016 | Brass Stab | Short brass |
| 018–021 | Sweep / Wobble / Rumble / Whistle | FX |
| 024 | Full Stack Unison | Detuned triple stack |

Programs **025+** follow naming like `025 Fat Model D Lead`, `042 Moog Warm Bass`, `128 Vintage Memorymoog Pad`, etc.

User tweaks are stored in the host project via APVTS; factory program index is stored as `factoryProgram` in plugin state.
