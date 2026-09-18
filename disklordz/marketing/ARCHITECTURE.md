# Disklordz Marketing Department — systems architecture

**Owner:** Marketing Director Agent (`marketing-director`)  
**PM counterpart:** Airtable Disklordz OS / Business Planner + Cursor implementation WOs  
**Slack:** `#disklordz-marketing-dept`  
**Charter:** [docs/DISKLORDZ_MARKETING_DIRECTOR.md](../../docs/DISKLORDZ_MARKETING_DIRECTOR.md)

## Purpose

Design, operate, and automate **direct-response + brand** marketing for DISKLORDZ: phonk sample packs, VST launch funnel, drum SaaS, and future SKUs. Marketing is the **revenue critical path**; engineering ships containers, Marketing ships **conversion**.

## System map (target state)

```text
                    [ Traffic: organic + paid + creator ]
                                    │
                                    ▼
              ┌─────────────────────────────────────────┐
              │  Next.js ( /launch + / + storefront )   │
              │  Web Audio demo · opt-in · checkout     │
              └─────────────────────────────────────────┘
                                    │
         ┌──────────────────────────┼──────────────────────────┐
         ▼                          ▼                          ▼
  [ Attribution layer ]      [ Lifecycle email ]          [ Commerce events ]
  UTM · pixels · Plausible   Resend / Loops / DB seq     Stripe · LS webhooks
         │                          │                          │
         └──────────────────────────┼──────────────────────────┘
                                    ▼
              ┌─────────────────────────────────────────┐
              │  Supabase: leads · orders · entitlements │
              │  + Airtable: campaigns · WOs · assets    │
              └─────────────────────────────────────────┘
                                    │
         ┌──────────────────────────┼──────────────────────────┐
         ▼                          ▼                          ▼
  [ Content factory ]        [ Creative QA ]            [ PM sync ]
  scripts · demos · covers   brand · legal · ears       SKU gate · ship dates
```

## Build vs buy (Marketing systems)

| Layer | Strategy | Stack |
|-------|----------|--------|
| Landing + funnel UX | **Build** (owned) | Next.js `/launch`, env-driven CTAs |
| Email sequences | **Build** logic, **buy** deliverability | Sequence engine in DB or Worker; Resend/Loops API |
| Analytics | **Buy** lightweight | Plausible / PostHog; UTM discipline in Director office |
| Social scheduling | **Buy** | Buffer / native; Director owns scripts |
| CRM | **Build** thin | Supabase `launch_leads` + tags; export to Loops audience |
| Asset DAM | **Build** git + R2 | `disklordz/launch-assets/`, cover PNG, demo WAV |
| Support macros | **Build** | Crisp + links to entitlements (future) |

## PM ↔ Marketing interface

| Event | PM owns | Marketing owns |
|-------|---------|----------------|
| New SKU | `product_id`, price, phase, acceptance | Brief, name, hook, tier in funnel |
| Ship date | WO merge, deploy | Launch thread, email Day 0, short-form batch |
| Blocked env | Secrets, Stripe, Supabase | Copy + URLs ready in draft |
| Kill SKU | Airtable status | Pull ads, redirect landing |

**Cadence:** Weekly **GTM sync** (async Slack thread in `#disklordz-marketing-dept` + Airtable comment on active campaign row).

## Work order prefix

All Marketing execution WOs: **`[Marketing]`** in GitHub / Airtable title.  
Subagent WOs: **`[Marketing][<Role>]`** e.g. `[Marketing][Lifecycle]`.

## Key modules (repo)

| Path | Role |
|------|------|
| `docs/DISKLORDZ_MARKETING_DIRECTOR.md` | Director charter + subagent roster |
| `disklordz/marketing/AGENT_INSTRUCTIONS.md` | Paste into Cursor / Cloud agent |
| `disklordz/marketing/subagents/` | Elite subagent prompts |
| `docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md` | Funnel SKU truth |
| `docs/DISKLORDZ_PHONK_KIT_AI_SOP.md` | Kit/demo asset SOP |
| `docs/DISKLORDZ_MARKETING_HANDOFF.md` | Checklist for humans |

## Extension points

- Zapier MCP **write** actions only after explicit user confirm (security baseline).
- New subagent: add `subagents/<name>.md` + row in Director roster; announce in Slack.
- Campaign row in Airtable: link Slack thread, GitHub issue, Vercel preview URL.

## Related docs

- [DISKLORDZ_SAAS_AGENT_LANES.md](../../docs/DISKLORDZ_SAAS_AGENT_LANES.md)  
- [AGENTIC_PROJECT_STANDARDS.md](../../docs/AGENTIC_PROJECT_STANDARDS.md)
