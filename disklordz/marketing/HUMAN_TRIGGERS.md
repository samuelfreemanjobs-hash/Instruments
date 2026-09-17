# Human triggers — what is NOT fully automated

**HELM rule:** If it spends money, sends email to a list, or opens a cart, assume **founder or PM explicit trigger** until v2 automation is built and tested.

## You must trigger (v1)

| # | Action | When | Who |
|---|--------|------|-----|
| 1 | **Approve tripwire sample WAV pack** | Before Stripe product goes live | **Founder** (ears + legal) |
| 2 | **Create Stripe Product/Price** (or confirm in Dashboard) | After pack approved | **Founder or PM** (one-time) |
| 3 | **Set Vercel env** (`LAUNCH_*`, `STRIPE_*`, `RESEND_*`, download URL) | Pre–Day 1 | **PM / Founder** |
| 4 | **Launch Day 1 — open free opt-in** | Campaign start | **Founder:** merge PR, confirm deploy, post “live” in Slack |
| 5 | **Day 2 — enable tripwire** | Calendar Day 2 | **Founder:** flip env or feature flag / publish Stripe link in email |
| 6 | **Send or schedule email batch** | Day 1–4 | **Founder or PM** until Resend automation WO done — *copy is pre-written* |
| 7 | **Paste Claude voice prompt** | When at home | **Founder** → `VOICE_LAYER.md` |
| 8 | **Create elite copy agents tonight** | Optional | **Founder** — see TODO |

## Runs without you (after setup)

| Action | Condition |
|--------|-----------|
| Opt-in → thank-you redirect | Deployed site |
| Soundboard / generate kit on SaaS | Deployed + limits |
| Stripe Pro webhook (existing SaaS) | Env configured |
| Cursor agents on **explicit WO** | You or PM opens GitHub issue / Cloud task |

## Target automation (later)

- Opt-in → auto Day 0 email (Resend)
- Purchase → tag + OTO branch
- Scheduled Day 2 cart email (cron/Loops)

**PM ADD:** Track automation gaps in Airtable WO `[Eng][Marketing] Lifecycle v2`.
