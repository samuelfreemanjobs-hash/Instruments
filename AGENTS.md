# Agents — Disklordz / Instruments

## PM of record

**Airtable Disklordz OS** — work orders, products, status.  
Rhythm: [disklordz/company/OPERATING_RHYTHM.md](disklordz/company/OPERATING_RHYTHM.md).  
Automation: [disklordz/automation/README.md](disklordz/automation/README.md).

## Star product (SaaS)

[v0 spec](docs/DISKLORDZ_SAAS_V0.md) — prompt → preview → download; path to subscription after v0.

## Read first (code)

1. [ARCHITECTURE.md](ARCHITECTURE.md)
2. [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — JD Upgraded plugin
3. [docs/HANDOFF.md](docs/HANDOFF.md)

## Build and verify (plugin)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
./tests/golden/verify_golden.sh
```

## Cloud Agent

- [.cursor/environment.json](.cursor/environment.json)
- `./scripts/setup-disklordz-integrations.sh cursor-cloud`
- Work orders → issues: workflow `airtable-work-order-to-github.yml`

## Scope

- Minimize diff; no heap on audio thread.
- PR titles for WOs: `[WO-2026-NNN] …`
