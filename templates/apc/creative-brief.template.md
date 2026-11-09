# {{PLUGIN_NAME}} — creative brief (DPCS Phase 1 PRD)

**Track:** APC greenfield · **Category:** `{{CATEGORY}}` · **Class:** {{PLUGIN_CLASS}} 
**Phase:** 1 — behaviour only. No C++, JUCE types, includes or file paths beyond the dependency list. 
**SOP:** [SOP-VF-01 Intake & Dream](../../../docs/disklordz/sops/SOP-VF-01-intake-dream.md)

## Elevator pitch

<!-- Two sentences: what it does and why someone reaches for it. -->

## User stories

1. As a <role>, I <action> so that <outcome>.
2. As a factory operator, I run factory CI on {{PLUGIN_NAME}} and get exit 0 without subjective listening.
3. As a debugger, I <observe a defined behaviour at an edge case>.

## Audio behaviour

### Topology

- **Inputs:**
- **Outputs:**
- **Processing order:** <!-- e.g. input gain → core → mix → output gain -->

### Controls

<!-- One subsection per control group: what happens across its whole range, smoothing, and what must NOT happen. -->

### Silence and extremes

| Condition | Expected output / display |
|-----------|---------------------------|
| All-zero input ≥ 1 s | |
| Full-scale input | |
| NaN / Inf at input | Output replaces non-finite samples with 0; no NaN at output |
| Sample rates 44.1–192 kHz, any block size incl. 0 and 1 | Identical behaviour within tolerance |

### Bypass

<!-- Bit-identical? What do meters/visuals show while bypassed? -->

### Latency

<!-- Samples of latency, lookahead, tail length. -->

## UI behaviour

<!-- What the user sees and does. Name design-system components (gui.meter, gui.rotary-knob...). No colours here: the Design phase picks roles. -->

## MIDI / sidechain

## Presets

- Factory: `Init` (+ list)

## Acceptance (Phase 4 — forward reference)

```bash
tools/build-vst.sh {{PLUGIN_NAME}}
python3 vst-testing-ops/run_business.py --profile factory --plugin {{PLUGIN_NAME}}
```

**Behavioural checks (numbered, measurable, with tolerances):**

1.
2.

## Out of scope (V1)

-

## Dependencies (conceptual only)

- DSP:
- GUI:

## Links

- Constitution: [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md)
- Parameters: [parameter-spec.md](parameter-spec.md)
