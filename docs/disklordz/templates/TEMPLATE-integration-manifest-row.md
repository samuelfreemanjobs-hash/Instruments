# integrations/manifest.json — new row template

```json
{
  "id": 36,
  "repo": "org/repo-name",
  "url": "https://github.com/org/repo-name",
  "status": "partial",
  "wiring": "path/to/doc-or-script-or-skill"
}
```

## Status guide

| status | Meaning |
|--------|---------|
| `integrated` | Automated check or production route uses it |
| `partial` | Documented + stub/script only |
| `external` | Operator-run outside repo |

After edit:

```bash
python3 scripts/update-roadmap-progress.py
bash disklordz/integrations/scripts/verify-integrations.sh
```
