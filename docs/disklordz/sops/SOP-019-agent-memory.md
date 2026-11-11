# SOP-019 — Agent memory (continuous improvement)

## Purpose

Every agent starts with what previous agents learned and leaves behind what it learned, so the same failure never costs time twice. Lessons that recur become SOP steps, rules, tests or scripts.

## Where memory lives

`disklordz/agents/memory/` in git (every Cursor, Cloud and CLI agent gets it with the clone):

| File | Holds |
|------|-------|
| `MEMORY.md` | Index (≤ 200 lines): session protocol, "Always load" hot list, generated manifest |
| `shared_learnings.md` / `shared_incidents.md` | Lessons for any agent |
| `<agent-id>_learnings.md` | Validated, reusable lessons for one agent role (`L-0001`…) |
| `<agent-id>_incidents.md` | Dated problems and their fixes (`I-0001`…) |

Agent ids match `disklordz/agents/profit/<id>/` (e.g. `vst-factory-director` for factory, plugin, design-system and JUCE work).

## Procedure

1. **Recall at start:** `python3 scripts/agent_memory.py recall --agent <id> <3-6 task keywords>`. Read the "Always load" list and the matches before planning.
2. **Capture during work:** when a step fails for a non-obvious reason, or a review finds something tests missed, write down: when it applies, the lesson, the exact fix, and the evidence.
3. **Record at end** (before saying "done"):
   ```bash
   python3 scripts/agent_memory.py add --agent <id> --kind learning \
     --title "<rule-like title>" --area "<tags>" --when "<trigger>" \
     --lesson "<what is true>" --do "<exact fix: commands, flags, paths>" \
     --evidence "<PR / run id / output>" --confidence executed
   python3 scripts/agent_memory.py check     # must exit 0
   ```
   Use `--kind incident` for a dated problem + fix. Commit memory changes with the work.
4. **Promote on repeat:** when you hit an existing lesson again, turn it into an SOP step, rule, test or script in the same PR, then `python3 scripts/agent_memory.py promote <ID> --agent <id> --to <path>`. Promoted lessons stay as history; the target is now the source of truth.
5. **Curate the hot list:** keep at most ~10 bullets under "Always load" in `MEMORY.md`, each pointing to an entry (`<file-stem>:<ID>`). Retire stale entries by setting `status: retired`.

## Quality bar for an entry

- **Specific:** names the trigger (`when`) and the exact fix (`do`), not advice like "be careful".
- **Evidenced:** PR, run id, file or output; `confidence` is `executed` only if you ran it.
- **Not derivable:** if git log, an SOP or ARCHITECTURE.md already says it, link there instead.
- **No secrets or personal data:** `add` refuses and `check` fails on common secret formats.

## Definition of done

```bash
python3 scripts/agent_memory.py check
python3 -m unittest discover -s scripts -p "test_agent_memory.py"
```

## Related

[MEMORY.md](../../../disklordz/agents/memory/MEMORY.md) · [SOP-VF-11 Agent delegation](SOP-VF-11-agent-delegation.md) · [TEMPLATE-factory-cloud-agent-prompt.md](../templates/TEMPLATE-factory-cloud-agent-prompt.md)
