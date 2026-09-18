# Claude Code integration

How **Claude Code** (terminal / IDE) works alongside **Cursor Cloud**, **Cursor IDE**, and **Antigravity** in this monorepo.

Entry file for Claude: **[CLAUDE.md](../CLAUDE.md)** (same role as **AGENTS.md** for Cursor).

---

## Why both Cursor and Claude

| Tool | Strength here |
|------|----------------|
| **Cursor Cloud** | Long clean VM builds, draft PRs, Plugin Factory runs, scheduled agents |
| **Claude Code** | Local or Claude-hosted long sessions, multi-file refactors, second lane on another product |
| **Cursor IDE** | Same repo, uncommitted loop, DAW on your machine |
| **Antigravity** | Windows HISE only |

**Rule:** One **active implementing agent** per `repoPath` at a time. Register ownership in [agent-registry.json](agent-registry.json).

---

## Setup (Claude Code)

1. Clone this repo (same remote as Cursor).
2. Open repo root — Claude loads **CLAUDE.md** automatically.
3. Before starting work, read **agent-registry.json** and your product `ARCHITECTURE.md`.
4. Create branch: `claude/<short-description>-<suffix>` (mirror Cursor’s `cursor/*` policy).
5. Push and open a **draft PR**; paste link into registry `claudeActiveProducts` row.

Optional: add Claude’s project instructions pointing to [PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md) for new plugins.

---

## The two Claude products (you maintain this list)

Update **`claudeActiveProducts`** in [agent-registry.json](agent-registry.json) with the real names, paths, and PRs. Example shape:

```json
{
  "id": "my-product",
  "name": "Display name",
  "repoPath": "SomeFolder/",
  "branch": "claude/my-product-abc1",
  "githubIssueOrPr": "https://github.com/.../pull/N",
  "ownerHuman": "you@email"
}
```

If a Claude product lives on a **branch not yet merged** (e.g. VMPC, Character FM, Junova), set `repoPath` to the folder that branch owns once you know it, or use the scaffold on `main` until the first PR lands.

**Claude primary on `main` today (see registry):**

| Product | Path |
|---------|------|
| SP-1200 VST | `SP1200/` |
| Pluggnb Vital preset pack (1st preset SKU) | `disklordz/packs/vital/` |

**Do not** let Claude and Cursor edit the same folder on different branches without coordinating — merge or rebase one line of work first.

---

## Handoff: Cursor ↔ Claude

| Direction | Do this |
|-----------|---------|
| **Cursor → Claude** | Merge or pause Cursor PR; update registry; Claude checks out branch; read `Docs/GATES.md`, manifest, PR description |
| **Claude → Cursor** | Push `claude/*`; draft PR with VERIFIED/NOT VERIFIED; @ Cursor Cloud with “continue from PR #N” |
| **Either → Antigravity** | `./scripts/antigravity-bridge/antigravity-bridge.sh send --wo …` — HISE only |

Shared artifacts (not chat memory):

- `plugin.manifest.json`  
- `Docs/PRODUCT_SPEC.md`, `Docs/GATES.md`, `Docs/VALIDATION_REPORT.md`  
- `test.plan.json` (optional)

---

## Branch and PR policy

- Prefixes: `cursor/`, `claude/`, `agent/` (see registry).  
- Same quality bar: [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md).  
- Neither tool merges to `main` or deploys Disklordz prod without the owner.

---

## Where Claude should **not** work

- **HISE export** on Linux (use Antigravity lane).  
- Claiming **DAW / pluginval / CPU** without running or marking NOT VERIFIED.  
- Renaming products away from manifest `pluginName`.  
- Stripe/Supabase/Zapier **writes** without explicit user confirmation.

---

## Suggested split (Instruments + Disklordz)

| Product area | Default primary | Claude good for |
|--------------|-----------------|-----------------|
| WAVE-909 / Plugin Factory | Cursor Cloud | Preset expansion, docs, offline tests locally |
| JD Upgraded | Cursor | Risky DSP — coordinate single PR |
| Disklordz website | Either | Feature WOs, API routes, UI — **staging only** |
| RAG scripts | Either | Corpus + chunk scripts |
| HISE sketches | Antigravity | **Not Claude** on export path |

Adjust primary agent in **agent-registry.json** when you dedicate Claude to your two products.

---

## Checklist: “Claude is in the business”

- [ ] **CLAUDE.md** committed (done)  
- [ ] **agent-registry.json** — fill `claudeActiveProducts` with your 2 products + PR links  
- [ ] **AGENTIC_OPERATING_MODEL.md** — Claude row acknowledged (done)  
- [ ] One **draft PR per active Claude product**  
- [ ] Owner weekly: resolve duplicate branches on same `repoPath`

---

*See also [AGENTIC_OPERATING_MODEL.md](AGENTIC_OPERATING_MODEL.md) · [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)*
