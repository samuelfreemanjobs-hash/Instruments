# Disklordz event bus (FORGE v1)

| Event | Producer | Consumer | Status |
|-------|----------|----------|--------|
| `agent.created` | GitHub `forge-agent-registry.yml` | `POST /api/automation/register-agent` | Shipped |
| `lead.opted_in` | `/api/launch/opt-in` | `recordLeadAndSchedule` + optional immediate send | Shipped |
| `email.due` | Hourly cron | `processDueLaunchEmails` → Resend | Shipped |
| `work_order.airtable` | Airtable button | `airtable-antigravity-handoff.yml` | Shipped |
| `purchase.completed` | Stripe webhook | Credits (SaaS); launch tags P2 | Partial |

Future: Airtable → `repository_dispatch` for `[Marketing]` WOs → FORGE registers campaign cron.
