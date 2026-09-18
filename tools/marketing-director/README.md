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

See [ARCHITECTURE.md](ARCHITECTURE.md).
