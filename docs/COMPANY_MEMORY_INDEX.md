# Company memory index (Disklordz / Instruments)

Single map so agents do **not** lose context between products, rules, and fleet roles. Refresh when adding a product, agent, or always-on rule.

## Always-on Cursor rules (`.cursor/rules/`)

| Rule | Scope |
|------|--------|
| [`architecture-documentation.mdc`](../.cursor/rules/architecture-documentation.mdc) | Every product needs `ARCHITECTURE.md` |
| [`security-baseline.mdc`](../.cursor/rules/security-baseline.mdc) | Secrets, RLS, rate limits |
| [`disklordz-agent-fleet.mdc`](../.cursor/rules/disklordz-agent-fleet.mdc) | Fleet edits via `_specs.json` + sync script |
| [`disklordz-saas-context.mdc`](../.cursor/rules/disklordz-saas-context.mdc) | Web SaaS lane |
| [`antigravity-hise-lane.mdc`](../.cursor/rules/antigravity-hise-lane.mdc) | HISE / Antigravity |
| [`product-prd-planner.mdc`](../.cursor/rules/product-prd-planner.mdc) | PRD before greenfield code |
| [`company-memory-index.mdc`](../.cursor/rules/company-memory-index.mdc) | Points here |

If a rule seems “missing,” check this table and [`AGENTS.md`](../AGENTS.md) — do not rely on chat memory alone.

## Product index

| Product | Architecture | PRD / notes |
|---------|--------------|-------------|
| JD Upgraded | [docs/ARCHITECTURE.md](ARCHITECTURE.md) | Core VST3/CLAP synth |
| WAVE-9090 | [Wave9090/ARCHITECTURE.md](../Wave9090/ARCHITECTURE.md) | Trap wavetable |
| TRAP-FORGE | [disklordz/trap-forge/ARCHITECTURE.md](../disklordz/trap-forge/ARCHITECTURE.md) | PWA + VST shell |
| Serum Forge | [tools/serum-forge/ARCHITECTURE.md](../tools/serum-forge/ARCHITECTURE.md) | Symbolic Serum pipeline |
| SynthForge | [tools/synth-forge/ARCHITECTURE.md](../tools/synth-forge/ARCHITECTURE.md) | Hardware presets |
| Drum blueprint | [tools/drum-synth-blueprint/ARCHITECTURE.md](../tools/drum-synth-blueprint/ARCHITECTURE.md) | NumPy kick/808 |
| Memphis Architect | [disklordz/memphis-architect/ARCHITECTURE.md](../disklordz/memphis-architect/ARCHITECTURE.md) | Phonk kick/snare UI |
| **V-Voyager VST** | [products/VOYAGER_VST.md](products/VOYAGER_VST.md) | Explorer synth program (restored index) |
| Disklordz SaaS | [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md) | WO-SAAS-* |

## Agent fleet (35)

- Index: [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md)
- Registry: [`disklordz/agents/profit/_specs.json`](../disklordz/agents/profit/_specs.json)
- **New audio lane:** `audio-plugin-coder`, `code-project-planner`, `ddsp-ml-engineer`

## Automation & handoff

- Claude → Cursor: [CURSOR_CLAUDE_AUTOMATION.md](CURSOR_CLAUDE_AUTOMATION.md)
- Agent playbook: [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md)
- RAG corpus: `python3 disklordz/rag/scripts/chunk_corpus.py`

## Snare / phonk research (shared)

- [SNARE_RESEARCH_PLAN.md](SNARE_RESEARCH_PLAN.md) — 808 clap **+2–4 st** on snare
