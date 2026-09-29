# Product requirements document (PRD) — template

**Owner agent:** `code-project-planner`  
**Rule:** No implementation PR until this PRD is linked from the product `ARCHITECTURE.md` or `docs/products/<id>.md`.

## 1. Summary

One paragraph: who uses it, what ships, why now.

## 2. Problem / opportunity

- Current pain
- Revenue or quality lever

## 3. Goals & non-goals

**Goals**

1. …

**Non-goals**

- …

## 4. Users & scenarios

| Persona | Scenario | Success signal |
|---------|----------|----------------|
| Producer | … | … |

## 5. Functional requirements

1. …
2. …

## 6. Technical approach

- **Stack:** (Python / JUCE / Next.js / …)
- **Key modules:** paths
- **Fleet agents:** APC, DDSP, TrapForge, …
- **Verify commands:** `[executed]` checklist

## 7. Success criteria (release gate)

- [ ] Automated tests pass (name workflows)
- [ ] Manual / demo artifact (video or WAV)
- [ ] `ARCHITECTURE.md` updated
- [ ] No secrets in git

## 8. Phases

| Phase | Scope | Exit |
|-------|-------|------|
| 0 | PRD + ARCHITECTURE | Review |
| 1 | Vertical slice | CI green |
| 2 | … | … |

## 9. Risks & open questions

- …

## 10. Memory & docs

- Product index: [`docs/products/`](products/)
- Company index: [`COMPANY_MEMORY_INDEX.md`](../COMPANY_MEMORY_INDEX.md)
- RAG: reindex `docs/**` after merge
