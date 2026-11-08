# Branch protection (required checks)

GitHub **branch protection** for `main` should require the **Build** workflow job **`cmake`** before merge.

## One-time setup (repo admin)

```bash
OWNER=samuelfreemanjobs-hash
REPO=Instruments

gh api -X PUT "repos/${OWNER}/${REPO}/branches/main/protection" \
  --input - <<'EOF'
{
  "required_status_checks": {
    "strict": true,
    "contexts": ["cmake"]
  },
  "enforce_admins": false,
  "required_pull_request_reviews": null,
  "restrictions": null,
  "allow_force_pushes": false,
  "allow_deletions": false
}
EOF
```

Verify:

```bash
gh api "repos/${OWNER}/${REPO}/branches/main/protection" --jq '.required_status_checks.contexts'
```

If the API returns 404, enable **Settings → Branches → Branch protection rules** in the GitHub UI and add **`cmake`** as a required status check.

## CI Slack notifications

Workflow [`.github/workflows/ci-slack-notify.yml`](../.github/workflows/ci-slack-notify.yml) runs when **Build**, **Build MyFirstPlugin**, **Build Windows VST3**, or **Nightly QA** completes.

## Windows VST3 artifacts

Workflow [`.github/workflows/windows-vst3-monorepo.yml`](../.github/workflows/windows-vst3-monorepo.yml) builds on `windows-latest`:

| Target | Typical bundle name |
|--------|---------------------|
| `PluggVA_VST3` | `Plugg VA.vst3` |
| `Wave9090_VST3` | `WAVE-9090.vst3` |
| `SP1200_VST3` | `SP-1200 Drumulator.vst3` |
| `JDUpgraded_VST3` | `JD Upgraded.vst3` |

Download from the GitHub Actions run → **Artifacts** → `windows-vst3-<sha>`. Copy each `.vst3` folder to `C:\Program Files\Common Files\VST3\` (or your DAW path).

Windows CI configures with `-DJDUPGRADED_ENABLE_CLAP=OFF` so FetchContent does not pull `clap-juce-extensions` (shallow-clone fragility on GHA). Linux **Build** still builds CLAP by default.

`Wave9090Wavetable` on Windows runs bank/preset checks only (voice render smoke runs on Linux **Build** / ctest).

Configure the repo secret (admin):

```bash
./scripts/setup-disklordz-integrations.sh slack-ci --webhook-url 'https://hooks.slack.com/services/...'
```

Without `SLACK_WEBHOOK_URL`, the workflow skips quietly.

## Golden WAV policy

When DSP output changes **on purpose**, refresh committed references before merge:

```bash
cmake --build build -j --target OfflineRender SpectralDiff
./tests/golden/refresh_golden.sh
git add tests/golden/
git commit -m "Refresh golden WAVs after intentional DSP change"
```

Do **not** refresh goldens to make a failing pipeline green — fix the regression or update manifest thresholds in `verify_golden.sh` / `SpectralDiff` only with justification.
