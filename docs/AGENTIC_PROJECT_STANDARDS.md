# Agentic project standards (reusable)

Copy this checklist when starting **any** app repo (SaaS, plugin, tools). Commit standards to git so **Cursor Cloud** and **local Agent** behave the same.

---

## Required files

| File | Content |
|------|---------|
| `ARCHITECTURE.md` | Purpose, build/run, data flow, key modules, extension points |
| `.cursor/rules/architecture-documentation.mdc` | `alwaysApply: true` — agents read ARCHITECTURE first |
| `.cursor/rules/security-baseline.mdc` | No secrets in git; validate inputs; authZ |
| `AGENTS.md` | Test commands, deploy steps, secret **names**, branch policy |
| `.env.example` | All required env vars, no values |
| `.github/workflows/ci.yml` | Build + test on PR |

Optional: copy [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) or link from root README.

---

## Branch and PR policy

- Feature branches: `cursor/<description>-<suffix>` (Cloud Agent convention) or team standard  
- One logical commit per change when possible  
- Draft PR + artifacts before merge  
- No force-push / no prod deploy unless explicit  

---

## Rule templates

### Security baseline (`.cursor/rules/security-baseline.mdc`)

```markdown
---
description: Security baseline for all code changes
alwaysApply: true
---
- Never commit API keys, tokens, or .env values.
- Validate and bound all public API inputs.
- Return generic errors to clients; log details server-side.
- Rate-limit unauthenticated expensive endpoints.
```

### Stack rule (example: Next.js)

```markdown
---
description: Next.js App Router conventions for this repo
globs: "**/*.{ts,tsx}"
---
- Use server components by default; client only when needed.
- API routes: Zod (or similar) parse body; return typed JSON errors.
```

---

## Work order discipline (Disklordz OS)

1. Airtable **Agent Work Order** with `work_order_id`  
2. GitHub issue + PR title contains `WO-…`  
3. Merge → Airtable Done  
4. Backlog for SaaS: [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) (007+)  

---

## Definition of done (agent)

- [ ] Success criteria from prompt met with **evidence** (test output, video, URL)  
- [ ] `ARCHITECTURE.md` updated if structure changed  
- [ ] No debug logging left in production paths  
- [ ] `.env.example` updated for new configuration  
- [ ] **JUCE products:** manifest + `Docs/GATES.md` + [PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md) release status honest (VERIFIED / NOT VERIFIED)

---

## Multi-project monorepo

Root `ARCHITECTURE.md` **indexes** products; each product has its own `ARCHITECTURE.md` (see [architecture-documentation rule](../.cursor/rules/architecture-documentation.mdc)).

This **Instruments** repo: JD Upgraded, tools, HISE lane, `disklordz/website`, `disklordz/rag`, automation.
