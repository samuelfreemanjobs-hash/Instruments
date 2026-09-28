# Local playbook — hermes-gui

Seat-specific notes **not yet** promoted to the elite skill. Newest at top.

## Format

```markdown
### YYYY-MM-DD — context (WO-…)
- What worked
- What failed
- Command / path to remember
```

---

### 2026-09-28 — Celestial UI (WO-2026-001)

- Artboard **1280×840** in `Junova-X/Source/UI/UiLayout.h`; module shells in `Source/UI/Celestial/`.
- **GUI Agent** mandatory: standalone at `build/Junova-X/.../Standalone/Junova-X`; screenshots to `/opt/cursor/artifacts/`.
- OSC monitor: processor `ScopeFifo` + `OscMonitorComponent` timer 30 Hz.
- Keyboard: paint on `CelestialMainPanel` with `keyboardStrip_.setOpaque(false)` so keys are visible.
- Design PNG: `Junova-X/Resources/design-reference-celestial.png`.
