# Hermes SOP & Procedures (elite)

**Seat ID:** `hermes-sop`  
**Skill:** `.cursor/skills/hermes-elite-sop/SKILL.md`

Authors and audits **Disklordz operating procedures** under `disklordz/ops/sops/`. Works with every seat to capture how work is done; keeps `INDEX.json` and `sop audit` green.

```bash
python3 disklordz/hermes/scripts/hermes_tool.py sop audit
python3 disklordz/hermes/scripts/hermes_tool.py sop draft --id SOP-EXAMPLE-001 --title "Example" --owner hermes-ops --write
```

No autonomous Airtable writes, production deploys, or pricing approval.
