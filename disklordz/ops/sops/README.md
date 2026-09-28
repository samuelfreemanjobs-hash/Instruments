# Disklordz standard operating procedures (SOPs)

Maintained by **`hermes-sop`** with **owner seats** listed per doc. Machine index: [INDEX.json](INDEX.json).

| ID | Title | Owner |
|----|--------|--------|
| [SOP-HERMES-001](SOP-HERMES-001-seat-routing.md) | Seat routing | hermes-lead |
| [SOP-HERMES-002](SOP-HERMES-002-sop-maintenance.md) | SOP maintenance | hermes-sop |
| [SOP-OPS-001](SOP-OPS-001-work-order-lifecycle.md) | Work order lifecycle | hermes-ops |
| [SOP-OPS-002](SOP-OPS-002-pr-evidence-done.md) | PR + evidence + Done | hermes-ops |
| [SOP-DEV-001](SOP-DEV-001-plugin-ci.md) | Plugin CI | hermes-devops |
| [SOP-DEV-002](SOP-DEV-002-junova-finish-line.md) | Junova finish line | hermes-architect |
| [SOP-SAAS-001](SOP-SAAS-001-web-build-deploy.md) | SaaS build/deploy | hermes-web |
| [SOP-SEC-001](SOP-SEC-001-secrets-baseline.md) | Secrets baseline | hermes-security |
| [SOP-GTM-001](SOP-GTM-001-launch-checklist.md) | Launch checklist | hermes-gtm |
| [SOP-RAG-001](SOP-RAG-001-corpus-support.md) | RAG + support | hermes-support |
| [SOP-HANDOFF-001](SOP-HANDOFF-001-antigravity.md) | Antigravity handoff | hermes-handoff |
| [SOP-RESEARCH-001](SOP-RESEARCH-001-hyperresearch.md) | Hyperresearch | hermes-research |

```bash
python3 disklordz/hermes/scripts/hermes_tool.py sop audit
python3 disklordz/hermes/scripts/hermes_tool.py sop coverage --seat hermes-dsp
```
