---
name: disklordz-code-project-planner
description: Audio Programmer–style PRDs and phase gates before implementation.
when_to_use: Greenfield product, new CMake target, or "what happened to project X?" memory gaps.
disable-model-invocation: false
context: fork
paths:
  - "docs/templates/PRD_TEMPLATE.md"
  - "docs/COMPANY_MEMORY_INDEX.md"
  - "docs/products/**"
  - "ARCHITECTURE.md"
  - "disklordz/agents/profit/code-project-planner/**"
---

# Skill — Code Project Planner

```bash
python3 disklordz/integrations/agents/code_project_planner.py --self-test
python3 disklordz/integrations/agents/code_project_planner.py --product voyager
```

## Success criteria

- [ ] PRD linked from product ARCHITECTURE.md
- [ ] Success criteria name exact test/build commands
- [ ] Updates `docs/COMPANY_MEMORY_INDEX.md` when adding products
