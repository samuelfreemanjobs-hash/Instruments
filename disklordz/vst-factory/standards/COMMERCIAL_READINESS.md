# Commercial readiness & security

Factory executes AI-influenced builds. Treat compilation as **privileged** operations.

## Allowlisted subprocesses

Orchestrator and agents may invoke **only**:

| Script | Purpose |
|--------|---------|
| `tools/new-vst.sh` | Scaffold |
| `tools/build-vst.sh` | Compile APC plugin |
| `tools/package-vst.sh` | Release zip |
| `vst-testing-ops/run_business.py` | QA profiles |
| `disklordz/vst-factory/tools/analyze_wav.py` | Audio metrics |
| `git` | read-only: `status`, `diff`, `rev-parse`, `tag` (no push without human) |

No arbitrary `bash -c` from model output.

## Filesystem

Writes limited to: `plugins/<Name>/`, `vst/<Name>/`, `release/<Name>/`, `disklordz/vst-factory/` DB, and run logs.

Never write to system VST3 folders (`COPY_PLUGIN_AFTER_BUILD` stays off in CI/cloud).

## Builds

| Kind | Use |
|------|-----|
| `development` | Fast iteration, debug symbols |
| `release` | Requires `commercial.release_ready` + license checklist |

## SBOM

Template: [templates/release/sbom.template.json](../templates/release/sbom.template.json).  
Ship path: `release/<Name>/sbom.json` — enumerate JUCE, VST3 SDK, vendored code, fonts, samples.

## License gate

Before `commercial.license_checklist_passed: true`:

1. Run agent **`license-compliance`** ([DISKLORDZ_AGENTS.md](../../DISKLORDZ_AGENTS.md))
2. Complete [DiskLordz-Audio/LICENSES/README.md](../../DiskLordz-Audio/LICENSES/README.md) checklist
3. Human sign-off recorded in spec metadata

## Secrets

No API keys in specs, prompts, or git. Use env vars documented in `.env.example` only.
