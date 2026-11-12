#!/usr/bin/env bash
# Upload Disklordz go-live GitHub Actions secrets interactively (values never printed or stored in git).
set -euo pipefail

FULL_REPO="samuelfreemanjobs-hash/Instruments"
EXPECTED_REPO="samuelfreemanjobs-hash/Instruments"

if [[ "$FULL_REPO" != "$EXPECTED_REPO" ]]; then
  echo "ERROR: FULL_REPO is not $EXPECTED_REPO. Aborting."
  exit 1
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "ERROR: GitHub CLI (gh) is not installed. See docs/DISKLORDZ_GITHUB_SECRETS_SETUP.md"
  exit 1
fi

if ! gh auth status >/dev/null 2>&1; then
  echo "ERROR: gh is not authenticated. Run: gh auth login"
  echo "  Choose: GitHub.com → HTTPS → Login with a web browser"
  exit 1
fi

if ! gh repo view "$FULL_REPO" >/dev/null 2>&1; then
  echo "ERROR: Cannot access repository $FULL_REPO (check gh auth and permissions)."
  exit 1
fi

echo "Repository: $FULL_REPO"
echo "This script reads secrets from your terminal only. Nothing is written to the repo."
echo "Press Enter on optional prompts to skip."
echo ""

# name|required (yes|no)
REQUIRED_SECRETS=(
  "SUPABASE_ACCESS_TOKEN|yes"
  "SUPABASE_PROJECT_REF|yes"
  "DISKLORDZ_URL|yes"
  "VERCEL_DEPLOY_HOOK_URL|yes"
  "VERCEL_TOKEN|yes"
  "VERCEL_PROJECT_ID|yes"
  "NEXT_PUBLIC_SUPABASE_URL|yes"
  "NEXT_PUBLIC_SUPABASE_ANON_KEY|yes"
  "SUPABASE_SERVICE_ROLE_KEY|yes"
  "STRIPE_SECRET_KEY|yes"
  "STRIPE_WEBHOOK_SECRET|yes"
  "STRIPE_PRO_PRICE_ID|yes"
)

OPTIONAL_SECRETS=(
  "SAAS_DAILY_GEN_LIMIT|no"
  "VERCEL_TEAM_ID|no"
)

upload_secret() {
  local name="$1"
  local required="$2"
  local label="$3"
  local value=""

  printf '%s\n' "$label"
  read -r -s value || true
  printf '\n'

  if [[ -z "${value}" ]]; then
    if [[ "$required" == "yes" ]]; then
      echo "FAIL: $name (required value was empty)"
      exit 1
    fi
    echo "SKIP: $name"
    return 0
  fi

  if ! gh secret set "$name" --repo "$FULL_REPO" --body "$value"; then
    echo "FAIL: $name (gh secret set returned an error)"
    unset value
    exit 1
  fi

  unset value
  echo "OK: $name"
}

echo "=== Required secrets ==="
for entry in "${REQUIRED_SECRETS[@]}"; do
  name="${entry%%|*}"
  req="${entry##*|}"
  upload_secret "$name" "$req" "Required — $name (hidden input):"
done

echo ""
echo "=== Optional secrets ==="
for entry in "${OPTIONAL_SECRETS[@]}"; do
  name="${entry%%|*}"
  req="${entry##*|}"
  case "$name" in
    SAAS_DAILY_GEN_LIMIT)
      hint="Optional — $name (blank = workflow default 20):"
      ;;
    VERCEL_TEAM_ID)
      hint="Optional — $name (blank unless Vercel project is under a team):"
      ;;
    *)
      hint="Optional — $name:"
      ;;
  esac
  upload_secret "$name" "$req" "$hint"
done

echo ""
echo "Done. Secret names were uploaded; values were not logged."
echo "Verify names only: gh secret list --repo $FULL_REPO"
echo "Next: https://github.com/$FULL_REPO/actions (Disklordz go-live — do not run until you are ready)."
