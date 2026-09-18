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

Workflow [`.github/workflows/ci-slack-notify.yml`](../.github/workflows/ci-slack-notify.yml) runs when **Build**, **Build MyFirstPlugin**, or **Nightly QA** completes.

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
