# V Voyager — Four-Stack Moog Engine

**VOYAGER-V** is not a single-hardware clone. It combines four classic Moog monosynth personalities in one instrument:

| Lineage | Hardware reference | Engine emphasis |
|--------|---------------------|-----------------|
| **Voyager** | Moog Voyager | Triple osc bank, drift, modulation bus |
| **Sub Phatty** | Moog Sub Phatty | Hot mixer/filter drive, sub osc, tighter cutoff |
| **Subsequent 37** | Moog Subsequent 37 | Dual-osc feel, moderate sub, slight cutoff openness |
| **Model D** | Minimoog Model D | Vintage ladder, low drift, 3× saw character |

## UI

- **Keyboard lineage** combo selects the active stack.
- Badge under the title shows the short label (`VOYAGER`, `SUB PHATTY`, etc.).
- Accent color follows lineage (amber · coral · cyan · sky blue).
- **Sub −1** knob: square sub-oscillator one octave below root (essential for Phatty / Subsequent patches).
- Changing lineage applies a **starter template** (osc waves, sub level, filter/drive). Factory program number is unchanged; tweak after switching if needed.

## DSP (real-time)

`MoogStackLineage::coeffsFor()` scales per voice:

- Oscillator drift amount  
- Filter cutoff multiplier  
- Resonance bias  
- Mixer saturation and ladder drive  

Sub oscillator is mixed pre-ladder when **Sub −1** &gt; 0.

## Presets

All **300** factory programs remain valid on any lineage. For best results, match lineage to patch style (e.g. boogie bass → Sub Phatty or Model D, Voyager leads → Voyager).

## Related

- [UI_CONCEPT.md](UI_CONCEPT.md) — panel layout roadmap  
- [README.md](../README.md) — build and demos  
