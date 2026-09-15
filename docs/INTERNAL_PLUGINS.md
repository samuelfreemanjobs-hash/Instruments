# VMPC internal mixing plug-ins

Built-in **stereo insert** processors ship with the DAW (no scan required). Load them from the rack via **Load VST → VMPC internal mix**.

| Plug-in | Role |
|---------|------|
| **VMPC Gain** | Trim level (−60 … +12 dB) |
| **VMPC Pan** | Constant-power stereo pan |
| **VMPC EQ-3** | Low shelf (120 Hz), mid peak (1.2 kHz), high shelf (8 kHz) |
| **VMPC Compressor** | Threshold, ratio, attack, release, makeup |
| **VMPC Limiter** | Ceiling + release (safety on the master chain) |
| **VMPC HPF** | High-pass filter (rumble / mud) |
| **VMPC De-Esser** | Sibilance reduction |

## Suggested default chain

For a basic mix bus (after instrument VSTs):

1. **VMPC EQ-3** — tone balance  
2. **VMPC Compressor** — glue  
3. **VMPC Limiter** — peak control  

Use **Gain** and **Pan** on individual paths as needed.

## Vibe Mixing

Type a brief in the rack (**Vibe Mixing**) and click **Create vibe preset** to auto-build a chain + settings. See [VIBE_MIXING.md](VIBE_MIXING.md).

## Editors

Each internal plug-in uses the standard parameter editor (sliders). External VSTs still use their native UI.

## Implementation

Processors live under `Source/Audio/Internal/` as `juce::AudioProcessor` subclasses and share the same 8-slot rack as VST3/AU/LV2.
