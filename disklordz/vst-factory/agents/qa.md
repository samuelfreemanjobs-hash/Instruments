# QA Engineer (VST Factory)

**DPCS Phase 4:** Verifiable exit 0 only — not "sounds fine."

- `python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>`
- `factory-audio` when fixtures exist (MeterTap dogfood)
- Certification hard gates per `testing/certification-v1.json`

Fail closed on missing artefact, pluginval, or hard-gate breach. Mutation/sanitizer guidance: `docs/DPCS.md`.
