# Claude → Cursor handoff (copy into GitHub issue body)

Use this when Claude (or another external LLM) finishes a spec and you want a **Cursor Cloud Agent** to implement it in this repo.

## Goal

[One sentence outcome]

## Context

- Read: `/ARCHITECTURE.md`, product `ARCHITECTURE.md`, `AGENTS.md`, `.cursor/rules/*`
- Branch naming: `cursor/<short-description>-453b` (Cloud Agent suffix if required)

## Requirements

1. …
2. …

## Out of scope

- …

## Success criteria

- [ ] Build/test commands pass (see product doc)
- [ ] No secrets in git; document env names in `.env.example` only
- [ ] Draft PR only — do not merge or deploy production

## Security

- Validate HTTP inputs; enforce auth on user-owned data; generic client errors

## Attachments

- Links, screenshots, preset JSON, or prior Claude transcript excerpts

---

After the issue is ready, add the label **`cursor-agent`** to start the GitHub Actions handoff (see [docs/CURSOR_CLAUDE_AUTOMATION.md](../CURSOR_CLAUDE_AUTOMATION.md)).
