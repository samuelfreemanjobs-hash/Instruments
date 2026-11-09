# shared — learnings (validated lessons)

Format and rules: [MEMORY.md](MEMORY.md).

### L-0001 · Windows operator machine conventions
- **date:** 2026-11-08
- **area:** windows, powershell, git, build
- **when:** any agent working on the operator's Windows laptop (C:\dev\Instruments)
- **lesson:** PowerShell ignores bash syntax: `export X=..` and `\` line continuations fail. Other agents share C:\dev\Instruments; switching branches there breaks them. Building the root project takes 30+ minutes.
- **do:** `$env:X = 'value'` (lasts for that window only); run repo `.sh` with `bash ./path.sh` (Git Bash); `git worktree add C:\dev\Instruments-<task> -b cursor/<task> origin/<base>`; build one CMake target. See SOP-VF-12.
- **evidence:** 2026-11-08 factory session: user pasted bash exports into PowerShell; worktrees used for every task
- **confidence:** executed
- **status:** active

### L-0002 · Local gh token cannot push workflow files
- **date:** 2026-11-09
- **area:** github, ci, permissions
- **when:** changing anything under .github/workflows/ from the operator's Windows machine
- **lesson:** Push is rejected: "refusing to allow an OAuth App to create or update workflow ... without workflow scope". Cloud agents can push workflow files.
- **do:** Send the exact workflow change to the Cloud agent that owns the branch (Task resume), or ask the operator to refresh gh auth with the workflow scope. Do not retry the push.
- **evidence:** metertap.yml X11 fix rejected locally; pushed by Cloud agent, run 42571324884 green
- **confidence:** executed
- **status:** active

### L-0003 · Merging PRs requires explicit operator approval
- **date:** 2026-11-09
- **area:** git, github, approvals
- **when:** any agent wants to merge a PR (even after the operator said "continue autonomously")
- **lesson:** Auto-review blocks autonomous `gh pr merge` as a shared-state write. Broad autonomy is not merge approval.
- **do:** Get CI green, then ask with the exact PR list, targets and order. Merge with `gh pr merge N --merge --match-head-commit <sha>` so nothing unreviewed sneaks in.
- **evidence:** merge script blocked 2026-11-09 00:12; operator then approved "Merge 123-125"
- **confidence:** executed
- **status:** active

### L-0004 · Shell redirects corrupt binary files on Windows
- **date:** 2026-11-09
- **area:** windows, git, binary, evidence
- **when:** extracting a binary file (PNG, zip, wav) from git or a command with `>` in PowerShell or cmd
- **lesson:** Newline translation in the redirect produces an undecodable file, which looks like a broken artefact when it isn't.
- **do:** Write bytes from Python: `subprocess.run(["git","show",f"{ref}:{path}"],capture_output=True).stdout` then `Path(...).write_bytes(...)`.
- **evidence:** MeterTap renders unreadable via cmd redirect; valid PNG headers via Python
- **confidence:** executed
- **status:** active

### L-0005 · Never set placeholder secrets
- **date:** 2026-11-08
- **area:** secrets, slack, config
- **when:** a user pastes example config such as `.../services/...` or `<your real ... URL>`
- **lesson:** Setting the placeholder makes every integration call fail; leaving it unset makes integrations no-op cleanly (factory Slack returns a preview).
- **do:** Leave unset; tell the user how to set the real value in their own shell; never ask them to paste a secret into chat.
- **evidence:** VST Factory Slack setup; real webhook set by operator, test returned sent: True
- **confidence:** executed
- **status:** active

### L-0006 · Work that only exists on another machine is lost
- **date:** 2026-11-08
- **area:** cloud-agents, git, bundles
- **when:** a plan depends on a git bundle, local clone or file produced on a Cloud agent VM or another computer
- **lesson:** Fresh Cloud agents start from GitHub and cannot see another VM's files; an expired agent cannot be messaged from local tools.
- **do:** Agents push branches before finishing. If an artefact is missing, rebuild from the spec on a new branch rather than block (SOP-VF-11 step 8).
- **evidence:** metertap-m1-cf8c.bundle never found; missing factory modules rebuilt in PR #121
- **confidence:** executed
- **status:** active

### L-0007 · pluginval download flakes in CI
- **date:** 2026-11-09
- **area:** ci, pluginval, flaky
- **when:** cmake or linux-vst3 jobs fail in "Cache pluginval binary" / "Validate VST3 plugins" with HTTP 504
- **lesson:** Upstream download timeout, not a code failure; it recurred on 3 PRs in one night.
- **do:** `gh run rerun <run-id> --failed`. Candidate promotion: cache pluginval in CI.
- **evidence:** PR #122 and #124 runs 42569578862, 42571109192 (HTTP Error 504)
- **confidence:** executed
- **status:** active

### L-0008 · Stacked PRs after a base merges
- **date:** 2026-11-09
- **area:** git, github, prs
- **when:** a PR's base branch was just merged, or main changed under an open PR
- **lesson:** Dependents keep pointing at the old base; `mergeable` reads UNKNOWN for a few seconds after the base moves.
- **do:** `gh pr edit N --base <new-base>`; wait ~15 s and re-read `mergeable` before merging; never guess.
- **evidence:** #127 retargeted; #124 mergeable UNKNOWN right after #123 merged, MERGEABLE 15 s later
- **confidence:** executed
- **status:** active

### L-0009 · Path-filtered workflows leave contract checks unrun
- **date:** 2026-11-09
- **area:** ci, github-actions, workflows, coverage
- **when:** adding a check that should guard the whole repo
- **lesson:** Checks wired only into path-filtered workflows (e.g. design-system.yml) never ran for changes elsewhere; templates, doc links, memory and factory tests had no CI at all before repo-health.
- **do:** Put repo-wide contracts in .github/workflows/repo-health.yml (always-on, continue-on-error steps + summary that fails the job); mirror locally with bash scripts/repo_health.sh.
- **evidence:** PR #130 run 42580463051 (9 steps, 22 s)
- **confidence:** executed
- **status:** active
