# {{PLUGIN_NAME}} — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design {{PLUGIN_NAME}}` 
**SOP:** [SOP-VF-04 Plan](../../../docs/disklordz/sops/SOP-VF-04-plan.md)

## Complexity assessment (spec §8)

| Dimension | Score (0–3) | Notes |
|-----------|-------------|-------|
| Voices | | |
| Oscillators/voice | | |
| Filters | | |
| Modulation | | |
| FX | | |
| Surfaces | | |
| Targets | | |
| **Total** | | |

**Tier:** L_ (L0 0–4 · L1 5–8 · L2 9–13 · L3 14–21)

**Risk flags (§8.3):** none | list

**Plugin-specific risks**

| Risk | Mitigation |
|------|------------|
| | |

## Tier override (only if needed)

`OVERRIDE(spec §8.2 …): <reason>` — and the structure used instead.

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `juce-native` |
| Design-system theme | `{{THEME}}` |
| Rationale | |
| Implementation strategy | factory-lib design-system components + thin editor |

## Phase order from here

| Step | Command / artifact | Gate |
|------|--------------------|------|
| Design | `/apc-design {{PLUGIN_NAME}}` → `.ideas/design-spec.md` | `tools/apc-gate.sh {{PLUGIN_NAME}} design` |
| Factory approval | `approve_architecture` (+ `approve_gui` after Design) | Slack `#disklordz-factory-dept` |
| Contract (DPCS 2) | headers in architecture §9 | compile exit 0 |
| Implement (DPCS 3) | one module per run | per-run allowlist |
| Verify (DPCS 4) | architecture §7 | all exit 0 |
| Ship | `/apc-ship {{PLUGIN_NAME}}` | `tools/apc-gate.sh {{PLUGIN_NAME}} ship` + `approve_release` |
