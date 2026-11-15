# SOP-VF-02 — UI design (APC Design phase)

## Purpose

Produce a complete, buildable `design-spec.md` for a product by **composing design-system components with theme roles**. No new colours, no hex values, no custom widgets unless SOP-VF-10 is followed first.

## Inputs

- `.ideas/creative-brief.md` (UI behaviour), `.ideas/parameter-spec.md` (every control), `.ideas/architecture.md` (surface chain, open issues)
- Design system: `vst/factory-lib/design-system/` (contract, `components.md`, `foundations.md`, the product's theme and its preview)

## Procedure

1. **Pick the theme.** DiskLordz products: `disklordz`. Another brand: create a theme first ([SOP-VF-03](SOP-VF-03-design-system-theme.md)). Record theme id, theme version and contract version in `status.json → design_system`.
2. **Start from the template:** `bin/apc template <Name> design`.
3. **Inventory:** list every parameter, every display (meters, readouts), and every UI-only action (clear, preset). Each must appear in the control map or be marked "no widget" with a reason.
4. **Choose components** from `components.md`: knob (size by importance: one `xl` hero max), toggle (standard vs **caution** role for bypass/mute), segmented (2–5 options) vs dropdown, meter width, readouts. Bipolar parameters get a knob origin at neutral.
5. **Lay out on the 4 px grid:**
   - Standard header first (40 px): glyph, product name, brand, preset browser, version, bypass. Header needs ≈ 470 px of width; set the window width from that.
   - Signal flow left → right, top → bottom; group by stage in section panels (padding 12, gap 16).
   - Give exact rects (x, y, w, h) for every element; draw an ASCII wireframe with edge coordinates.
6. **States table:** normal, bypassed, silence, hot/over (if metering), disabled, automating. Describe each with role names only.
7. **Interaction:** product-specific steps (wheel and arrow increments in units, typed-value formats), tab order following signal flow.
8. **Accessibility:** screen-reader text per control; confirm colour is never the only signal; all targets ≥ 24 px.
9. **Implementation notes:** editor = layout + attachments only; `factory_gui_use_theme(<Name> THEME <id>)`; re-theme test.
10. **Review checklist** (below), then set `validation.design_complete` true, `current_phase` `design`, and request `approve_gui` in the factory ([SOP-VF-07](SOP-VF-07-factory-operations.md)).

## Review checklist

- [ ] Every parameter-spec id appears in the control map.
- [ ] No hex values, no font names, no pixel sizes that bypass tokens (except layout rects).
- [ ] Every rect sits on the 4 px grid and inside the window.
- [ ] Only one `active` primary action per view; bypass uses the caution role.
- [ ] Meter colours appear only on meters (theme policy).
- [ ] Tooltips written for every interactive control (name · description · range/default/gesture).
- [ ] Tab order listed.

## Definition of done

- `bash tools/apc-gate.sh <Name> impl` exits 0.
- `python3 vst/factory-lib/design-system/scripts/check_tokens.py --theme <id>` exits 0.
- `approve_gui` requested (pending or approved) in the factory.

## Templates

`templates/apc/design-spec.template.md` · example: `plugins/MeterTap/.ideas/design-spec.md`

## Related

[SOP-VF-03 theme](SOP-VF-03-design-system-theme.md) · [design-system ARCHITECTURE](../../../vst/factory-lib/design-system/ARCHITECTURE.md) · [.agents/skills/design/SKILL.md](../../../.agents/skills/design/SKILL.md)
