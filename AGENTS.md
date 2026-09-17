# Agents — Instruments monorepo

Read **`/ARCHITECTURE.md`** first, then the product `ARCHITECTURE.md` for the area you edit.

## Standards

- [docs/CURSOR_AGENT_PLAYBOOK.md](docs/CURSOR_AGENT_PLAYBOOK.md) — Agent Mode, Cloud, structured prompts  
- [docs/AGENTIC_PROJECT_STANDARDS.md](docs/AGENTIC_PROJECT_STANDARDS.md) — rules, PR policy, definition of done  
- `.cursor/rules/*.mdc` — always-on architecture and security  

## Disklordz SaaS (web)

```bash
cd disklordz/website && npm ci && npm run build && npm test  # if tests exist
```

Deploy: [disklordz/website/DEPLOY.md](disklordz/website/DEPLOY.md). Env: `NEXT_PUBLIC_SUPABASE_*`, `SUPABASE_SERVICE_ROLE_KEY` (prod kits), optional `SAAS_DAILY_GEN_LIMIT`.

Roadmap: [docs/DISKLORDZ_ILLUGEN_RESEARCH.md](docs/DISKLORDZ_ILLUGEN_RESEARCH.md) (WO-SAAS-007+).

## RAG (prompt knowledge)

```bash
python3 disklordz/rag/scripts/chunk_corpus.py
python3 disklordz/rag/scripts/query_local.py "your query"
```

Colab: [docs/COLAB_ZERO_INSTALL_TESTING.md](docs/COLAB_ZERO_INSTALL_TESTING.md).

## JUCE plugin (sandbox)

Cloud Agent `install` runs `./scripts/disklordz-sandbox.sh --ci` (see [.cursor/environment.json](.cursor/environment.json)).

Local or re-verify:

```bash
./scripts/setup-disklordz-integrations.sh onboard   # checks + full factory smoke
./scripts/disklordz-sandbox.sh --ci                   # same as Cloud install
./scripts/disklordz-sandbox.sh --full                 # + pluginval + MyFirstPlugin
```

Human onboarding (what still needs a browser once): [docs/SANDBOX_ONBOARDING.md](docs/SANDBOX_ONBOARDING.md).

## Git

- Do not force-push or deploy production unless the user asks.  
- Cloud feature branches: `cursor/<description>-<suffix>` when required by environment.  
