# SOP-HERMES-001 — Seat routing and dispatch

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-lead |
| **Consumer seats** | hermes-lead, hermes-ops |
| **Cadence** | every_task |

## Purpose

Route each task to the correct Hermes seat so work is parallel, evidenced, and WO-aligned.

## Trigger

Any implementation request in the Instruments monorepo (plugins, SaaS, ops, research).

## Procedure

1. Read [docs/HERMES_AGENT_FRAMEWORK.md](../../../docs/HERMES_AGENT_FRAMEWORK.md) and [.cursor/hermes/SKILLS_REGISTRY.md](../../../.cursor/hermes/SKILLS_REGISTRY.md).
2. Classify work:

   | Work type | Primary seats |
   |-----------|----------------|
   | Junova-X / JD / Wave909 DSP | hermes-dsp, hermes-architect |
   | Plugin UI | hermes-gui, hermes-qa |
   | SaaS Next.js | hermes-web, hermes-data, hermes-qa |
   | CI / GitHub Actions | hermes-devops |
   | WO dispatch / WIP | hermes-ops |
   | Antigravity / HISE | hermes-handoff |
   | Store copy (draft) | hermes-gtm |
   | Presets | hermes-presets |
   | RAG / FAQ | hermes-support |
   | Pre-merge secrets | hermes-security |
   | New or broken procedure | **hermes-sop** |

3. Dispatch Cursor Task with prefix `hermes-<seat>:` and require skill read at start.
4. Cap **≤ 2** concurrent Cursor **JUCE** WOs; do not mix SaaS + plugin in one PR ([SOP-OPS-001](SOP-OPS-001-work-order-lifecycle.md)).
5. Link applicable SOPs from [README.md](README.md) in PR body when non-trivial.

## Verification

- Named implement seat in PR/issue.
- Correct product `ARCHITECTURE.md` cited in PR.

## Related

- [SOP-OPS-001](SOP-OPS-001-work-order-lifecycle.md)
- `.cursor/rules/hermes-default.mdc`
