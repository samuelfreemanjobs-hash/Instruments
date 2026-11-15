Sam's WAVE-9090 design system guide (pasted in chat). Stitch screen: https://stitch.google.com/projects/14288623377553782524/screens/51789caf2f05415fa837622f01f1bdab

# WAVE-9090 UI Design System (Sam, v1)
Style: late-80s Ensoniq (SQ-80/TS-10/VFX-SD: amber VFD displays, membrane buttons, colour-coded zones) x Arturia V Collection/PolyBrute (anodized aluminium, walnut cheeks, knurled knobs with halo LEDs, OLED scopes).

## Colour tokens
Domain: cyan-glow #00f0d0 (OSC1 wavetable, Env1/VCA, primary accent); amber-glow #ff9900 (OSC2/Transwave, VCF2, ring mod, saturation); signal-blue #3877ff (mod/LFO/sub); panic-red #e62e2d (panic, clip LEDs); felt-crimson #8a1818 (key felt strip).
Surfaces: bg-chassis #0b0e14; surface-panel #101319; surface-inset #171b24; surface-glass #070a0e; wood-dark #23120b; wood-highlight #542b18; metal-screw #4a5263.
Text: primary #f0f4fc; secondary #8e9bb0; dim #475063.

## Type
Space Grotesk for UI; JetBrains Mono / Courier New for readouts.
Brand 20px 900 caps +0.15em; module title 12px 700 caps +0.08em; knob label 10px 600 caps +0.05em; readout mono 11px 700; silkscreen 8.5px 700 caps +0.12em.

## Components
- Knobs: dark titanium rim with inset shadow, conic spun cap, 2px pointer with glowing tip in domain colour, optional 270° halo arc (7 to 5 o'clock) on macros.
- Module boxes: 6px radius, 1px rgba(255,255,255,.07) border, top highlight .12, 4px corner rivets #1f242e.
- Scopes/VFD: radial glass gradient #0d151c to #06090d, 2px scanlines at 4%.
- Buttons: #191d26, border #2e3545; active state backlit with a coloured glow. Segmented rocker selectors (LP|BP|HP etc).
- Keybed: 61 keys C1-C6, ivory gradient whites, ebony blacks at 60% height, #8a1818 felt strip, octave labels.
- Pitch/mod wheels in recessed ribbed wells with LED; 4 macro knobs.
- Sliders with textured thumbs, LED VU meters (green/yellow/red).

## Layout (signal flow, left to right)
Top chrome: logo, preset browser, meters, PANIC, tabs.
Row 1: OSC1 (cyan) | FM/Ring link | OSC2 (amber)
Row 2: VCF1 ladder | blend/routing | VCF2 SVF
Row 3: ENV 1-3 | LFO 1/2 | Macros M1-M4
Bottom: performance wheels | 61-key keybed | FX wing
Spacing: 4px grid; button radius 2px, panel radius 4-6px. Layers: wood, then faceplate #101319, then module bays #171b24, then controls, then LEDs/waveforms.
