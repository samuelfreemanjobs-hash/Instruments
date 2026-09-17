# DiskLordz Factory

Software scaffold for the **Autonomous Music Factory** — batch-oriented catalog manufacturing with human approval gates.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** first.

```bash
cd apps/factory-api
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
uvicorn disklordz_factory.main:app --reload --port 8787
```

Open `http://127.0.0.1:8787/docs` for the OpenAPI UI.
