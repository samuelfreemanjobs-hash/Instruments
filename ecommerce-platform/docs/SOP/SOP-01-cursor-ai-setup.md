# SOP-01 — Cursor / AI setup

## 1. Load project context

In Cursor chat, reference:

- `@ecommerce-platform/AGENTS.md`
- `@ecommerce-platform/docs/BLUEPRINT.md`
- Current SOP file for the phase

## 2. Enable ecommerce-builder skill

Path: `ecommerce-platform/.cursor/skills/ecommerce-builder/SKILL.md`

Prompt template for each task:

```text
Phase: [N from BLUEPRINT]
SOP: [SOP-XX name]
Goal: [one sentence]
Constraints: RBAC, Zod validation, use templates from templates/
Do not: change unrelated modules
Verify: npm run build in web/
```

## 3. Branch & commit discipline

- Branch: `cursor/<feature>-<suffix>` or team convention
- One SOP subsection per commit when possible

## 4. Code generation rules for AI

- Copy from `templates/` first; modify names/paths only as needed
- Server mutations → Server Actions or `app/api` per template
- Never commit `.env.local` or live API keys

## Definition of Done

- [ ] Skill file present and referenced in AGENTS.md
- [ ] Team agrees on prompt template above

## Verification

Ask the agent: "What phase of BLUEPRINT are we on?" — it should cite the doc.
