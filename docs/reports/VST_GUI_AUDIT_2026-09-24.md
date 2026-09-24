# VST GUI audit — 2026-09-24

**Reviewer:** `vst-gui-designer` (initial team onboarding audit)  
**Branch reviewed:** `cursor/prophet-rev2-trap-ddc8`  
**Rubric:** [VST_GUI_DESIGNER_AGENT.md](../VST_GUI_DESIGNER_AGENT.md)

## Executive summary

Night Circuit is the **production reference** for the monorepo GUI system after the 1280×820 migration. WAVE-909 and JD Upgraded remain **legacy layout** (fixed or ad-hoc sizing) and need specs + migration WOs before any SKU claims. No P0 defects found in Night Circuit standalone smoke review; host matrix still NOT TESTED.

## Product scores

| Product | Verdict | Default / min size | P0 | P1 | P2 |
|---------|---------|-------------------|----|----|-----|
| **Night Circuit** | Pass with notes | 1280×820 / 1120×760 | 0 | 2 | 3 |
| **WAVE-909** | Fail (SKU GUI debt) | 720×420 fixed | 0 | 3 | 2 |
| **JD Upgraded** | Pass with notes | Complex multi-panel | 0 | 2 | 4 |
| **MyFirstPlugin** | N/A (template) | — | — | — | — |

---

## Night Circuit (`ProphetRev2Trap/`)

**Verdict:** Pass with notes  
**Evidence:** Standalone capture 2026-09-24; `NIGHT_CIRCUIT_GUI_SPEC.md` status table updated.

**Strengths**

- Full-width preset browser: category, preset, search, prev/next, Featured/Favorites, star, save-to-user.
- Spec palette (graphite / panel / violet) in `paint()` section cards.
- Modified badge via APVTS baseline; disk favorites (`FavoritesStore`).
- Resize limits match spec minimum.

**P1 (before Milestone 1 host sign-off)**

1. Host GUI checklist empty — FL Studio / MPC Software NOT TESTED (`HOST_COMPATIBILITY.md`).
2. Keyboard focus order not documented or validated (`EDITOR_WANTS_KEYBOARD_FOCUS` enabled).

**P2 (backlog)**

1. Knob readouts lack ms / cents / Hz labels per spec synthesis section.
2. Filter cutoff normalized 0–1 in UI — consider Hz display WO with Factory DSP agreement.
3. Sound-design featured count copy should stay synced (30 foundations).

**Factory follow-ups:** `[Plugin][JUCE]` WOs for P1 host matrix; `[Plugin][JUCE][GUI]` for keyboard focus spec.

---

## WAVE-909 (`Wave909/`)

**Verdict:** Fail (legacy — not production GUI standard)

**Findings**

- Fixed **720×420**; no `setResizeLimits`; no preset browser pattern for expanding library.
- Custom LAF present but no shared tokens from `VST_GUI_SYSTEM.md`.
- No `Wave909/design/WAVE909_GUI_SPEC.md`.

**PM recommendation:** Open `WO-2026-JUCE-GUI-W909-SPEC` → `vst-gui-designer` (spec + DESIGN.md), then Factory implementation WO.

---

## JD Upgraded (`Source/PluginEditor.*`)

**Verdict:** Pass with notes (mature, different domain)

**Strengths**

- Program prev/next; structured sections; ROM/program UX.

**P1**

1. No monorepo GUI spec file; tokens differ from Night Circuit (acceptable for SKU, document in `Source/UI/ARCHITECTURE.md`).
2. Minimum host sizes not recorded in one checklist.

**P2:** Align typography/rotary styles when cross-product brand unification is requested.

---

## Cross-repo actions (pm-agent)

| Priority | WO suggestion | Owner |
|----------|---------------|--------|
| P1 | `[Plugin][JUCE][GUI][Review]` Night Circuit — FL/MPC GUI checklist template | night-circuit-qa + vst-gui-designer |
| P1 | `[Plugin][JUCE][GUI]` WAVE-909 spec + resize target | vst-gui-designer → factory |
| P2 | `[Plugin][JUCE][GUI]` Monorepo token doc sync in JD UI ARCHITECTURE | vst-gui-designer |
| Recurring | Fri GUI audit handoff | vst-gui-designer |

## Sign-off

This audit establishes baseline ownership for **vst-gui-designer**. Next scheduled review: see `fri-vst-gui-audit` in `scheduled_work.yaml`.
