# Disklordz SOPs & templates

Operational playbooks for **Cursor Cloud**, **IDE Agent**, and **profit-agent fleet** work. Synthesized from Disklordz work orders, integration hub work, agent fleet rollout, and [CURSOR_AGENT_PLAYBOOK.md](../CURSOR_AGENT_PLAYBOOK.md).

**Note:** Cursor does not expose your full chat history to agents. These SOPs capture **durable patterns** from that work so future sessions do not rediscover the same steps.

---

## When to use what

| Situation | Start here |
|-----------|------------|
| **“I don’t know how” / operator task** | [../OPERATOR_AUTOPILOT.md](../OPERATOR_AUTOPILOT.md) · GitHub **Agent run** issue template · [sops/SOP-018-operator-autopilot.md](sops/SOP-018-operator-autopilot.md) |
| New Cloud Agent task | [templates/TEMPLATE-cloud-agent-prompt.md](templates/TEMPLATE-cloud-agent-prompt.md) + [sops/SOP-001-cloud-agent-task.md](sops/SOP-001-cloud-agent-task.md) |
| Add / change profit agent | [sops/SOP-002-add-profit-agent.md](sops/SOP-002-add-profit-agent.md) |
| “Agents get to work” / daily ops | [sops/SOP-003-run-agent-fleet.md](sops/SOP-003-run-agent-fleet.md) |
| Wire OSS integration | [sops/SOP-004-integrations-hub.md](sops/SOP-004-integrations-hub.md) |
| Deploy or smoke prod | [sops/SOP-005-go-live-verify.md](sops/SOP-005-go-live-verify.md) |
| Airtable WO → GitHub | [sops/SOP-006-work-order-to-pr.md](sops/SOP-006-work-order-to-pr.md) |
| RAG / prompt corpus | [sops/SOP-007-rag-corpus.md](sops/SOP-007-rag-corpus.md) |
| Roadmap / progress update | [sops/SOP-008-roadmap-and-progress.md](sops/SOP-008-roadmap-and-progress.md) |
| Make fleet repo-wide | [sops/SOP-009-repo-wide-fleet-governance.md](sops/SOP-009-repo-wide-fleet-governance.md) |
| Open-source batch (35 refs) | [sops/SOP-010-open-source-integration-batch.md](sops/SOP-010-open-source-integration-batch.md) |
| Roadmap backlog automation | [sops/SOP-011-roadmap-automation.md](sops/SOP-011-roadmap-automation.md) · `./scripts/complete-roadmap-automation.sh` |
| Operator autopilot | [sops/SOP-018-operator-autopilot.md](sops/SOP-018-operator-autopilot.md) · [../OPERATOR_AUTOPILOT.md](../OPERATOR_AUTOPILOT.md) · **ops master checklist** [../OPERATOR_OPS_MASTER_CHECKLIST.md](../OPERATOR_OPS_MASTER_CHECKLIST.md) |
| **SparkLE elite lanes** (gangsta trap, Memphis, ATL, hardware, regional, rage) | [spark-le/ARCHITECTURE.md](spark-le/ARCHITECTURE.md) · [SOP-012](sops/SOP-012-spark-le-elite-system-charter.md) · [LANES.md](spark-le/LANES.md) · [TRAP-HAT-LOOP-DNA.md](spark-le/TRAP-HAT-LOOP-DNA.md) · [PRODUCER-LANE-INDEX.md](spark-le/PRODUCER-LANE-INDEX.md) · [SUBAGENTS](spark-le/SUBAGENTS.md) · [MEMORY-AND-CI](spark-le/MEMORY-AND-CI.md) · Skills: `spark-trap-loop-architect`, `spark-le-memory-ci` in `.claude/skills/` → [013](sops/SOP-013-spark-le-phase-1-feed-spark.md)–[017](sops/SOP-017-spark-le-phase-5-elite-leadership.md) |

---

## Templates (copy-paste)

| File | Use |
|------|-----|
| [TEMPLATE-cloud-agent-prompt.md](templates/TEMPLATE-cloud-agent-prompt.md) | Every Cloud Agent job |
| [TEMPLATE-pr-description-disklordz.md](templates/TEMPLATE-pr-description-disklordz.md) | Draft PR body |
| [TEMPLATE-work-order-github.md](templates/TEMPLATE-work-order-github.md) | Issue + PR title linkage |
| [TEMPLATE-pm-add-agent-entry.md](templates/TEMPLATE-pm-add-agent-entry.md) | New agent announcement (then regen) |
| [TEMPLATE-incident-debug.md](templates/TEMPLATE-incident-debug.md) | Broken generate / checkout / CI |
| [TEMPLATE-integration-manifest-row.md](templates/TEMPLATE-integration-manifest-row.md) | New row in integrations manifest |
| [TEMPLATE-spark-le-session-review.md](templates/TEMPLATE-spark-le-session-review.md) | Phase 3 keeper / scoring session |
| [TEMPLATE-spark-le-manifest.json](templates/TEMPLATE-spark-le-manifest.json) | 8-pad SparkLE pack metadata |

---

## Quick commands

```bash
./scripts/run-agent-fleet-now.sh              # execute local fleet roles
./scripts/sync-disklordz-agent-fleet.sh       # regen fleet + roadmap metrics
./scripts/setup-open-source-integrations.sh   # integrations hub
cd disklordz/website && npm run build
bash disklordz/website/scripts/verify-go-live.sh
python3 scripts/update-roadmap-progress.py
```

---

## Related

- [../ROADMAP.md](../ROADMAP.md) — milestones and documentation audit  
- [../../DISKLORDZ_AGENTS.md](../../DISKLORDZ_AGENTS.md) — 31 agents  
- [../DISKLORDZ_ILLUGEN_RESEARCH.md](../DISKLORDZ_ILLUGEN_RESEARCH.md) — SaaS 007+ backlog  
