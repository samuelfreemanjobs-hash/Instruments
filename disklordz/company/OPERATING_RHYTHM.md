# Disklordz operating rhythm

**Creative Director:** you (taste, ship, price).  
**PM of record:** Airtable **Disklordz OS**.  
**Implementer:** Cursor Cloud Agent (+ you for merges and audio judgment).

## Weekly cadence

| When | Ritual | Output |
|------|--------|--------|
| **Monday (30 min)** | **Plan** — pick **3 outcomes max** for the week in Airtable. One must advance **SaaS v0** ([docs/DISKLORDZ_SAAS_V0.md](../../docs/DISKLORDZ_SAAS_V0.md)). | 3 work orders in `Ready` |
| **Monday** | Run **A1** for each WO: `gh workflow run airtable-work-order-to-github.yml -f work_order_id=…` | GitHub issues `cloud-agent` + `work-order` |
| **Tue–Thu** | **Build** — Cloud Agent on issues; PRs must include `WO-YYYY-NNN` in title if tied to a WO | Open PRs, green CI |
| **Wednesday (15 min)** | **Mid-week** — `#disklordz-dev`: blockers only | Unblock or kill a WO |
| **Friday (45 min)** | **Ship / kill** — merge or close; post merged SaaS/plugin work in `#disklordz-shipped` | Airtable `Done` via A2 on merge |
| **Friday (15 min)** | **Learn** — one line in Airtable WO or Products: what did we learn? | Feeds next Monday plan |

## Daily (solo, lightweight)

- Glance `#disklordz-ci` (once webhook is set).  
- **No new WO** unless you finish or kill one (WIP limit: **2** active implementation WOs).  
- **Ears rule:** anything that changes sound waits for you; agents do scaffolding, tests, docs, wiring.

## Agentic OS loop (operating definition)

```text
Airtable WO (Ready)
  → A1 workflow → GitHub issue
  → Cursor Cloud Agent → PR [WO-…] title
  → CI + your review → merge
  → A2 workflow → Airtable Done + github_pr
  → #disklordz-shipped
```

That loop **is** the agentic OS for a one-person company. No 24/7 bot required.

## Status vocabulary (Airtable)

| Status | Meaning |
|--------|---------|
| `Backlog` | Not this week |
| `Ready` | Approved to start; run A1 |
| `In Progress` | Issue linked |
| `In Review` | PR open |
| `Done` | Merged + A2 patched |
| `Killed` | Explicitly dropped; note why |

## What we do not do

- Parallel “star” products without killing or parking a WO  
- Merge without CI green (JD Upgraded golden rules still apply to plugin paths)  
- Second PM tool (Linear is optional read-only; Airtable wins)
