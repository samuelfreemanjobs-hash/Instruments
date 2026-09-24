# Automated coding & product work completion

**Owner:** workflow-automation-agent  
**Queue:** `disklordz/vst-factory/inbox/HO-*.json` (all agents via `to_agent`)  
**Done:** `disklordz/vst-factory/outbox/HO-<id>-done.json` + PR when code changes

## Pipeline

```text
UTC cron (GitHub) → verify/build artifact (optional)
                 → agent-scheduled-handoffs.yml emits inbox JSON
                 → Slack #disklordz-dev (on push to `main` inbox paths only)
                 → Cursor Cloud Agent consumes handoff
                 → PR + tests + outbox done
```

| Layer | Automation | Completes when |
|-------|------------|----------------|
| **Verify** | `vst-plugin-factory-schedule.yml`, `nightly-qa.yml` | Artifact uploaded; Slack schedule notify |
| **Assign** | `agent-scheduled-handoffs.yml` + [scheduled_work.yaml](../disklordz/automation/scheduled_work.yaml) | New `HO-*.json` with `status: open` |
| **Implement** | Cursor Cloud (`to_agent` in handoff) | Draft PR; CI green per agent contract |
| **Close** | Agent or pm-agent | `outbox` JSON + handoff `status: closed` in PR or follow-up commit |

## Scheduled jobs (UTC)

| Cron | Handoff emitter | Agent | Product outcome |
|------|-----------------|-------|-----------------|
| Mon 09:00 | (health job only) | — | CI artifact |
| Mon 09:40 | `mon-health-triage` | cursor-vst-plugin-factory | Triage / fix WO |
| Mon 09:45 | `mon-night-circuit-qa-gate1` | night-circuit-qa | Gate 1 report |
| Tue 15:00 | `tue-product-drain` | cursor-vst-plugin-factory | Up to 2 open inbox WOs → PRs |
| Wed 10:00 | (integrity job) | — | Night Circuit artifact |
| Wed 10:45 | `wed-night-circuit-integrity-followup` | night-circuit-qa | Fix or escalate |
| Thu 16:00 | `thu-sound-design-batch` | night-circuit-sound-design | Featured/GUI/listening |
| Fri 17:00 | (doc-sync job) | — | Checklist artifact |
| Fri 17:20 | `fri-doc-sync` | cursor-vst-plugin-factory | Docs PR if needed |

Manual emit:

```bash
pip install pyyaml
python3 scripts/agent-schedule/emit_handoffs.py --list
python3 scripts/agent-schedule/emit_handoffs.py --cron "45 9 * * 1" --dry-run
gh workflow run agent-scheduled-handoffs.yml -f cron="0 15 * * 2"
```

## Definition of done (all agents)

1. **Acceptance criteria** in handoff JSON satisfied or explicitly deferred in outbox with reason.
2. **Evidence:** CI profile appropriate to change (`run_business.py --profile ci` for plugin C++).
3. **PR** title includes `work_order.id` when code/docs change.
4. **Outbox** file:

```json
{
  "handoff_id": "HO-20260924-abcd",
  "status": "done",
  "completed_at": "2026-09-24T12:00:00Z",
  "agent": "night-circuit-qa",
  "result": "pass",
  "pr_url": "https://github.com/.../pull/60",
  "notes": "Gate 1 PASS; FL/MPC NOT TESTED"
}
```

5. **Slack:** optional reply in #agent-team thread with PR link.

## Cursor Automations (parallel)

GitHub handoffs **assign** work; Cursor dashboard **Automations** (same UTC as [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md)) should run agents that **poll inbox** or **execute the matching WO** without waiting for human. Configure both for redundancy.

See [.cursor/automations/README.md](../.cursor/automations/README.md).

## Night Circuit M1 priority

Until [MILESTONE_1.md](../ProphetRev2Trap/docs/MILESTONE_1.md) gates pass, `tue-product-drain` and factory WOs favor **verification** over new features.

## Related

- [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md)
- [SLACK_AGENT_TEAM.md](SLACK_AGENT_TEAM.md)
- [disklordz/vst-factory/AGENT_INSTRUCTIONS.md](../disklordz/vst-factory/AGENT_INSTRUCTIONS.md)
