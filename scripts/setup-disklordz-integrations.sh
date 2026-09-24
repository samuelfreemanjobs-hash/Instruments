#!/usr/bin/env bash
# Disklordz integrations: GitHub Actions → Slack (#disklordz-ci) and Cursor Cloud Agents.
set -euo pipefail

REPO_DEFAULT="samuelfreemanjobs-hash/Instruments"
SLACK_CHANNEL_NAME="disklordz-ci"
SLACK_CHANNEL_ID="C0C1X6RMGTZ"
ENV_JSON=".cursor/environment.json"
CURSOR_DOCS="https://cursor.com/docs/cloud-agent/setup"

usage() {
  cat <<'EOF'
Usage: setup-disklordz-integrations.sh <command> [options]

Commands:
  slack-ci          Configure GitHub secret SLACK_WEBHOOK_URL and test notifications
  slack-antigravity Configure SLACK_WEBHOOK_ANTIGRAVITY_URL + optional SLACK_MENTION_USER_ID (#disklordz-dev)
  slack-vst-factory Configure SLACK_WEBHOOK_VST_FACTORY_URL + optional SLACK_MENTION_VST_FACTORY_USER_ID
  cursor-cloud      Verify repo-managed Cursor Cloud Agent config (environment.json)
  all               Run cursor-cloud, then slack-ci (if webhook provided)

slack-ci options:
  --webhook-url URL     Incoming webhook URL (or set SLACK_WEBHOOK_URL)
  --repo OWNER/NAME     GitHub repo (default: samuelfreemanjobs-hash/Instruments)
  --skip-secret         Only send a test message; do not call gh secret set
  --skip-test           Set secret only; do not POST a test payload

cursor-cloud options:
  --repo OWNER/NAME     GitHub repo (default: samuelfreemanjobs-hash/Instruments)
  --trigger-build       Print instructions to validate the environment build (no API token required)

Examples:
  ./scripts/setup-disklordz-integrations.sh cursor-cloud
  ./scripts/setup-disklordz-integrations.sh slack-ci --webhook-url 'https://hooks.slack.com/services/...'
  ./scripts/setup-disklordz-integrations.sh slack-antigravity --webhook-url 'https://hooks.slack.com/services/...'
  ./scripts/setup-disklordz-integrations.sh slack-vst-factory --webhook-url 'https://hooks.slack.com/services/...'
  SLACK_WEBHOOK_URL='https://hooks.slack.com/...' ./scripts/setup-disklordz-integrations.sh all

Slack webhook (one-time, in browser):
  1. Slack → Apps → Incoming Webhooks → Add to Slack → channel #disklordz-ci
  2. Copy the webhook URL and pass it to slack-ci

Cursor GitHub access (one-time, in browser — cannot be fully scripted without your Cursor session):
  1. Cursor → Settings → Cloud Agents → connect GitHub and grant access to this repo
  2. Commit .cursor/environment.json (this repo already includes it)
EOF
}

die() {
  echo "error: $*" >&2
  exit 1
}

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "missing required command: $1"
}

repo_from_git() {
  local url
  url="$(git config --get remote.origin.url 2>/dev/null || true)"
  if [[ -z "$url" ]]; then
    echo "$REPO_DEFAULT"
    return
  fi
  url="${url%.git}"
  url="${url#git@github.com:}"
  url="${url#https://github.com/}"
  url="${url#https://x-access-token:*@github.com/}"
  echo "$url"
}

cmd_slack_ci() {
  local webhook_url="${SLACK_WEBHOOK_URL:-}"
  local repo="$REPO_DEFAULT"
  local skip_secret=0
  local skip_test=0

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --webhook-url)
        webhook_url="$2"
        shift 2
        ;;
      --repo)
        repo="$2"
        shift 2
        ;;
      --skip-secret)
        skip_secret=1
        shift
        ;;
      --skip-test)
        skip_test=1
        shift
        ;;
      *)
        die "unknown slack-ci option: $1"
        ;;
    esac
  done

  [[ -n "$webhook_url" ]] || die "set --webhook-url or SLACK_WEBHOOK_URL"

  if [[ "$skip_secret" -eq 0 ]]; then
    need_cmd gh
    gh auth status >/dev/null 2>&1 || die "run: gh auth login"
    echo "Setting GitHub secret SLACK_WEBHOOK_URL on $repo ..."
    printf '%s' "$webhook_url" | gh secret set SLACK_WEBHOOK_URL --repo "$repo"
    echo "Secret updated."
  fi

  if [[ "$skip_test" -eq 0 ]]; then
    need_cmd curl
    need_cmd jq
    echo "Sending test message to #${SLACK_CHANNEL_NAME} (${SLACK_CHANNEL_ID}) ..."
    curl -fsS -X POST "$webhook_url" \
      -H 'Content-type: application/json' \
      --data "$(jq -n \
        --arg ch "#${SLACK_CHANNEL_NAME}" \
        --arg repo "$repo" \
        '{
          text: ("Disklordz CI webhook test — GitHub Actions will post here after Build / Build MyFirstPlugin workflows complete."),
          blocks: [
            { type: "section", text: { type: "mrkdwn", text: ("*Disklordz CI connected*\nRepo: `" + $repo + "`\nChannel: " + $ch) } },
            { type: "context", elements: [ { type: "mrkdwn", text: "Triggered by setup-disklordz-integrations.sh slack-ci" } ] }
          ]
        }')" \
      >/dev/null
    echo "Test message sent."
  fi

  echo ""
  echo "Next: push any branch or re-run CI; workflow ci-slack-notify.yml listens for completed Build jobs."
  echo "Manual test: gh workflow run ci-slack-notify.yml --repo $repo -f conclusion=success"
}

cmd_slack_antigravity() {
  local webhook_url="${SLACK_WEBHOOK_ANTIGRAVITY_URL:-}"
  local mention_user_id="${SLACK_MENTION_USER_ID:-}"
  local repo="$REPO_DEFAULT"
  local skip_secret=0
  local skip_test=0

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --webhook-url)
        webhook_url="$2"
        shift 2
        ;;
      --mention-user-id)
        mention_user_id="$2"
        shift 2
        ;;
      --repo)
        repo="$2"
        shift 2
        ;;
      --skip-secret)
        skip_secret=1
        shift
        ;;
      --skip-test)
        skip_test=1
        shift
        ;;
      *)
        die "unknown slack-antigravity option: $1"
        ;;
    esac
  done

  [[ -n "$webhook_url" ]] || die "set --webhook-url or SLACK_WEBHOOK_ANTIGRAVITY_URL"

  if [[ "$skip_secret" -eq 0 ]]; then
    need_cmd gh
    gh auth status >/dev/null 2>&1 || die "run: gh auth login"
    echo "Setting GitHub secret SLACK_WEBHOOK_ANTIGRAVITY_URL on $repo ..."
    printf '%s' "$webhook_url" | gh secret set SLACK_WEBHOOK_ANTIGRAVITY_URL --repo "$repo"
    echo "Secret SLACK_WEBHOOK_ANTIGRAVITY_URL updated."
    if [[ -n "$mention_user_id" ]]; then
      printf '%s' "$mention_user_id" | gh secret set SLACK_MENTION_USER_ID --repo "$repo"
      echo "Secret SLACK_MENTION_USER_ID updated."
    fi
  fi

  if [[ "$skip_test" -eq 0 ]]; then
    need_cmd curl
    need_cmd jq
    echo "Sending Antigravity inbox test message ..."
    curl -fsS -X POST "$webhook_url" \
      -H 'Content-type: application/json' \
      --data "$(jq -n \
        --arg repo "$repo" \
        --arg mention "$mention_user_id" \
        '{
          text: (if $mention != "" then ("<@" + $mention + "> Antigravity inbox webhook test") else "Antigravity inbox webhook test" end),
          blocks: [
            { type: "section", text: { type: "mrkdwn", text: ("*Antigravity inbox Slack connected*\nRepo: `" + $repo + "`\nTriggers on `main` when `disklordz/antigravity/inbox/HO-*.json` is pushed.") } },
            { type: "context", elements: [ { type: "mrkdwn", text: "setup-disklordz-integrations.sh slack-antigravity" } ] }
          ]
        }')" \
      >/dev/null
    echo "Test message sent."
  fi

  echo ""
  echo "Workflow: antigravity-inbox-slack.yml (on push to disklordz/antigravity/inbox/HO-*.json)"
  echo "Manual test: gh workflow run antigravity-inbox-slack.yml --repo $repo -f handoff_path=disklordz/antigravity/inbox/<file>.json"
}

cmd_slack_vst_factory() {
  local webhook_url="${SLACK_WEBHOOK_VST_FACTORY_URL:-}"
  local mention_user_id="${SLACK_MENTION_VST_FACTORY_USER_ID:-}"
  local repo="$REPO_DEFAULT"
  local skip_secret=0
  local skip_test=0

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --webhook-url)
        webhook_url="$2"
        shift 2
        ;;
      --mention-user-id)
        mention_user_id="$2"
        shift 2
        ;;
      --repo)
        repo="$2"
        shift 2
        ;;
      --skip-secret)
        skip_secret=1
        shift
        ;;
      --skip-test)
        skip_test=1
        shift
        ;;
      *)
        die "unknown slack-vst-factory option: $1"
        ;;
    esac
  done

  [[ -n "$webhook_url" ]] || die "set --webhook-url or SLACK_WEBHOOK_VST_FACTORY_URL"

  if [[ "$skip_secret" -eq 0 ]]; then
    need_cmd gh
    gh auth status >/dev/null 2>&1 || die "run: gh auth login"
    echo "Setting GitHub secret SLACK_WEBHOOK_VST_FACTORY_URL on $repo ..."
    printf '%s' "$webhook_url" | gh secret set SLACK_WEBHOOK_VST_FACTORY_URL --repo "$repo"
    echo "Secret SLACK_WEBHOOK_VST_FACTORY_URL updated."
    if [[ -n "$mention_user_id" ]]; then
      printf '%s' "$mention_user_id" | gh secret set SLACK_MENTION_VST_FACTORY_USER_ID --repo "$repo"
      echo "Secret SLACK_MENTION_VST_FACTORY_USER_ID updated."
    fi
  fi

  if [[ "$skip_test" -eq 0 ]]; then
    need_cmd curl
    need_cmd jq
    echo "Sending VST Plugin Factory Slack test message ..."
    curl -fsS -X POST "$webhook_url" \
      -H 'Content-type: application/json' \
      --data "$(jq -n \
        --arg repo "$repo" \
        --arg mention "$mention_user_id" \
        '{
          text: (if $mention != "" then ("<@" + $mention + "> VST Plugin Factory Slack connected") else "VST Plugin Factory Slack connected" end),
          blocks: [
            { type: "section", text: { type: "mrkdwn", text: ("*VST Plugin Factory agent — Slack connected*\nRepo: `" + $repo + "`\nAgent: `cursor-vst-plugin-factory`\nInbox: `disklordz/vst-factory/inbox/HO-*.json` on `main`") } },
            { type: "context", elements: [ { type: "mrkdwn", text: "Schedule: Mon CI · Wed Night Circuit · Fri doc-sync · setup-disklordz-integrations.sh slack-vst-factory" } ] }
          ]
        }')" \
      >/dev/null
    echo "Test message sent."
  fi

  echo ""
  echo "Workflows: vst-factory-inbox-slack.yml, vst-factory-schedule-slack.yml"
  echo "Recommended webhook channel: #disklordz-dev (C0C1X6S61B9)"
  echo "Agent sync / roster: #agent-team (C0C3M2NDK7Z) — docs/SLACK_AGENT_TEAM.md"
  echo "Docs: docs/VST_PLUGIN_FACTORY_SLACK.md"
  echo "Manual test: gh workflow run vst-factory-schedule-slack.yml --repo $repo -f conclusion=success"
}

cmd_cursor_cloud() {
  local repo="$REPO_DEFAULT"
  local trigger_build=0

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --repo)
        repo="$2"
        shift 2
        ;;
      --trigger-build)
        trigger_build=1
        shift
        ;;
      *)
        die "unknown cursor-cloud option: $1"
        ;;
    esac
  done

  [[ -f "$ENV_JSON" ]] || die "missing $ENV_JSON — Cloud Agents need a repo-managed environment file"

  need_cmd jq
  jq empty "$ENV_JSON" 2>/dev/null || die "$ENV_JSON is not valid JSON"

  local name install
  name="$(jq -r '.name // empty' "$ENV_JSON")"
  install="$(jq -r '.install // empty' "$ENV_JSON")"
  [[ -n "$install" ]] || die "$ENV_JSON must define an install command"

  echo "Repository-managed Cursor Cloud Agent environment: OK"
  echo "  file:     $ENV_JSON"
  echo "  name:     ${name:-<unset>}"
  echo "  install:  ${install:0:72}..."

  if command -v gh >/dev/null 2>&1 && gh auth status >/dev/null 2>&1; then
    echo ""
    echo "GitHub repo: $repo"
    gh repo view "$repo" --json nameWithOwner,url -q '"  url: " + .url' 2>/dev/null || echo "  (could not read repo — check gh auth and repo name)"
  else
    echo ""
    echo "Install gh and run 'gh auth login' to verify repo access from this machine."
  fi

  cat <<EOF

Cursor Cloud Agents — finish GitHub connection in the browser (one-time per account/org):
  1. Open Cursor → Settings → Cloud Agents
  2. Connect GitHub and authorize repository: $repo
  3. Confirm the environment uses this repo's $ENV_JSON (repo file overrides dashboard defaults)

Docs: $CURSOR_DOCS
Dashboard (environments): https://cursor.com/dashboard/cloud-agents

After merge: new agents on $repo pick up $ENV_JSON from the checked-out branch.
EOF

  if [[ "$trigger_build" -eq 1 ]]; then
    cat <<EOF

Validate the environment build (optional):
  - Cursor dashboard → your environment → trigger a build from branch main
  - Or start a Cloud Agent on this repo; it runs the install command from $ENV_JSON
EOF
  fi
}

main() {
  local cmd="${1:-}"
  shift || true

  case "$cmd" in
    slack-ci)
      cmd_slack_ci "$@"
      ;;
    slack-antigravity)
      cmd_slack_antigravity "$@"
      ;;
    slack-vst-factory)
      cmd_slack_vst_factory "$@"
      ;;
    cursor-cloud)
      cmd_cursor_cloud "$@"
      ;;
    all)
      cmd_cursor_cloud
      echo ""
      cmd_slack_ci "$@"
      ;;
    -h | --help | help | "")
      usage
      [[ -z "$cmd" ]] && exit 0
      ;;
    *)
      die "unknown command: $cmd (try --help)"
      ;;
  esac
}

main "$@"
