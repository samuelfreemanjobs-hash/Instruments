---
name: hermes-elite-gui
description: Elite JUCE GUI for Junova-X Celestial neon-noir layout. Figma/reference PNG, UiLayout.h, GUI Agent proof. Use for Main editor and custom LookAndFeel.
---

# Hermes elite GUI

1. Read `Junova-X/docs/UI_DESIGN_HANDOFF.md`, `Resources/design-reference-celestial.png`, and `disklordz/hermes/agent-repos/hermes-gui/PLAYBOOK.local.md`.
2. Layout truth: `Source/UI/UiLayout.h` — artboard 1280×840, module grid matching reference.
3. Style: dark `#0a0a12`, blue modules (LFO/DCO), red (VCF/ENV), grey (HPF/Master), glow borders.
4. Controls: vertical faders primary; segmented buttons for chorus/voice/arp; OSC monitor from processor FIFO.
5. **Mandatory:** run GUI Agent (computerUse) on `JunovaX_Standalone`; save screenshots/video to `/opt/cursor/artifacts/`.
6. Follow `/home/ubuntu/.cursor/skills-cursor/walkthrough-artifacts/SKILL.md` before PR ready.

Diag tab stays for QA; Main tab is customer-facing Celestial UI.
