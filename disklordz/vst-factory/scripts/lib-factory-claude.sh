# shellcheck shell=bash
# Sourced by run-factory-*.sh — Claude Code non-interactive customize pass.

factory_have_claude() {
  command -v claude >/dev/null 2>&1
}

# Usage: factory_run_claude "$prompt" "$log_file" "$max_turns"
factory_run_claude() {
  local prompt="$1"
  local log_file="$2"
  local max_turns="${3:-28}"
  if ! factory_have_claude; then
    echo "[SKIP] claude CLI not on PATH — wrote prompt to ${log_file%.log}.customize.prompt.txt"
    printf '%s\n' "$prompt" >"${log_file%.log}.customize.prompt.txt"
    return 1
  fi
  # Read AGENTS.md + CLAUDE_VST_ENGINEER.md bootstrap inline for headless runs.
  claude -p "$prompt" \
    --dangerously-skip-permissions \
    --max-turns "$max_turns" \
    --allowedTools "Bash,Read,Write,Edit,Glob,Grep" \
    2>&1 | tee -a "$log_file"
}