# DiskLordz Automation

Workflow Automation Engineer owns this tree.

## Layout

```text
automation/
├── README.md
├── .env.example
├── requirements.txt
├── scripts/
│   └── airtable_client.py
├── workflows/
│   ├── n8n/          # exported JSON
│   └── activepieces/
└── templates/
    └── mvp-work-orders.md
```

## Environment

Copy `.env.example` to `.env` (never commit `.env`).

| Variable | Purpose |
|----------|---------|
| `AIRTABLE_API_KEY` | Personal access token |
| `AIRTABLE_BASE_ID` | Base ID from Airtable URL |
| `GITHUB_TOKEN` | Issues/PR automation (fine-grained PAT) |
| `GITHUB_REPO` | e.g. `samuelfreemanjobs-hash/Instruments` |

## Running scripts

```bash
cd disklordz/automation
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python scripts/airtable_client.py list "Agent Work Orders" --max 5
```

## Registration

Every live automation must have a row in Airtable **Automations** (see AUTOMATION_ROADMAP).
