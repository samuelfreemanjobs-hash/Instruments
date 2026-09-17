# Disklordz automation (Sprint A1 / A2)

**PM system of record:** Airtable base **Disklordz OS** (`Agent Work Orders` table).

## Work order → GitHub issue (A1)

```bash
cd disklordz/automation
cp .env.example .env   # local only
python3 scripts/wo_to_github_issue.py --work-order-id WO-2026-001
```

GitHub Actions (secrets: `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID`):

```bash
gh workflow run airtable-work-order-to-github.yml -f work_order_id=WO-2026-001
```

**Airtable button (optional):** Automation → Run script → `POST` GitHub `repository_dispatch`  
event `airtable-work-order` with body `{"work_order_id":"WO-2026-001"}`  
(use a PAT with `repo` scope as `GITHUB_PAT` secret in Airtable script, or Zapier webhook).

## PR merged → Airtable Done (A2)

Workflow [`.github/workflows/airtable-pr-sync.yml`](../../.github/workflows/airtable-pr-sync.yml) patches the work order when a merged PR title contains `WO-YYYY-NNN`.

PR title convention: `[WO-2026-003] SaaS v0 — prompt API scaffold`

## Schema setup

Full base provisioning lives on branch `agent/wo-a1-github-api` / PR #9. Minimum fields on **Agent Work Orders**:

`work_order_id`, `title`, `status`, `acceptance_criteria`, `assigned_team`, `assigned_role`, `product`, `github_issue`, `github_pr`
