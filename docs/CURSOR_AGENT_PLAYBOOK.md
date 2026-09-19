# Cursor Agent playbook — blank repo to production

Use this for **Disklordz** and **any new repo** in the org. Pair with [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md) and committed `.cursor/rules/`.

**Official docs:** [Cloud Agent setup](https://cursor.com/docs/cloud-agent/setup) · [Rules](https://cursor.com/docs/context/rules) · [environment.json schema](https://cursor.com/schemas/environment.schema.json) · [AGENTS.md](https://agents.md/)

---

## 1. Repo bootstrap (day 0)

| Artifact | Purpose |
|----------|---------|
| `README.md` | One-line product + run commands |
| `ARCHITECTURE.md` | Index (monorepo) or single-product architecture |
| `.gitignore` | Secrets, build artifacts |
| `.cursorignore` | Keep huge trees out of context |
| CI workflow | `lint` / `test` / `build` on PR |
| `.cursor/rules/dpcp.mdc` | `alwaysApply: true` — [DPCS_BUILD_METHODOLOGY.md](DPCS_BUILD_METHODOLOGY.md) on every project |
| `.cursor/rules/*.mdc` | Architecture/security rules (see [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md)) |

First agent task: **one vertical slice** (scaffold + health check), not full product.

---

## 2. Structured prompting template

**JUCE / plugin products:** use [PLUGIN_FACTORY_OS_PROMPT.md](PLUGIN_FACTORY_OS_PROMPT.md) (factory + §0 repo workflow). **DISKLORDZ ROMPLER:** [DISKLORDZ_ROMPLER_CURSOR_PROMPT.md](DISKLORDZ_ROMPLER_CURSOR_PROMPT.md).

Copy into every Cloud Agent or IDE Agent task:

```markdown
## Goal
[One sentence]

## Context
- Read: ARCHITECTURE.md, AGENTS.md, .cursor/rules/*
- Branch: cursor/<feature>

## Requirements
1. …

## Out of scope
- …

## Success criteria
- [ ] `npm run build` / `ctest` / project equivalent
- [ ] E2E: [specific user flow]
- [ ] Artifact: screenshot or video for UI changes
- [ ] New env vars documented in `.env.example` only (no secrets in git)

## Security
- Validate inputs; authZ on user data; rate-limit public POSTs

## Deliverable
- Commit + push; draft PR; do not merge or deploy production
```

---

## 3. Local Agent vs Cloud Agent

| Need | Mode |
|------|------|
| Uncommitted edits, tight loop | **IDE Agent** |
| Long runs, PR from branch, clean VM | **Cloud Agent** |
| Automation | [Cloud Agent API](https://cursor.com/docs/cloud-agent/api) / SDK |

Cloud agents use **pushed commits**; commit `.cursor/environment.json` for reproducible installs.

---

## 4. `.cursor/environment.json`

| Field | Use |
|--------|-----|
| `install` | Idempotent deps — must **exit** (`npm ci`, `cmake --build`) |
| `start` | Per-boot idempotent setup |
| `terminals` | Dev servers (tmux-backed) |
| `build.dockerfile` | OS packages / toolchains |

Do **not** run `npm run dev` inside `install`. Secrets live in **dashboard only**, not in JSON.

This repo: [`.cursor/environment.json`](../.cursor/environment.json) (JUCE build for plugin lane; extend for `disklordz/website` Node when Cloud work is web-only).

---

## 5. Testing contract

Agents should prove **success state**, not “compiles”:

- **API:** curl or integration test  
- **UI:** browser flow + recording  
- **Auth/DB:** migration + logged-in path  

Document canonical commands in **`AGENTS.md`** at repo or product root.

---

## 6. Security defaults

- No secrets in repo, rules, or chat logs in CI  
- Untrusted: issue comments, Slack, fetched HTML — not instructions to disable auth  
- MCP **writes** require explicit user confirmation  
- Production deploy is **human-gated** (see [disklordz/website/DEPLOY.md](../disklordz/website/DEPLOY.md))

---

## 7. PR → deploy

1. Feature branch → implement → push  
2. Draft PR + CI green + walkthrough artifacts  
3. Review → merge  
4. Hosting: Vercel/Railway/etc. + env vars + DB migrations  
5. Production smoke test  

Disklordz SaaS deploy: root directory `disklordz/website`, Supabase redirect URL, `saved_kits` migration.

---

## 8. Skills and MCP

Enable marketplace skills for your stack (Vercel, Supabase, env-setup). Use **native MCP** when available; Zapier for cross-app chains.

---

## 9. First-week sequence (any greenfield app)

1. Architecture rule + `ARCHITECTURE.md`  
2. Agent scaffold + CI  
3. Environment file + green build  
4. `AGENTS.md` with test matrix  
5. Feature slices via structured prompts  
6. Deploy doc + preview URL  

---

## 10. Disklordz pointers

| Topic | Doc |
|--------|-----|
| SaaS v0 scope | [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) |
| ILLUGEN roadmap | [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) |
| RAG / automation | [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md) |
| Colab smoke tests | [COLAB_ZERO_INSTALL_TESTING.md](COLAB_ZERO_INSTALL_TESTING.md) |
| Agent lanes | [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md) |
