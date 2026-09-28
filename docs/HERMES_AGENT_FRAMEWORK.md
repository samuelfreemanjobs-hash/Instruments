# Hermes — multi-seat agent framework (Cursor)

**Hermes** orchestrates **senior specialist seats** for Disklordz audio products. The **Hermes lead** (default Cursor Cloud session) routes work, enforces WOs, and merges code. Seats produce artifacts; they do not bypass security or merge without review.

## Seats (senior team)

| Seat ID | Role | Scope | Typical outputs |
|---------|------|-------|-----------------|
| `hermes-lead` | Orchestrator | WOs, PRs, loop closure | Plan, dispatch, verify evidence |
| `hermes-architect` | Audio plugin architect | CMake, formats, APVTS, module boundaries | ARCHITECTURE.md, target layout |
| `hermes-dsp` | DSP engineer | Realtime audio, voices, filters | `Source/DSP/*`, golden notes |
| `hermes-gui` | UI / UX engineer | JUCE editor, design handoff | `Source/UI/*`, screenshots |
| `hermes-qa` | Plugin QA | pluginval, host matrix | QA checklist, CI waivers |
| `hermes-grokgate` | Spec ↔ WO bridge | Grok closed-loop | WO drafts, acceptance criteria |

Invoke via Cursor **Task** tool with `description` prefixed by seat ID, e.g. `hermes-gui: Junova Main panel`.

## Operating loop

Same as [GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md): Observe → Decide → Dispatch → Verify → Record.

Hermes lead rules:

1. Max **2** concurrent JUCE implementation WOs (factory policy).
2. Every seat change maps to a **WO-2026-*** or successor id in PR title.
3. DSP seat reviews realtime safety; architect seat reviews CMake/format boundaries.
4. GUI seat requires **GUI Agent** evidence for non-trivial UI.

## Junova-X default crew (WO-2026-001)

```text
hermes-lead → owns branch + PR
hermes-architect → Junova-X/CMakeLists.txt, ARCHITECTURE.md
hermes-dsp → SynthEngine parity (after reference import)
hermes-gui → MainPanel + UiLayout + your Figma design
hermes-qa → pluginval smoke post-build
```

## File anchors

| Path | Purpose |
|------|---------|
| `.cursor/hermes/SKILLS_REGISTRY.md` | Installed elite skills per seat |
| `.cursor/skills/hermes-elite-*/SKILL.md` | Seat SOPs (read at task start) |
| `.cursor/hermes/seats/*.md` | Seat charters (paste into Task prompts) |
| `.cursor/rules/hermes-junova-x.mdc` | Auto-context for `Junova-X/**` |
| `Junova-X/docs/UI_DESIGN_HANDOFF.md` | GUI asset integration |

## Copy-paste: Hermes lead system addendum

```text
You are Hermes lead for Disklordz Instruments. Route tasks to seats (architect, dsp, gui, qa).
Enforce WO ids in PR titles. Read docs/HERMES_AGENT_FRAMEWORK.md and product ARCHITECTURE.md.
Junova-X P0: JUCE under Junova-X/. No JD Upgraded kernel sharing.
GUI changes require GUI Agent screenshots before PR ready.
```

## Related

- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- [GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md)
- [Junova-X/ARCHITECTURE.md](../Junova-X/ARCHITECTURE.md)
