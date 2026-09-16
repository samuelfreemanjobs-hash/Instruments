# Automation Roadmap — Workflow Automation Engineer

Ordered backlog to reach **Phase 4 autopilot** (see [MASTER_PLAN.md](MASTER_PLAN.md)). Each item should become a row in Airtable **Automations** when implemented.

## Tool selection matrix

| Use case | First choice | Fallback |
|----------|--------------|----------|
| Quick Airtable ↔ SaaS glue, MCP from Cursor | **Zapier** | Activepieces cloud |
| Complex branching, self-host, audio file paths | **n8n** (self-host) | Scripts + cron |
| Repo CI, plugin builds | **GitHub Actions** | Already in `MyFirstPlugin` |
| Long implementation / refactors | **Cursor Cloud Agent** | — |
| Secrets | Host env / Zapier connections | 1Password (human) |

Engineer should **prefer boring**: one platform per integration class; document in Automations table.

---

## Sprint A — Wiring (highest ROI)

| ID | Automation | Trigger | Action | Platform |
|----|------------|---------|--------|----------|
| A1 | Work Order → GitHub Issue | WO status = `Queued`, agent needs code | Create issue in correct repo with AC + links | Zapier or script |
| A2 | PR merged → WO done | PR closed merged | Update WO + Project % | GitHub Action webhook → Airtable |
| A3 | Product `Briefed` → Project shell | Product status change | Create Project + default WOs from template | Zapier |
| A4 | Stale WO alert | Daily schedule | Slack/email digest of WOs > 48h in `In Progress` | Zapier |
| A5 | Automation health | Weekly | Set `last_verified` or mark `Broken` | n8n or script |

**Deliverable:** `disklordz/automation/workflows/` exports (n8n JSON / Zapier doc links) + README runbooks.

---

## Sprint B — Sample pipeline

| ID | Automation | Notes |
|----|------------|-------|
| B1 | Watch `RAW/` ingest folder | New file → queue processing WO |
| B2 | Processing script success | Write `Sound Assets` row + move to `FINAL/` |
| B3 | Duplicate detection | Hash compare; flag in Airtable |
| B4 | Kit assembly | When N assets tagged `kit_candidate` → Kit Architect WO |

Script home: `disklordz/automation/scripts/sample_pipeline/` (to be created in Phase 1 execution).

---

## Sprint C — Plugin pipeline

| ID | Automation | Notes |
|----|------------|-------|
| C1 | CI failure | Open WO for JUCE engineer; link run URL |
| C2 | CI success on `main` | Bump Release `Candidate`; attach artifact URL |
| C3 | pluginval report | Parse artifact → QA WO checklist |

Leverage existing `.github/workflows/build-plugin.yml`.

---

## Sprint D — Release & store

| ID | Automation | Notes |
|----|------------|-------|
| D1 | Release Agent zip template | Script: folder layout from MASTER_PLAN |
| D2 | `Ready to Publish` | Generate approval packet (markdown + links) |
| D3 | Post-publish | Archive WOs; set Product `Released` |

---

## Operating rules (Automation Engineer agent)

1. **Work until blocked** — pick highest-priority open item from this roadmap or Airtable Automations with status `Planned`.
2. **Idempotency** — safe to re-run; use external IDs (issue number, WO id) as keys.
3. **Observability** — every run logs to Airtable or structured log file referenced in Automations row.
4. **No secrets in git** — `.env.example` only.
5. **Confirm writes** — Zapier/external writes that affect production data: follow Zapier MCP confirmation rules for user-visible actions.
6. **Handoff to Audio PM** — new WOs for code the engineer cannot merge (plugin features).

## Exit criteria (business “runs without me”)

All Sprint A items **Live**; at least B2 + C2 + D2 **Live**; Creative Director only receives approval packets and weekly digest.
