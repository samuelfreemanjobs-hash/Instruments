# Human triggers — automation-first (FORGE v2)

**Default:** PM configures **once**; **FORGE** runs schedules. Founder is **not** the cron job.

## Automated (shipped or shipping on PR #33)

| Action | How |
|--------|-----|
| Opt-in → schedule 4-day emails | Supabase queue on `/api/launch/opt-in` |
| Send due emails | Vercel Cron hourly → `/api/cron/launch-queue` |
| Day 2 tripwire links in email | `LAUNCH_CAMPAIGN_START` + `getLaunchCampaignDay()` ≥ 2, or `LAUNCH_TRIPWIRE_ENABLED=true` |
| New agent → register workflow | GitHub `forge-agent-registry.yml` → `/api/automation/register-agent` |

## Founder / PM — one-time only

| Action | Who |
|--------|-----|
| Approve tripwire WAV pack (ears + legal) | **Founder** |
| Set env + migration + `LAUNCH_CAMPAIGN_START` | **PM** |
| Stripe Product / Payment Link for tripwire | **PM** |
| Paste voice prompt | **Founder** (optional) |

## PM Agent — ongoing

- Own [PM_SCHEDULING.md](../automation/PM_SCHEDULING.md) checklist each campaign
- Open `[Eng][Automation]` WOs when FORGE reports gaps
- Never assign founder “click send on Day 2” — use queue

## Fallback if Supabase/Resend missing

Opt-in still redirects to thank-you; emails log error until PM fixes secrets.
