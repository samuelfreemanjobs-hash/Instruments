# {{PLUGIN_NAME}} — architecture (APC Plan)

**Phase:** APC Plan · between DPCS Phase 1 and Phase 2 
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md) 
**Normative:** [dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · [DPCS.md](../../../docs/DPCS.md) 
**SOP:** [SOP-VF-04 Plan](../../../docs/disklordz/sops/SOP-VF-04-plan.md)

No code here. Type and file names are Phase 2 contract targets.

## 1. Core components

| Component (spec §3.1) | Lives in | Role |
|-----------------------|----------|------|
| Context | `vst/{{PLUGIN_NAME}}/Source/PluginProcessor` | sample rate, block size, layout |
| ParameterBank | APVTS | params from parameter-spec v1 |
| <DSP block> | `vst/factory-lib/<area>/<Block>.h` or `shared/dsp/` | |
| Editor | `vst/{{PLUGIN_NAME}}/Source/PluginEditor` | layout + attachments only (design-system components) |

Reuse first: list existing `shared/dsp/` and `vst/factory-lib/` blocks before proposing new ones. New reusable blocks go to factory-lib ([SOP-VF-10](../../../docs/disklordz/sops/SOP-VF-10-reusable-module.md)).

## 2. Program chain (audio thread, per `processBlock`)

```text
for each block (numSamples 0..maxBlockSize):
  1. ScopedNoDenormals
  2. Sanitize non-finite input
  3. ...
```

Decisions (each with the reason):

-

## 3. Surface chain (message thread)

| Surface | Binding |
|---------|---------|
| APVTS | |
| Editor timer | |
| State | APVTS XML |

## 4. Global layer

| Item | Value |
|------|-------|
| Bus layout | |
| prepareToPlay | |
| releaseResources | |
| Programs | |
| Plugin codes | Manufacturer `Inst`, plugin `<4 chars>` (checked unique) |

## 5. Threading and realtime contract

- Audio thread may:
- Audio thread must not: allocate, lock, log, touch GUI.

## 6. Build wiring (Implement phase)

- `./tools/new-vst.sh {{PLUGIN_NAME}} --type {{PLUGIN_CLASS}}`
- `target_include_directories(... ../factory-lib)`; `factory_gui_use_theme({{PLUGIN_NAME}} THEME {{THEME}})`
- Windows: `COPY_PLUGIN_AFTER_BUILD` off

## 7. Verification plan (DPCS Phase 4)

| # | Check | How | Exit-0 criterion |
|---|-------|-----|------------------|
| V1 | | | |
| V7 | Build | `tools/build-vst.sh {{PLUGIN_NAME}}` | exit 0 |
| V8 | Factory QA | `run_business.py --profile factory --plugin {{PLUGIN_NAME}}` | exit 0 |
| V9 | pluginval | Windows, operator online | pass |

## 8. Open issues for Design / operator

Contradictions or gaps found in Phase 1 docs, each with a chosen default:

- **A.**

## 9. Phase 2 (Contract) allowlist

-
