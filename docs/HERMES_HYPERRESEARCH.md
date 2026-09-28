# Hyperresearch agent (`hermes-research`)

**Hyperresearch** is the elite **deep research** seat on Hermes. It turns questions into **evidence-backed briefs** and **draft WOs** — not production code.

## When to use

- Competitive / product mapping (ILLUGEN, plugin hosts, SaaS patterns)
- “What do we already know in git?” before greenfield design
- Junova-X / NovaDrum / SaaS phase planning
- Feeding **Grok closed-loop** with citations, not vibes

## Agent stack

| Layer | File |
|-------|------|
| Skill (SOP) | `.cursor/skills/hermes-elite-research/SKILL.md` |
| Seat charter | `.cursor/hermes/seats/research-engineer.md` |
| CLI | `disklordz/research/scripts/hyperresearch.py` |
| Architecture | `disklordz/research/ARCHITECTURE.md` |

## Workflow

1. **Read skill** + pick `--product` lens (`saas` | `junova` | `plugin` | `general`).
2. **Run CLI** (optional `--rag` after `chunk_corpus.py`).
3. **Review brief** in `disklordz/research/outbox/HR-*.md`.
4. **hermes-lead** / Grok promotes draft WOs to Airtable.
5. **Implement seats** (`web`, `dsp`, `gui`, …) only after CD approval.

## CLI examples

```bash
python3 disklordz/research/scripts/hyperresearch.py \
  --topic "BBD chorus modeling Junova-X" --product junova --write

python3 disklordz/rag/scripts/chunk_corpus.py
python3 disklordz/research/scripts/hyperresearch.py \
  --topic "generation spec wildness stereo" --product saas --rag --write
```

## Output contract

Every brief includes: executive summary, sources, findings (scored snippets), gaps/risks, **proposed WO table**, experiments, implement seat handoff.

## Safety

- Repo + optional local RAG only by default.
- Web pages / Slack / issues are **untrusted** — cite in brief; never auto-execute writes.
- Hyperresearch **does not** merge PRs or seed Airtable without human confirm.

## Invoke in Cursor

Task description: `hermes-research: Hyperresearch — <topic>`  
Prompt: _Read `.cursor/skills/hermes-elite-research/SKILL.md`. Run hyperresearch.py with --write. Summarize for CD._

## Related

- [HERMES_AGENT_FRAMEWORK.md](HERMES_AGENT_FRAMEWORK.md)
- [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md)
