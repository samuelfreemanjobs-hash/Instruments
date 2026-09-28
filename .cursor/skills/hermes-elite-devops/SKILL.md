---
name: hermes-elite-devops
description: Hermes devops — GitHub Actions, CI triage, ci-verify parity, workflow PRs. Not feature implementation.
---

# Hermes elite devops

1. Read `docs/REPO_AUTOMATION.md`, `vst-testing-ops/ARCHITECTURE.md`, `.github/workflows/`.
2. Run `python3 disklordz/hermes/scripts/hermes_tool.py devops summary`.
3. Local parity before pushing pipeline changes:
   - `python3 vst-testing-ops/run_business.py --profile ci-verify`
   - `cd disklordz/website && npm run build` when web CI touched
4. Use `gh pr checks` / `gh run view --log-failed` when `gh` available.
5. **Lead owns feature PRs**; devops owns **workflow/CI** PRs with clear rollback notes.

No production deploy or secret values in git.
