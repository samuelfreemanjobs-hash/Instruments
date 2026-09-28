# Product finish playbook (monorepo)

**Policy:** No product is “done” until **automated gates** pass and **Tier C shippable** artefacts exist. Agents run finish scripts; humans only handle Tier D items that require accounts, signing, or hosts.

## Products and finish scripts

| Product | Finish definition | Automation entry |
|---------|-------------------|------------------|
| **Junova-X** | [Junova-X/docs/FINISH_LINE.md](../Junova-X/docs/FINISH_LINE.md) | `Junova-X/scripts/finish_line.sh`, `--profile junova-ship` |
| **JD Upgraded** | Goldens + `ci-verify` artefacts | `tests/golden/verify_golden.sh` (JD section) |
| **WAVE-909** | `Wave909Tests` + VST3 in CI | `run_business.py --profile ci-verify` |
| **Disklordz SaaS** | `npm run build` + tests | `run_business.py --profile full` |

Add a `FINISH_LINE.md` + `scripts/finish_line.sh` for any **new** product before calling it MVP.

## Agent loop (100% automated until shippable)

1. Read product `ARCHITECTURE.md` + `FINISH_LINE.md`.
2. Implement WO with smallest diff toward the **next failing gate**.
3. Run local profile: `python3 vst-testing-ops/run_business.py --profile junova-ship` (or product equivalent).
4. Commit with `WO-…` in message; push; ensure **Build** CI green.
5. Update `ROADMAP.md` / Airtable when a tier completes.
6. Repeat until Tier C green; open Tier D WOs for marketplace-only work.

## CI map

| Workflow | Profile | When |
|----------|---------|------|
| [build.yml](../.github/workflows/build.yml) | `ci-verify` + finish line `--mode ci` | Every PR / push |
| [nightly-qa.yml](../.github/workflows/nightly-qa.yml) | `junova-ship` + `full` | Daily |

## Definition of done (product)

- [ ] Tier B green on main
- [ ] Tier C green (demo zip + docs)
- [ ] `ARCHITECTURE.md` matches behaviour
- [ ] Tier D items filed as issues/WOs with owners (not blockers for “shippable demo”)

See also [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md).
