# Disklordz — architecture index

Growth SKU in the [Instruments](../ARCHITECTURE.md) monorepo: drum SaaS, sound factory, RAG, DAW inbox, and Airtable automation.

## Products

| Product | Doc | Run |
|---------|-----|-----|
| **Drum SaaS (Next.js)** | [website/ARCHITECTURE.md](website/ARCHITECTURE.md) | `cd website && npm run dev` |
| **DAW inbox (WO-016)** | [daw-inbox/ARCHITECTURE.md](daw-inbox/ARCHITECTURE.md) | `cd daw-inbox && npm start` |
| **RAG (prompt knowledge)** | [rag/ARCHITECTURE.md](rag/ARCHITECTURE.md) | `python3 rag/scripts/chunk_corpus.py` |
| **Sound factory (offline WAV)** | [sound-factory/README.md](sound-factory/README.md) | `python3 sound-factory/scripts/generate_stub_kits.py` |
| **Antigravity bridge** | [antigravity/ARCHITECTURE.md](antigravity/ARCHITECTURE.md) | repo `scripts/antigravity-bridge/` |
| **Airtable automation** | [automation/README.md](automation/README.md) | `automation/scripts/wo_to_antigravity_handoff.py` |

## QA (local + CI)

```bash
./disklordz/scripts/run_qa.sh
```

Covers: website `lint` + `build`, daw-inbox syntax, RAG chunk + keyword query smoke. Monorepo nightly/full pipeline runs the same SaaS steps via [vst-testing-ops/business_pipeline.py](../vst-testing-ops/business_pipeline.py) (`disklordz_web` stage).

## Deploy & go-live (requires secrets)

| Step | Doc |
|------|-----|
| Vercel + env | [website/DEPLOY.md](website/DEPLOY.md) |
| Go-live checklist | [docs/DISKLORDZ_GO_LIVE.md](../docs/DISKLORDZ_GO_LIVE.md) |
| GitHub secrets | [docs/DISKLORDZ_GO_LIVE_SECRETS.md](../docs/DISKLORDZ_GO_LIVE_SECRETS.md) |
| Manual workflow | `.github/workflows/disklordz-go-live.yml` |

## Roadmap

v0 checklist: [docs/DISKLORDZ_SAAS_V0.md](../docs/DISKLORDZ_SAAS_V0.md) · ILLUGEN phases: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](../docs/DISKLORDZ_ILLUGEN_RESEARCH.md) · **End-to-end business plan:** [docs/DISKLORDZ_COMPLETION_PLAN.md](../docs/DISKLORDZ_COMPLETION_PLAN.md)

## Agent workflow

1. Read this file, then the product `ARCHITECTURE.md` you edit.
2. After SaaS changes: `./disklordz/scripts/run_qa.sh` (or `cd website && npm run lint && npm run build`).
3. Do not deploy production or push Vercel env without explicit user request.
