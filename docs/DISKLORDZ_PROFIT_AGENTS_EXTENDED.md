# 15 additional profit agents (phase 2)

Companion to the implemented fleet in [`disklordz/agents/profit/`](../disklordz/agents/profit/). Scaffold a new agent: edit `_specs.json`, run `python3 disklordz/agents/profit/scaffold_agents.py --publish-skills`.

| # | Agent | Upstream / pattern | Profit lever | Automate |
|---|--------|-------------------|--------------|----------|
| 16 | **Churn Win-back** | n8n + Supabase `saved_kits` | Reactivate lapsed Pro users | Query inactive 14d → email + credit grant |
| 17 | **SEO Kit Pages** | Vercel + AI SDK | Organic traffic → free signups | Programmatic pages from factory metadata |
| 18 | **Referral & Affiliate** | Stripe Connect / Rewardful pattern | Lower CAC | Referral codes in manifest.json |
| 19 | **Fraud & Abuse** | Rate limit + IP heuristics | Protect margin on free tier | Extend `rate-limit.ts` + Supabase RPC |
| 20 | **Pricing Experiment** | Stripe MCP read + feature flags | ARPU optimization | Price A/B via `STRIPE_PRO_PRICE_ID` variants |
| 21 | **Onboarding Concierge** | RAG + Inngest drip | TTF first kit → conversion | Event `user.signed_up` → 3-step email |
| 22 | **DAW Inbox Copilot** | `disklordz/daw-inbox` | Stickiness in producer workflow | Watch folder + notify when kit lands |
| 23 | **Airtable WO Triage** | GitHub MCP + antigravity inbox | PM throughput | `airtable-antigravity-handoff.yml` |
| 24 | **Golden WAV QA** | `vst-testing-ops` | Plugin SKU quality (cross-sell) | `run_business.py --profile dsp-only` |
| 25 | **Competitive Intel** | Firecrawl / read-only fetch | Positioning vs ILLUGEN | Scheduled digest to `docs/research/` |
| 26 | **License & Compliance** | Skills + legal checklist | Avoid NC model violations | Engine-swap agent gate on audiocraft license |
| 27 | **Social Clip Factory** | FFmpeg + factory WAVs | Top-of-funnel content | Batch 15s previews from `saved_kits` |
| 28 | **Analytics Interpreter** | Vercel Analytics + Stripe | Decision-grade weekly metrics | Cron → Slack summary workflow |
| 29 | **Release Notes** | GitHub MCP | Trust + upgrade clicks | PR labels → customer-facing changelog |
| 30 | **Investor Metrics** | Stripe + Supabase SQL | Fundraising / partnerships | MRR, gen/batch, credit burn CSV |

**Next five (31–35) for a full 30-agent program:** Partner API agent, Sample pack wholesale B2B, Discord community mod, Colab demo agent ([docs/COLAB_ZERO_INSTALL_TESTING.md](COLAB_ZERO_INSTALL_TESTING.md)), HISE lane handoff agent ([disklordz/antigravity/](../disklordz/antigravity/ARCHITECTURE.md)).

See [DISKLORDZ_PROFIT_AGENTS.md](DISKLORDZ_PROFIT_AGENTS.md) for the original 15 (implemented).
