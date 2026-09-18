# Agentic operating model — Instruments + Disklordz

One page. **Hybrid:** agents build and document; humans own taste, money, and production.

---

## 1. Two product lanes

| Lane | Products | Ship shape | Source of truth |
|------|----------|------------|-----------------|
| **JUCE factory** | JD Upgraded, WAVE-909, future `Wave*/` plugins | VST3 + CLAP + Standalone, root CMake CI, golden/offline render | [PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md), `<product>/plugin.manifest.json`, `Docs/GATES.md` |
| **Disklordz** | Drum SaaS (`disklordz/website/`), RAG, automation | Next.js on Vercel, Supabase RLS | [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md), [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) |
| **HISE sketch** (parallel R&D) | Rompler/sampler experiments | Local VST3 from HISE export — **not** monorepo CI until port WO | [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md), `hise-sketch/` |

**Rule:** HISE lane does not silently become JUCE lane. Promotion = explicit **port WO** or new Plugin Factory manifest.

---

## 2. Where work runs

| Environment | Use for | Do not use for |
|-------------|---------|----------------|
| **Cursor Cloud Agent** | Long JUCE builds, Plugin Factory runs, SaaS PRs from `cursor/*`, docs/CI, RAG scripts | Antigravity GUI, HISE export, Windows-only MSVC |
| **Claude Code** | **SP-1200 VST** (`SP1200/`), **Vital preset pack** (`disklordz/packs/sp1200-vital/`), other `claude/*` lanes | Same `repoPath` as active Cursor PR without handoff; HISE export on Linux |
| **Cursor IDE (local Agent)** | Tight loops, uncommitted DSP/UI, listening in a DAW on your machine | Pretending DAW/pluginval ran in Cloud |
| **VS Code + Antigravity** | HISE projects, Windows VST3 export, `hise-sketch/` | Root CMake CLAP CI, Disklordz deploy |
| **Antigravity bridge** | Async handoff Cursor ↔ Windows (`disklordz/antigravity/inbox|outbox/`) | Real-time pair programming |

Cloud default for **Instruments monorepo plugin/SaaS** per [AGENTS.md](../AGENTS.md). Antigravity for **`[Plugin][HISE]`** work orders only.

---

## 3. Roles (human + agent)

| Role | Owner | Delivers |
|------|--------|----------|
| **Owner / release** | You | Production deploy, pricing, merge to main, “ship it” |
| **PM agent** | Cursor | WO scope, `GATES.md` status, RELEASE BLOCKED vs RC/beta |
| **Factory / C++ agent** | Cursor Cloud or **Claude Code** (one per `repoPath`) | Manifest-locked code, CMake, tests, CI |
| **DSP / A&R** | You + agent draft | Presets, sonic sign-off; agent generates params/WAVs, **you listen** |
| **SaaS agent** | Cursor | `npm ci && build && test`, API/UI diffs; **no prod** without ask |
| **Marketing agent** | Cursor draft | Copy, hero scripts; **you publish** |
| **Workflow automation** | Cursor + GitHub | `build.yml`, golden verify, bridge scripts, optional Slack/Airtable |

Agents communicate in **git artifacts** (specs, manifest, validation report) — not chat-only.

---

## 4. Gates (all lanes)

| Gate | JUCE factory | Disklordz SaaS |
|------|----------------|----------------|
| **Spec** | manifest + PRODUCT_SPEC | Issue/WO + acceptance criteria |
| **Build** | `cmake --build` targets | `npm run build` |
| **Test** | ctest, offline render, golden | unit/e2e if present |
| **Security** | No secrets; untrusted presets | RLS, input bounds, rate limits ([security baseline](../.cursor/rules/security-baseline.mdc)) |
| **Evidence** | Logs / NOT VERIFIED table | Preview URL or test output |
| **Ship** | DAW smoke + pluginval (human/tool) before **retail** | Deploy per DEPLOY.md — **human only** |

Default agent conclusion: **`RELEASE BLOCKED`** until Gate “Ship” row is explicitly checked.

Preset counts (plugins): manifest `presetPolicy` — RC ≥8, beta ≥24, retail ≥48 (WAVE-909 reference).

---

## 5. Work intake → delivery

```text
Idea or Airtable WO
  → GitHub issue (WO id in title)
  → Branch cursor/<feature>-<suffix>  OR  claude/<feature>-<suffix>  OR  Antigravity inbox HO-*.json
  → Register active product in docs/agent-registry.json
  → Agent implements + pushes
  → Draft PR + evidence (test log / video for UI)
  → Human: review, DAW or staging click, merge
  → Production deploy (Disklordz): explicit only
```

Structured task template: [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) §2.

---

## 6. Tooling map (this repo)

| Need | Command / path |
|------|----------------|
| Plugin factory core | [PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md) |
| Validate manifest | `python3 docs/plugin-factory/validate_manifest.py <product>/plugin.manifest.json` |
| JUCE CI | `.github/workflows/build.yml` |
| SaaS build | `cd disklordz/website && npm ci && npm run build && npm test` |
| RAG | `python3 disklordz/rag/scripts/chunk_corpus.py` |
| Antigravity send | `./scripts/antigravity-bridge/antigravity-bridge.sh send --wo …` |
| Claude Code entry | [CLAUDE.md](../CLAUDE.md), [CLAUDE_CODE_INTEGRATION.md](CLAUDE_CODE_INTEGRATION.md) |
| Agent / product ownership | [agent-registry.json](agent-registry.json) |
| MCP writes (Stripe, Zapier) | **Confirm with owner before execute** |

---

## 7. What we are not doing

- Agents **merge to main**, **deploy prod**, or **post marketing** without you.
- Claiming **DAW compatibility**, **CPU %**, or **RELEASE READY** without VERIFIED evidence.
- Letting example prompts rename products (manifest `pluginName` wins).
- Running HISE export inside Cloud Agent (wrong VM).

---

## 8. Next habit (one loop)

Pick **one** WO → Cloud Agent → manifest/gates updated → draft PR → you run **one** human gate (DAW smoke *or* staging URL) → merge.

Repeat until boring; then add preset packs, SaaS WO-SAAS-007+, or second Plugin Factory product.

---

*Related: [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md) · [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) · root [ARCHITECTURE.md](../ARCHITECTURE.md)*
