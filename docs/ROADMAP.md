# Instruments & Disklordz — roadmap and progress

Single place to see **where we are going** and **what is done**. PM Agent refreshes the progress block; human owners edit phase tables below.

**Related:** [disklordz/README.md](disklordz/README.md) (SOPs & templates) · [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) (SaaS 007+) · [PHASE5.md](PHASE5.md) (JD Upgraded plugin) · [HANDOFF.md](HANDOFF.md) (last agent handoff)

---

<!-- ROADMAP_PROGRESS_BEGIN -->
*Last updated:* 2026-09-28 16:42 UTC · regenerate: `python3 scripts/update-roadmap-progress.py`

| Metric | Value |
|--------|------:|
| **Profit agents (documented)** | 31 |
| **Agents with CI or runtime activation** | 17 / 31 |
| **OSS integrations (manifest)** | 35 total · 34 integrated · 0 partial · 1 external |
| **Fleet autopilot** | `agent-fleet-governance.yml` · `agent-fleet-execute.yml` · `agent-fleet-health.yml` (on `main` after merge) |

**Agent activation breakdown:** `ci_event` ×2, `ci_manual` ×2, `ci_on_pr` ×1, `ci_on_push` ×2, `ci_scheduled` ×2, `ci_script` ×1, `cli` ×1, `cli_local` ×1, `cli_script` ×1, `manual_doc` ×1, `manual_external` ×2, `manual_gate` ×1, `manual_skill` ×1, `manual_stub` ×4, `runtime_api` ×4, `runtime_build` ×1, `runtime_env` ×1, `runtime_inngest` ×2, `runtime_stripe` ×1
<!-- ROADMAP_PROGRESS_END -->

---

## Progress at a glance (milestones)

| Track | Current focus | Status |
|-------|---------------|--------|
| **Disklordz SaaS v0** | Auth, generate, ZIP, deploy | **Shipped** — see [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md) |
| **SaaS 007–016 (ILLUGEN-shaped)** | Spec, variations, factory, inbox | **Shipped in git** — prod secrets for async/Stripe/pgvector: [NEXT.md](NEXT.md) |
| **Agent fleet (31)** | Repo-wide docs + autopilot CI | **Shipped** — scheduled CI on `main` |
| **OSS integrations (35)** | manifest + website wiring | **34 integrated**, 1 external (Bytebot) — run `promote-integrations.py` after new stubs |
| **JD Upgraded plugin** | Phase 5 | See [PHASE5.md](PHASE5.md) |
| **WAVE-9090** | Trap wavetable synth | Active product — [Wave9090/ARCHITECTURE.md](../Wave9090/ARCHITECTURE.md) |

---

## Products — documentation audit

Every product must have `ARCHITECTURE.md` ([policy](../.cursor/rules/architecture-documentation.mdc)).

| Product | ARCHITECTURE | In root [ARCHITECTURE.md](../ARCHITECTURE.md) | Notes |
|---------|--------------|-----------------------------------------------|-------|
| JD Upgraded | [docs/ARCHITECTURE.md](ARCHITECTURE.md) · [Source/UI/](../Source/UI/ARCHITECTURE.md) | Yes | VST3/CLAP/standalone |
| Offline tools | [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md) | Yes | ROM, render, spectral diff |
| VST testing ops | [vst-testing-ops/ARCHITECTURE.md](../vst-testing-ops/ARCHITECTURE.md) | Yes | pluginval / CI dashboard |
| HISE sketch | [hise-sketch/ARCHITECTURE.md](../hise-sketch/ARCHITECTURE.md) | Yes | Antigravity lane |
| WAVE-9090 | [Wave9090/ARCHITECTURE.md](../Wave9090/ARCHITECTURE.md) | Yes | Sampleless trap synth |
| **Disklordz (umbrella)** | [disklordz/ARCHITECTURE.md](../disklordz/ARCHITECTURE.md) | Yes | Package index |
| Disklordz website | [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md) | Yes | Next.js SaaS |
| Sound factory | [disklordz/sound-factory/ARCHITECTURE.md](../disklordz/sound-factory/ARCHITECTURE.md) | Via disklordz index | Kit scripts |
| Disklordz RAG | [disklordz/rag/ARCHITECTURE.md](../disklordz/rag/ARCHITECTURE.md) | Yes | pgvector path optional |
| Integrations hub | [disklordz/integrations/ARCHITECTURE.md](../disklordz/integrations/ARCHITECTURE.md) | Yes | 35 OSS refs |
| Agent fleet | [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) · [disklordz/agents/](../disklordz/agents/ARCHITECTURE.md) | Yes | 31 agents |
| DAW inbox | [disklordz/daw-inbox/ARCHITECTURE.md](../disklordz/daw-inbox/ARCHITECTURE.md) | Yes | WO-016 |
| Antigravity | [disklordz/antigravity/ARCHITECTURE.md](../disklordz/antigravity/ARCHITECTURE.md) | Yes | Cursor bridge |
| META charter | [disklordz/agents/charter/ARCHITECTURE.md](../disklordz/agents/charter/ARCHITECTURE.md) | Yes | CLAUDE.md sync |
| Disklordz SOPs & templates | [docs/disklordz/ARCHITECTURE.md](disklordz/ARCHITECTURE.md) | Yes | Cursor/PM playbooks |

---

## Agent fleet — documentation & execution

| Layer | Location | Purpose |
|-------|----------|---------|
| **Company roster** | [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) | Root discovery |
| **PM ADD** | [disklordz/agents/workflows/PM_ADD.md](../disklordz/agents/workflows/PM_ADD.md) | Announcements |
| **Activation matrix** | [DISKLORDZ_AGENT_FLEET.md](DISKLORDZ_AGENT_FLEET.md) | ci vs runtime vs manual |
| **Machine registry** | [.github/agents/fleet.json](../.github/agents/fleet.json) | CI + tooling |
| **Specs (source)** | [disklordz/agents/profit/_specs.json](../disklordz/agents/profit/_specs.json) | Add agent here first |
| **Run now** | `./scripts/run-agent-fleet-now.sh` | Local execute all safe roles |

**Orchestration agents:** `workflow-automation`, `pm-agent`.

**Autopilot (GitHub, on `main`):**

| Workflow | Schedule / trigger |
|----------|-------------------|
| [agent-fleet-governance.yml](../.github/workflows/agent-fleet-governance.yml) | Daily + fleet path pushes |
| [agent-fleet-execute.yml](../.github/workflows/agent-fleet-execute.yml) | Weekdays |
| [agent-fleet-health.yml](../.github/workflows/agent-fleet-health.yml) | Daily |

Set secret **`DISKLORDZ_VERIFY_BASE_URL`** for production smoke in CI.

---

## SaaS roadmap (WO sequence)

| WO | Theme | Progress |
|----|-------|----------|
| 001–006 | v0 scaffold → rate limits | **Done** (see [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md)) |
| 007 | GenerationSpec in API/UI | **Done** — `GenerationSpecFields`, API parse |
| 008 | Variations | **Done** — 2 studio / 3 creative |
| 009–010 | Async + credits / Stripe | **Code done** — needs `INNGEST_*` / `STRIPE_*` on Vercel |
| 011–012 | History + RAG | **Done** keyword; **012b** pgvector needs embed keys |
| 013–016 | Engines, loop/SFX, factory, daw-inbox | **Done** in repo |
| **Next** | Production + phase-3 agents | [NEXT.md](NEXT.md) |

Detail mapping to ILLUGEN systems: [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) (table § “Twelve systems”).

---

## Plugin & ops roadmap

| Item | Doc |
|------|-----|
| JD Upgraded Phase 5 | [PHASE5.md](PHASE5.md) |
| Golden WAV / plugin CI | `golden-wav-qa` agent · [build.yml](../.github/workflows/build.yml) |
| Repo automation | [REPO_AUTOMATION.md](REPO_AUTOMATION.md) |

---

## Automation vs blocked

| Category | Automate now | Blocked on secrets |
|----------|--------------|-------------------|
| OSS partial → integrated | `./scripts/complete-roadmap-automation.sh` | GPU/Modal live deploy |
| Agent stub roster | `stub-agent-env-check.sh` → [STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md) | n8n OAuth, Bytebot |
| SaaS 007 variations | `saas-007-smoke.sh` (code already ships 2/3 variations) | — |
| Async Inngest | smoke returns `inngest_not_configured` until keys set | `INNGEST_*` on Vercel |
| Credits / Stripe | credit fields in API when configured | `STRIPE_*` prod |
| pgvector embed | `--dry-run` chunk count | `OPENAI_API_KEY` + Supabase service role |

See [docs/disklordz/sops/SOP-011-roadmap-automation.md](disklordz/sops/SOP-011-roadmap-automation.md).

## How to update this doc

1. Edit phase tables above when milestones move.
2. Run `python3 scripts/update-roadmap-progress.py` (also runs from `./scripts/sync-disklordz-agent-fleet.sh`).
3. Open PR with `WO-SAAS-NNN` or agent id in title when applicable.
