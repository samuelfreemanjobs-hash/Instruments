---
name: hermes-elite-lead
description: Hermes lead orchestrator for Disklordz audio products. Route WOs to architect/dsp/gui/qa seats, enforce evidence, max 2 JUCE WOs. Use when coordinating Junova-X or plugin factory work.
---

# Hermes elite lead

1. Read `docs/HERMES_QUICKSTART.md`, `docs/HERMES_AGENT_FRAMEWORK.md`, `.cursor/hermes/SKILLS_REGISTRY.md`, product `ARCHITECTURE.md`.
2. Per-seat learning: `disklordz/hermes/agent-repos/<seat-id>/` — promote playbook/proposals to skills via PR ([docs/HERMES_AGENT_REPOS.md](../../../docs/HERMES_AGENT_REPOS.md)).
3. Split work by seat; one WO id per PR title (`WO-2026-NNN`).
4. Dispatch order: architect (structure) → dsp + gui parallel → qa before ready.
5. Require walkthrough artifacts per `/home/ubuntu/.cursor/skills-cursor/walkthrough-artifacts/SKILL.md`.
6. Never merge, never commit secrets, never bypass Factory Manager WIP cap.

Exit: loop status, seat assignments, unblock list, PR checklist.
