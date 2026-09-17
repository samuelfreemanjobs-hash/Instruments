# GTM decisions log (founder → VARA)

Decisions here override informal chat until superseded. PM Agent (Business Planner + Airtable) co-signs ship dates.

| Date | Decision | Detail |
|------|----------|--------|
| 2026-09-17 | **Cart / launch sequencing** | **Day 2:** tripwire **sample** offer goes live (Day 1 = free vocal vault + results-in-advance; no paid cart pressure on open day) |
| 2026-09-17 | **Payments** | **Stripe only** — keep it simple; no Lemon Squeezy MoR in v1 |
| 2026-09-17 | **PM Agent** | **Confirmed:** Business Planner + Airtable Disklordz OS = PM Agent; VARA syncs on WOs and SKU truth |
| 2026-09-17 | **Director voice prompt** | Founder will paste Claude Marketing Director prompt when at home → merge into `AGENT_INSTRUCTIONS.md` § Voice layer |

---

## Launch calendar (v1 per founder)

| Day | Focus | Primary CTA |
|-----|--------|-------------|
| **Day 1** | Free stems + soundboard; trust, demo, list growth | `/launch/vocals` opt-in |
| **Day 2** | **Tripwire sample** live (paid, low ticket) | Stripe Checkout → sample SKU |
| **Day 3–4** | Kern urgency + flagship seed | Email + TY page OTO paths |

Tripwire sample SKU (name TBD with PM): e.g. lite one-shot pack or IR teaser — must match `[Marketing][Commerce]` copy before WO.

---

## Stripe-simple checklist (engineering + PM)

- [ ] One Stripe **Product** per tripwire sample (Day 2)
- [ ] `STRIPE_*` on Vercel (existing SaaS webhook pattern)
- [ ] Post-purchase: redirect to thank-you / download URL (entitlements WO later)
- [ ] No multi-gateway, no MoR in v1

---

## Pending

- [ ] Paste founder Claude prompt → `disklordz/marketing/VOICE_LAYER.md` + fold into `AGENT_INSTRUCTIONS.md`
