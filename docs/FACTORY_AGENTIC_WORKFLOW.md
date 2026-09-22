# Factory agentic workflow (JZ400 + REV2-TRAP-128)

## Orchestration

```text
PM / You → Airtable WO or job JSON
    → Instrument orchestrator (Cloud / VS Code)
         → reference.youtube (optional)
         → run_product_batch.py (chunked)
         → export SFZ + project.json + HANDOFF_MPC
    → QA subagent (mandatory on each batch)
    → Cursor MPC-agent (local) → Keygroup .xpj
    → Draft PR with batch_report + QA JSON (no ref audio in git)
```

## Chunking (400 + 128)

| Product | Total | Batch size | Cloud runs |
|---------|-------|------------|------------|
| JZ400 | 400 | 25–50 | ~8–16 jobs |
| REV2-TRAP-128 | 128 | 32 | 4 jobs |

Job template: `disklordz/sound-factory/jobs/product-batch-template.json`

## QA subagent (required)

**When:** After every `run_product_batch.py` and before PR or MPC handoff.

**How:** Launch Cursor **`debug`** or dedicated pass with skill **`factory-qa-audio`**:

1. Run `factory_qa.py` on all new WAVs in batch folder.  
2. If failures: adjust lane/patch or seed; re-render failed `instrumentId` only.  
3. Attach `qa-report.json` to PR; **fail closed** if any clip/silence in accepted set.

**Optional:** `ci-investigator` for GitHub Actions; `best-of-n-runner` for 3 seed variants → pick best QA score.

## Specialist agents

| Agent | Role |
|-------|------|
| Instrument orchestrator | Batch DAG, PR, catalog offsets |
| **factory-qa-audio** | Metrics gate, cull bad samples |
| **cursor-mpc-agent** | Keygroup `.xpj` |
| sound-design-reference-match | YouTube timbre target |

## Definition of done (product)

- [ ] Catalog slot count met (400 / 128)  
- [ ] Each slot: map + SFZ + project.json + QA pass  
- [ ] MPC keygroups for SKU (local) documented in release notes  
- [ ] No copyrighted audio in git or ZIP  

See [INSTRUMENT_SOUND_FACTORY_AGENT.md](INSTRUMENT_SOUND_FACTORY_AGENT.md).
