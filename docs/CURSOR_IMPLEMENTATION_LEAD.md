# Cursor Cloud Agent — role in DISKLORDZ

## What this agent is

| Title (accurate) | **Staff Implementation Engineer** / **Chief Coding Agent** |
|------------------|-------------------------------------------------------------|
| **Not** | CEO, CMO, PM, release manager, or unattended cron |
| **Is** | Senior-level **software engineer** that edits the repo, runs builds, ships PRs, and executes **WOs** from PM |

Think: **the build crew + integrator**, reporting to **PM** and taking **creative direction** from VARA/CROW/HELM **as specs**, not as parallel executives in one chat.

## What you already have for “leadership”

You do **not** need to hire a human VP on day one if these are **used as decision owners**:

| Role | Agent / system | Decides |
|------|----------------|---------|
| **Product & schedule** | PM (Airtable + Business Planner) | SKU, date, secrets, WO priority |
| **Marketing & copy** | VARA + email/DR subagents | Campaigns, funnel copy |
| **Launch execution** | HELM | T-7 calendar, asset freeze |
| **Automation** | FORGE | Queues, cron, registry |
| **Sound & lanes** | CROW + artist agents | QC, preset DNA |
| **Strategy veto** | **You** | Price, legal, taste, brand |

**Gap:** Leadership is **documented** but **not all wired to Airtable/Slack triggers** — see [DISKLORDZ_INTEGRATION_AUDIT.md](DISKLORDZ_INTEGRATION_AUDIT.md).

## What Cursor should do (default)

1. Read `/ARCHITECTURE.md` + product doc + integration audit when touching Disklordz  
2. Implement `[Eng]`, `[Factory]`, `[Marketing]` **code** WOs  
3. Wire systems (webhooks, scripts, CI) when PM prioritizes  
4. **Refuse** to pretend ops are live without env/migration proof  

## What Cursor should **not** do alone

- Deploy production without you/PM  
- Send marketing email to real lists without Resend + policy  
- Approve sample packs (A&R + you)  
- Replace PM scheduling judgment  

## If you want “more leadership”

| Option | Effort |
|--------|--------|
| **A. Use the agent roster** (VARA, HELM, FORGE, CROW, PM) with **mandatory Slack + Airtable WOs** | Low $ — discipline |
| **B. Nightly Cloud Agent cron** on PM queue (future FORGE WO) | Medium eng |
| **C. Human part-time PM/CMO** | $ — when revenue supports |

**Recommendation:** **A now**, **B after P0 wiring** (migration, tripwire ZIP, Stripe), **C when MRR justifies it**.

## Invoke implementation work

```
PM opens WO → GitHub issue → Cursor Cloud branch → PR → PM Done
```

Prompt: *“Execute WO-LAUNCH-001 per INTEGRATION_AUDIT P0.”*
