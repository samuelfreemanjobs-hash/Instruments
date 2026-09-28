# Junova-X GUI design handoff

Your **designed GUI** integrates here without rewriting processor code.

## Steps (Hermes GUI seat + Cursor)

1. Export assets from Figma (PNG/SVG) into `Junova-X/Resources/` (create folder).
2. Update `Source/UI/UiLayout.h` with artboard width/height and control anchors from design.
3. Implement `MainPanel::paint` backgrounds and optional `Drawable` knobs from SVG.
4. Run **GUI Agent** (computerUse): standalone smoke + screenshots to `/opt/cursor/artifacts/`.

## Current scaffold

- **Main** tab: dual ADSR, cutoff/res, HPF toggle, chorus Off|I|II.
- **Diag** tab: test tone, frequency, Panic.

## Parameter map

All controls bind via APVTS IDs in `Source/Parameters/ParameterIds.h` — do not rename IDs after presets ship.
