# DISKLORDZ — integration audit (loose ends)

**Last audit:** 2026-09-18 · **Owner:** PM Agent schedules fixes; **Cursor Cloud** implements `[Eng]` / `[Factory]` WOs.

Legend: ✅ wired · 🟡 partial · 🔴 not wired · 📋 docs-only

---

## Executive summary

| Question | Answer |
|----------|--------|
| **Is the factory automated?** | **🟡 Partial.** Generation is **real parametric code** in Next.js (`/api/generate`) + CLI `generate_kit.py`. **Not** unattended batch-to-SKU: no job queue, no auto-ZIP to `launch-assets`, no A&R QC hook. |
| **Is everything one system?** | **No.** SaaS, launch funnel, marketing agents, A&R, RAG, Antigravity, and JUCE plugin are **separate lanes** — this doc lists gaps. |
| **Who wires things?** | **FORGE** (automation), **PM** (secrets + WOs), **Cursor Cloud** (code). Leadership agents (VARA, HELM, CROW) **do not deploy**. |

---

## Product factory (sound)

| Component | Status | Wired to | Gap |
|-----------|--------|----------|-----|
| In-process factory | ✅ | `/api/generate`, KitGenerator | Sync only; no async jobs (ILLUGEN 009) |
| `sound-factory/generate_kit.py` | 🟡 | Copies stubs from `public/samples` if present | Not called from CI; batch script added → `scripts/batch-tripwire-sample.sh` |
| `generate_stub_kits.py` | ✅ | Static preview WAVs | Must run manually before stubs exist |
| Tripwire ZIP output | 🔴 | — | `disklordz/launch-assets/` empty — no packaged SKU |
| A&R QC | 📋 | `ar/QC_TRIPWIRE_SAMPLE.md` | No API gate before download |
| Colab smoke | 📋 | `docs/COLAB_ZERO_INSTALL_TESTING.md` | Not in CI |

---

## Web app (disklordz/website)

| Component | Status | Wired to | Gap |
|-----------|--------|----------|-----|
| SaaS `/` | ✅ | Stripe Pro, Supabase kits | Needs prod env |
| Launch `/launch` | ✅ | opt-in, soundboard | — |
| Opt-in → email queue | 🟡 | Supabase + Resend + cron | **Migration + env** must be applied on prod |
| Vercel cron | 🟡 | `vercel.json` hourly | Requires Vercel plan + `CRON_SECRET` |
| Stripe webhook | 🟡 | **Pro subscription only** | Launch tripwire one-time checkout **not** handled |
| Tripwire Payment Link | 🔴 | env `LAUNCH_TRIPWIRE_SAMPLE_URL` | PM must create Stripe product |
| Launch vocal ZIP | 🔴 | `LAUNCH_VOCAL_STEMS_URL` | No asset in repo |
| Entitlements / download API | 🔴 | Planned R2 | Gumroad-style delivery not built |

---

## Agents & departments (docs vs runtime)

| Agent | Charter in git | Auto-trigger | Gap |
|-------|----------------|--------------|-----|
| VARA + mkt subagents | ✅ | FORGE registry on push | Copy agents founder creates tonight → merge to repo |
| HELM | ✅ | Manual / Slack | Not a Cursor subagent type — **prompt + docs** |
| FORGE | ✅ | CI + cron code | GitHub secrets `LAUNCH_AUTOMATION_REGISTER_URL` often unset |
| CROW + artists | ✅ | FORGE on `disklordz/ar/**` | No QC API after factory batch |
| PM Agent | 📋 Airtable | Airtable→GH for HISE only | Launch WOs not in Airtable seed |
| OpenClaw artists | 📋 lanes doc | Local machine | Not in Cloud VM |

---

## Automation & CI

| Workflow | Status | Secrets needed |
|----------|--------|----------------|
| `disklordz-website.yml` | ✅ build on PR | none |
| `forge-agent-registry.yml` | 🟡 | `LAUNCH_AUTOMATION_REGISTER_URL`, `AUTOMATION_SECRET` |
| `airtable-antigravity-handoff.yml` | 🟡 | Airtable + PAT |
| `antigravity-inbox-slack.yml` | 🟡 | Slack webhook |
| `ci-slack-notify.yml` | 🟡 | Slack webhook |
| Plugin `build.yml` | ✅ | — |

Run: `./scripts/setup-disklordz-integrations.sh all` (partial manual Slack).

---

## RAG

| Component | Status | Gap |
|-----------|--------|-----|
| Keyword RAG in website | ✅ `/api/rag/suggest` | — |
| `chunk_corpus.py` | ✅ | Corpus manifest **missing** new marketing/AR/launch docs → fixed in manifest |
| pgvector / Supabase | 🔴 | Future 012b |

---

## Plugin / hardware lanes

| Lane | Status | Gap |
|------|--------|-----|
| JD Upgraded | ✅ CI | — |
| Memphis Tape Track E | 📋 | No JUCE target |
| HISE / Antigravity | 🟡 | Handoff JSON; Windows local |
| Junova / NovaDrum | 📋 | Not on main |

---

## Priority wiring queue (PM → Cursor)

| P | WO | Closes |
|---|-----|--------|
| P0 | Apply Supabase migration + Vercel env ([PM_SCHEDULING.md](../disklordz/automation/PM_SCHEDULING.md)) | Email automation live |
| P0 | `[Factory]` batch + pack tripwire → `launch-assets/tripwire-sample-v1/` | Day 2 product exists |
| P1 | Stripe one-time checkout + webhook metadata for launch SKU | Tripwire revenue |
| P1 | GitHub secrets for FORGE registry URL | Agent auto-register |
| P2 | Stripe webhook → purchaser tags + email branch | OTO automation |
| P2 | Airtable seed rows for launch WOs | PM single pane |
| P3 | Async factory jobs | Scale |

---

## Single diagram (target)

```text
Airtable WOs → GitHub / Cursor Cloud ──► website + factory code
                    │
OpenClaw sessions ──┼──► ar/sessions → RAG corpus → /api/rag/suggest
                    │
Factory generate ───┼──► launch-assets ZIP → Stripe → webhook → entitlements
                    │
FORGE cron ─────────┴──► Resend lifecycle
```

**Related:** [disklordz/ARCHITECTURE.md](../disklordz/ARCHITECTURE.md) · [DISKLORDZ_WORKFORCE.md](DISKLORDZ_WORKFORCE.md)
