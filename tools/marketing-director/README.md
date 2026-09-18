# Marketing Director — local live mode (dev only)

**Do not** import this module from CI, GitHub Actions, or `business_agent.py`.

Requires `ANTHROPIC_API_KEY` in your environment and `pip install anthropic`.

```bash
export ANTHROPIC_API_KEY=…
python3 tools/marketing-director/marketing_director_live.py "Write three subject lines for …"
```

For production workflow use **team mode**:

```bash
python3 scripts/business-agents/business_agent.py director plan --request "…" --slug my-run
```

### Tests (local API orchestrator)

```bash
cd tools/marketing-director
python3 test_harness.py --mode mocked          # 14 cases — CI, no API key
python3 test_harness.py --mode smoke           # 3 smoke cases (copy_subject_lines, campaign_mothers_day, ops_spend_pacing)
python3 test_harness.py --mode full --report report.html
python3 test_harness.py --case copy_subject_lines
```

Traces: `./traces/<run_id>_<case>.jsonl` (gitignored).

See [ARCHITECTURE.md](ARCHITECTURE.md).
