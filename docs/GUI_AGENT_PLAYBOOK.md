# GUI Agent playbook — Cursor

The **GUI Agent** is a Cursor **Cloud Agent** or **IDE Agent** run with a fixed mandate: implement and verify user interfaces from **`GUI_MOCKUP_SPEC.md`**, following [GUI_DESIGN_SOP.md](GUI_DESIGN_SOP.md).

It is not a separate binary — it is **how you prompt and constrain** any agent session that touches UI.

---

## When to invoke the GUI Agent

- New screen, panel, or major layout change  
- After **Stitch / Figma / Gemini** mockups are ready (or after spec lock)  
- Re-skin or accessibility pass on existing UI  
- Wiring buttons to new APIs (factory generate, export stems, etc.)

**Do not** use the GUI Agent alone for pure backend or DSP — scope stays UI + thin API glue + docs.

---

## How to start a GUI Agent run (Cursor)

### Cloud Agent

1. Branch: `cursor/<feature>-<suffix>` (per cloud policy).  
2. Paste the **structured prompt** below into a new Cloud Agent task.  
3. Attach or link mockup PNGs under `references/gui/`.  
4. Require **screen recording** in success criteria.  

### IDE Agent

Same prompt; user runs `npm run dev` / `sequencer/serve.py` locally; agent uses browser tools if available.

### Optional: `@` context

- `@docs/GUI_DESIGN_SOP.md`  
- `@docs/GUI_AGENT_PLAYBOOK.md`  
- `@<product>/docs/GUI_MOCKUP_SPEC.md`  
- `@.cursor/rules/gui-design.mdc`  

---

## Structured prompt template (copy-paste)

```markdown
## Role
You are the **GUI Agent** for this repo. Follow docs/GUI_DESIGN_SOP.md and docs/GUI_AGENT_PLAYBOOK.md.

## Goal
[One sentence UI outcome]

## Context
- Product ARCHITECTURE: [path]
- GUI spec: [path to GUI_MOCKUP_SPEC.md] — Status: LOCKED vN (or DRAFT)
- Mockups: references/gui/[files]
- Stack: [e.g. static HTML in sequencer/static, Next.js App Router, etc.]

## Requirements
1. Implement sections and controls from the spec (IDs where listed).
2. Preserve existing API contracts; extend only if spec says so.
3. Match Memphis / phonk visual mood if applicable (dark, purple accent — or mockup colors).
4. Update GUI_MOCKUP_SPEC if behavior changed; bump LOCKED version.

## Out of scope
- Backend refactors unrelated to UI
- New product features not in spec

## Success criteria
- [ ] All spec controls present and functional
- [ ] Manual browser test + **screen recording** saved to /opt/cursor/artifacts/
- [ ] Screenshots of key frames (default tab, generate flow, error state if any)
- [ ] Tests: product test command (e.g. unittest / npm test) still green
- [ ] No secrets in client code

## Design handoff
- Gemini / Stitch / AI Studio were used for: [list frames or “none — spec only”]
- Open questions from spec: [paste or “none”]

## Deliverable
Commit, push, draft PR with artifact embeds.
```

---

## GUI Agent workflow (agent steps)

1. Read product `ARCHITECTURE.md` + **`GUI_MOCKUP_SPEC.md`**.  
2. Diff mockup vs current `static/` or `app/` — list gaps.  
3. Implement **smallest diff** that satisfies spec (match existing CSS patterns).  
4. Wire controls to existing APIs first; add API routes only if spec requires.  
5. **Test in browser** (computer use or local): happy path + one edge case.  
6. Record video; capture screenshots.  
7. Update spec status, `sequencer/ARCHITECTURE.md` or web ARCHITECTURE if routes added.  
8. PR with before/after images.

---

## Integration with Gemini, Stitch, AI Studio

| Tool | GUI Agent consumes |
|------|---------------------|
| **Gemini** | Copy deck (labels, hints), state matrix, a11y notes → paste into spec §Controls |
| **Google AI Studio** | Exported prompt results, JSON UI copy → spec appendix |
| **Google Stitch** | PNG frames → `references/gui/`; agent maps frames to sections |

**Human step:** After Stitch, add a table to the spec:

| Frame | File | Notes |
|-------|------|--------|
| Kick tab | phonk-v1-kick.png | Large steps, export buttons right-aligned |

GUI Agent implements to match; if mockup conflicts with API, **spec wins** until human updates mockup.

---

## Quality bar (GUI Agent definition of done)

Same as [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md) plus:

| Check | Required |
|-------|----------|
| Spec LOCKED or explicit DRAFT waiver in PR | Yes |
| Visual evidence (video or screenshot set) | Yes for non-trivial UI |
| Keyboard focus not completely broken | Yes |
| Status / loading feedback for network actions | Yes |
| Mobile | Best-effort unless spec says desktop-only |

---

## Example: Lofi-12 phonk factory

- **Spec:** `disklordz/lofi12-phonk-factory/docs/GUI_MOCKUP_SPEC.md`  
- **Code:** `disklordz/lofi12-phonk-factory/sequencer/static/`  
- **Run:** `python3 disklordz/lofi12-phonk-factory/sequencer/serve.py`  
- **Mockups:** `disklordz/lofi12-phonk-factory/references/gui/`  

GUI Agent tasks: restyle to Stitch mockup, add Phase 2 controls (stem mutes), per-step FX UI — **only after** spec/mockup updated.

---

## Escalation

- **Spec ambiguous:** GUI Agent documents assumption in PR, does not guess new features.  
- **API missing:** Add minimal endpoint + test, document in spec API table.  
- **Design system needed:** Propose `docs/GUI_TOKENS.md` (colors, spacing, type) in a separate PR.
