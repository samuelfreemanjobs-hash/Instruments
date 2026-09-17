# Sandbox & onboarding — what runs automatically vs what you do once

This repo includes a **JUCE factory sandbox**: a Linux environment that installs dependencies, builds **JD Upgraded**, and runs the same smoke checks as CI. You should not need to hand-install CMake packages or remember golden-test commands.

---

## One command (local Linux or Cloud Agent VM)

From the repository root:

```bash
chmod +x scripts/disklordz-sandbox.sh   # first time only
./scripts/disklordz-sandbox.sh --ci
```

| Flag | What it does |
|------|----------------|
| `--ci` | **Default for Cloud** — apt deps, configure, build, golden + OfflineRender smoke |
| `--pluginval` | Download [pluginval](https://github.com/Tracktion/pluginval) and validate the VST3 |
| `--myfirstplugin` | Build template plugins under `MyFirstPlugin/` |
| `--full` | `--ci` + pluginval + MyFirstPlugin |

Tools cache: `.sandbox/tools/pluginval` (gitignored).

---

## Cloud Agent environment (automatic)

File: [`.cursor/environment.json`](../.cursor/environment.json)

When you start a **Cloud Agent** on this repo, Cursor runs the `install` script, which calls **`scripts/disklordz-sandbox.sh --ci`**. That is your **sandbox** — agents arrive on a machine that already built the factory and passed smoke tests.

**You do not** need to run CMake manually before asking the agent to edit JUCE code, unless you changed dependencies and want to refresh locally.

After merging sandbox changes, trigger a fresh **environment build** in Cursor (dashboard → Cloud Agents → your environment → rebuild) so new agents pick up the updated install script.

---

## Integrations script (Slack + sanity checks)

```bash
./scripts/setup-disklordz-integrations.sh onboard
```

| Subcommand | Automated | You still do (browser, once) |
|------------|-----------|------------------------------|
| `cursor-cloud` | Validates `environment.json`, prints repo URL | [Cursor → Settings → Cloud Agents](https://cursor.com/docs/cloud-agent/setup) → connect **GitHub** → allow this repository |
| `slack-ci --webhook-url 'https://hooks.slack.com/...'` | Sets `SLACK_WEBHOOK_URL` secret via `gh`, sends test ping | Create Incoming Webhook in Slack → channel `#disklordz-ci` → paste URL into the command |
| `sandbox --ci` | Runs full factory sandbox | Nothing (needs sudo on local machine) |
| `onboard` | Runs cursor-cloud + sandbox `--ci` + prints Slack steps | GitHub connection + webhook URL when you want Slack |

Slack webhook creation (one time):

1. Slack → **Apps** → **Incoming Webhooks** → **Add to Slack**
2. Pick channel **#disklordz-ci**
3. Copy URL → run:

```bash
./scripts/setup-disklordz-integrations.sh slack-ci --webhook-url 'PASTE_URL_HERE'
```

Requires [`gh auth login`](https://cli.github.com/) on your machine once.

---

## GitHub Actions (already automatic on push)

| Workflow | Purpose |
|----------|---------|
| [build.yml](../.github/workflows/build.yml) | JD Upgraded build + golden smoke |
| [build-plugin.yml](../.github/workflows/build-plugin.yml) | MyFirstPlugin + pluginval |
| [sandbox.yml](../.github/workflows/sandbox.yml) | Manual **Run workflow** — full sandbox + pluginval on demand |

To run sandbox in GitHub without pushing code: **Actions → Disklordz sandbox → Run workflow**.

---

## Local VS Code (optional)

Open [disklordz.code-workspace](../disklordz.code-workspace) for CMake Tools + clangd against `build/compile_commands.json` after a sandbox build.

---

## What we cannot automate (credentials)

| Item | Why | Where to configure |
|------|-----|-------------------|
| Cursor ↔ GitHub | OAuth in your Cursor account | Cursor Settings → Cloud Agents |
| Slack webhook | Secret owned by Slack workspace | `setup-disklordz-integrations.sh slack-ci` |
| Supabase / Stripe / Vercel (website) | Production secrets | [disklordz/website/.env.example](../disklordz/website/.env.example), [DEPLOY.md](../disklordz/website/DEPLOY.md) |
| Airtable automation | API keys | [disklordz/automation/.env.example](../disklordz/automation/.env.example) |
| Antigravity / HISE on Windows | Your workstation | [disklordz/antigravity/env.windows.example](../disklordz/antigravity/env.windows.example) |

Agents and CI can build **plugins** and run **smoke tests** without those secrets.

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Cloud Agent build fails on apt | Re-run environment build; check [environment build logs](https://cursor.com/dashboard/cloud-agents) |
| `pluginval` fails | Run with `PLUGINVAL_STRICTNESS=3 ./scripts/disklordz-sandbox.sh --pluginval` to loosen; report flake |
| Golden WAV mismatch | Intentional DSP change → [tests/golden/](../tests/golden/) refresh per project docs |

---

## Related

- [AGENTS.md](../AGENTS.md) — agent entry point  
- [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md) — prompting and Cloud workflow  
- [docs/ARCHITECTURE.md](ARCHITECTURE.md) — JD Upgraded factory  
