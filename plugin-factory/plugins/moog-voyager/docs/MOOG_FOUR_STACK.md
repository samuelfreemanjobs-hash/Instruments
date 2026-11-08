# V Voyager — Four-Stack Moog Engine

**VOYAGER-V** is not a single-hardware clone. It combines four classic Moog monosynth personalities in one instrument:

| Lineage | Hardware reference | Engine emphasis |
|--------|---------------------|-----------------|
| **Voyager** | Moog Voyager | Triple osc bank, drift, modulation bus |
| **Sub Phatty** | Moog Sub Phatty | Hot mixer/filter drive, sub osc, tighter cutoff |
| **Subsequent 37** | Moog Subsequent 37 | Dual-osc feel, moderate sub, slight cutoff openness |
| **Model D** | Minimoog Model D | Vintage ladder, low drift, 3× saw character |

## UI

- **STACK MODE** four-way switch (Voyager · Sub Phatty · Sub 37 · Model D) selects the active stack — automatable via **Moog Stack**.
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

All **300** factory programs include a default **STACK MODE** (0–3) and **pink noise** hint, applied when you load the program (you can still override with the four switches).

Heuristics: bass / screw / hot-mix patches → Sub Phatty; glide-heavy → Subsequent 37; Model D names / vintage mono → Model D; otherwise Voyager.

## Related

- [UI_CONCEPT.md](UI_CONCEPT.md) — panel layout roadmap  
- [README.md](../README.md) — build and demos  
