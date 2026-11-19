# DiskLordz autonomous agent rules (all agents)

Read `AGENTS.md`, `docs/CLAUDE_VST_ENGINEER.md`, and the product `ARCHITECTURE.md` for the area you touch.

## Scope

1. Fix **only** what the task asks — minimal diff.
2. Keep **CI green** on the PR branch you touch.
3. For **plugin PRs**: ensure GitHub Actions produces a downloadable **Windows x64 VST3 artifact** (add or fix the workflow if missing).
4. **Never merge** pull requests yourself.
5. **Never** publish releases, post to Slack/social, or spam comments; PR comments only when summarizing your own run (where the workflow allows it).
6. **Never** modify committed **master/golden audio** (shipped WAV libraries, golden references) unless the task explicitly requires it — prefer DSP fixes.
7. **Never commit secrets** or `.env` values; use documented secret names only.
8. Peak ceiling **-0.5 dBFS** on masters; follow DiskLordz naming conventions; treat masters as **read-only** unless the slice says otherwise.