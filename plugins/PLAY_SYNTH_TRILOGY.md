# PlaySynth Trilogy (3-pack suite)

Upgraded PLAY Series line — **three distinct engines** (not reskins). Shared UX shell:

- Equal-power **A/B blend** + **XY pad** (`xy_x` = blend, `xy_y` = cutoff + space/shimmer)
- **8 genre-specific macros** per plugin
- **Dice** patch randomizer (UI/message thread only; bounded params; never master gain)
- 12 **BinaryData** factory presets each

| Plugin | CODE | APC folder | Signature DSP / FX | UI |
|--------|------|------------|-------------------|-----|
| AnalogDreams84 | Ad84 | `plugins/AnalogDreams84` | PWM/saw + Juno BBD chorus + 80s plate | Magenta / midnight blue |
| CloudSupplyTrap | CsTr | `plugins/CloudSupplyTrap` | 3-op FM bells + formant choir + shimmer verb | Obsidian / cyan |
| LoFiGlowTape | LgTp | `plugins/LoFiGlowTape` | EP tine + 12-bit DAC + wow/flutter + vinyl | Amber / charcoal |

**Factory batch:** `disklordz/vst-factory/data/overnight-queue.json` (priority 10 trilogy block).

**Scaffold:** copy `templates/juce8-synth-base/` → root `AnalogDreams84/` (rename symbols).

Supersedes generic `PlaySynthDreams` / PsAD / PsCS / PsLF queue stubs.