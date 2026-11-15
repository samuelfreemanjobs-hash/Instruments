# Antigravity product factory

**Goal:** You run the business; **Antigravity prepares products**; **Cursor** ships C++/VST; you only approve, playtest, and sell.

## Division of labor

| Who | Does |
|-----|------|
| **You** | Brand calls, DAW smoke tests, dev box once, pricing, customer support |
| **Antigravity** | Folders, data JSON, docs, assets, packaging scripts, HISE sketches, WAV batch tooling |
| **Cursor** | JUCE/DPF C++, VST builds, SaaS code, CI |
| **Gemini Spark + Coach** | One human task per day |
| **audio-business-copilot** | Sprint routing — **feeds Antigravity first** |

## Automation (hands-off for you)

```bash
# 1) Merge product prep catalog into queue + dispatch up to 2 jobs (cron / daily)
python3 scripts/antigravity-bridge/sync-product-prep.py --merge-only --dispatch --push

# 2) Windows pulls work every 15 min
.\scripts\antigravity-bridge\watch-handoffs.ps1
```

GitHub: **`antigravity-product-prep-sync.yml`** (daily, after queue dispatch).

Config:

| File | Role |
|------|------|
| `disklordz/antigravity/product-routing.json` | What must never go to Antigravity vs default prep |
| `disklordz/antigravity/product-prep-catalog.json` | Concrete prep WOs per sprint task |
| `disklordz/antigravity/active-queue.json` | Runtime queue (merged from catalog) |

## Sprint coupling

When Coach’s **current task** is e.g. `github-repo` or `theory-engine`, `sync-product-prep.py` auto-selects matching **WO-PREP-*** items (see `sprint_task_routing` in product-routing.json).

Examples:

- Before **github-repo**: DPF folder scaffold (no C++)
- Parallel **plugin-skeleton**: README templates + REAPER fixture docs
- Before **theory-engine**: chord spelling JSON for tests
- Before **first-alpha**: packaging script + alpha README

## Outbox → product

Antigravity commits under `docs/drafts/`, `disklordz/antigravity/outbox-artifacts/`, and `scripts/product-prep/`. Cursor Factory **imports** data into C++ — you do not retype specs.

## Policies

- No Stripe/Supabase from Antigravity.
- No JUCE C++ from Antigravity (data + docs only).
- Original music theory data only (no copied progression packs).

## Claude Code / DPF coding

Antigravity does **not** write production C++, but runs Windows builds, pytest, and fixtures for **Claude Code** — [ANTIGRAVITY_CLAUDE_CODE_COPILOT.md](ANTIGRAVITY_CLAUDE_CODE_COPILOT.md).

## Related

- [ANTIGRAVITY_ACTIVE_MODE.md](ANTIGRAVITY_ACTIVE_MODE.md)
- [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md)
