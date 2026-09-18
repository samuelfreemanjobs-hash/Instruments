# TR-808 recreation — monorepo handoff

**Path:** `vst-tr808/`  
**Shipping name:** **TBD** (see [docs/PRODUCT_NAMING.md](docs/PRODUCT_NAMING.md))  
**Vision:** Physical **circuit emulation** (16 voices, four architectural classes) — **not** sample playback. Full spec: [docs/circuit-emulation-spec.md](docs/circuit-emulation-spec.md).

## Identity (until name lock)

| Field | Value |
|-------|--------|
| Product (working) | TR-808 recreation |
| `product_id` (seed) | `DL-PLUGIN-TR808` |
| Division | Plugin |
| Stack (target) | **JUCE 8** · VST3 · **CLAP** · AU/AAX later per GTM |
| DSP | WDF (MNA bridged-T), state-space ODE (pulse shaper, VCAs), continuous Schmitt oscillators |
| Prototyping | FAUST → vectorized C++ integration per design spec |
| HISE lane | **Out of scope** — circuit DSP belongs in JUCE factory, not Antigravity sketch |

## Status

- **Spec:** circuit-emulation design brief captured (2026).
- **MVP code:** not started; gated behind Junova-X host milestone unless Planner overrides.
- **MVP voices:** BD → SD → CH/OH → CP, then remainder per [plugin-spec-mvp.md](plugin-spec-mvp.md).

## Priority

**P1** factory track B — parallel **spec/DSP design** with Junova-X **P0**; implementation WOs after Junova WO-2026-001 smoke or explicit override.

## Related

- [docs/DISKLORDZ_PLUGIN_TRACKS.md](../docs/DISKLORDZ_PLUGIN_TRACKS.md)
- Airtable: `tr808-circuit-product-2026.json`

## Branch

**Canonical:** `cursor/novadrum-juce-handoff-029a` — NovaDrum only; Junova-X on `cursor/junova-x-handoff-029a`.
