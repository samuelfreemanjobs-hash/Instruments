#!/usr/bin/env python3
"""Generate per-agent automations and PM_ADD roster from profit registry."""

from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
REPO = ROOT.parents[2]
REGISTRY = ROOT.parent / "profit" / "registry.json"
AGENTS_DIR = ROOT / "agents"
MANIFEST = ROOT / "manifest.json"
PM_ADD = ROOT / "PM_ADD.md"
REPO_FLEET_ENTRY = REPO / "DISKLORDZ_AGENTS.md"
DOCS_FLEET = REPO / "docs" / "DISKLORDZ_AGENT_FLEET.md"
GITHUB_FLEET = REPO / ".github" / "agents" / "fleet.json"

# How the agent actually runs in this repo (for PM / fleet tables)
ACTIVATION: dict[str, str] = {
    "workflow-automation": "ci_on_push",
    "pm-agent": "ci_scheduled",
    "conversion-qa": "ci_manual",
    "billing-ops": "manual_stub",
    "ship-velocity": "ci_event",
    "async-generation": "runtime_inngest",
    "prompt-coach": "ci_on_push",
    "product-factory": "runtime_api",
    "spec-validator": "runtime_api",
    "lane-workflow": "cli",
    "ops-schema": "ci_manual",
    "marketing-glue": "manual_stub",
    "support-macro": "runtime_api",
    "desktop-ops": "manual_external",
    "factory-batch-gpu": "manual_external",
    "engine-swap": "runtime_env",
    "integration-health": "ci_scheduled",
    "churn-winback": "manual_stub",
    "seo-kit-pages": "runtime_build",
    "referral-affiliate": "runtime_stripe",
    "fraud-abuse": "runtime_api",
    "pricing-experiment": "manual_skill",
    "onboarding-concierge": "runtime_inngest",
    "daw-inbox-copilot": "cli_local",
    "airtable-wo-triage": "ci_event",
    "golden-wav-qa": "ci_on_pr",
    "competitive-intel": "manual_doc",
    "license-compliance": "manual_gate",
    "social-clip-factory": "cli_script",
    "analytics-interpreter": "manual_stub",
    "release-notes": "ci_script",
    "hardware-preset-designer": "ci_on_pr",
    "audio-plugin-coder": "manual_doc",
    "code-project-planner": "manual_doc",
    "ddsp-ml-engineer": "ci_on_pr",
    "audio-rd": "manual_doc",
}

CI_WORKFLOWS = [
    (".github/workflows/agent-fleet-governance.yml", "pm-agent, workflow-automation"),
    (".github/workflows/agent-fleet-health.yml", "integration-health, pm-agent"),
    (".github/workflows/agent-fleet-execute.yml", "pm-agent + executable fleet roles"),
    (".github/workflows/scaffold-agent-workflows.yml", "workflow-automation"),
    (".github/workflows/disklordz-go-live.yml", "conversion-qa, ops-schema"),
    (".github/workflows/rag-reindex.yml", "prompt-coach"),
    (".github/workflows/activate-integrations.yml", "ops-schema, integration-health"),
    (".github/workflows/airtable-antigravity-handoff.yml", "ship-velocity, airtable-wo-triage"),
    (".github/workflows/build.yml", "golden-wav-qa"),
    (".github/workflows/synth-forge.yml", "hardware-preset-designer"),
    (".github/workflows/serum-forge.yml", "hardware-preset-designer, ddsp-ml-engineer"),
    (".github/workflows/drum-synth-blueprint.yml", "ddsp-ml-engineer, audio-rd"),
]

# First-person announcements + one-line help for Disklordz
ANNOUNCE: dict[str, str] = {
    "workflow-automation": (
        "I'm **Workflow Automation** — I wire GitHub Actions, Inngest, and n8n stubs "
        "so every profit agent runs on a schedule or event without you babysitting scripts."
    ),
    "pm-agent": (
        "I'm **PM Agent (Fleet ADD)** — I publish the company-wide roster at repo root "
        "(`DISKLORDZ_AGENTS.md`, `.github/agents/fleet.json`) and CI proves the fleet is registered and executing."
    ),
    "conversion-qa": (
        "I'm **Conversion QA** — I run go-live smoke and Playwright checks so generate → "
        "preview → checkout never silently breaks after a deploy."
    ),
    "billing-ops": (
        "I'm **Billing Ops** — I watch Stripe credits, failed payments, and Pro status "
        "so revenue leaks get flagged before users churn."
    ),
    "ship-velocity": (
        "I'm **Ship Velocity** — I turn Airtable WOs into issues and PRs fast so SaaS "
        "features reach production while the idea is still hot."
    ),
    "async-generation": (
        "I'm **Async Generation** — I queue long kit jobs through Inngest so paid "
        "generations finish even when Vercel would time out."
    ),
    "prompt-coach": (
        "I'm **Prompt Coach** — I use RAG and lane docs to sharpen prompts and specs so "
        "free users hear their vibe in the WAVs and upgrade."
    ),
    "product-factory": (
        "I'm **Product Factory** — I batch storefront packs (KICKS/PERC folders) so you "
        "ship SKUs with metadata, not one-off ZIPs."
    ),
    "spec-validator": (
        "I'm **Spec Validator** — I reject bad GenerationSpec JSON before generate runs "
        "so you don't burn credits on nonsense BPM/key combos."
    ),
    "lane-workflow": (
        "I'm **Lane Workflow** — I drive prompt → spec → variations so phonk/drift/cyber "
        "lanes stay consistent across candidates."
    ),
    "ops-schema": (
        "I'm **Ops Schema** — I apply and verify Supabase migrations safely so auth, "
        "kits, and pgvector stay up while you ship schema."
    ),
    "marketing-glue": (
        "I'm **Marketing Glue** — I connect n8n flows for leads and email so traffic "
        "comes back without manual copy-paste."
    ),
    "support-macro": (
        "I'm **Support Macro** — I draft answers from your lane corpus so support is "
        "fast, accurate, and cheap."
    ),
    "desktop-ops": (
        "I'm **Desktop Ops** — I handle browser/vendor chores via Bytebot while you keep "
        "code changes in git where they belong."
    ),
    "factory-batch-gpu": (
        "I'm **Factory Batch GPU** — I orchestrate heavy catalog batches off the "
        "serverless path so big drops don't choke the site."
    ),
    "engine-swap": (
        "I'm **Engine Swap** — I manage remote AudioCraft/Stable workers with parametric "
        "fallback so Pro sound improves without bricking generate."
    ),
    "integration-health": (
        "I'm **Integration Health** — I hit `/api/integrations/status` and verify scripts "
        "so misconfigured env vars don't eat revenue overnight."
    ),
    "churn-winback": (
        "I'm **Churn Win-back** — I find lapsed Pro users and trigger opt-in win-back "
        "with credits so MRR recovers."
    ),
    "seo-kit-pages": (
        "I'm **SEO Kit Pages** — I turn factory metadata into indexable pages so organic "
        "search feeds signups."
    ),
    "referral-affiliate": (
        "I'm **Referral & Affiliate** — I design referral hooks in manifests and Stripe "
        "coupons so customers bring customers."
    ),
    "fraud-abuse": (
        "I'm **Fraud & Abuse** — I tighten rate limits and guest abuse heuristics so "
        "free tier doesn't become a free CDN for bots."
    ),
    "pricing-experiment": (
        "I'm **Pricing Experiment** — I structure Stripe price tests with /premortem "
        "discipline so ARPU moves with evidence."
    ),
    "onboarding-concierge": (
        "I'm **Onboarding Concierge** — I guide magic-link → first kit so new users "
        "hear value in minutes, not days."
    ),
    "daw-inbox-copilot": (
        "I'm **DAW Inbox Copilot** — I keep Downloads → inbox → DAW smooth so kits land "
        "where producers actually work."
    ),
    "airtable-wo-triage": (
        "I'm **Airtable WO Triage** — I route work orders to GitHub and Antigravity inbox "
        "JSON so PM and eng stay in sync."
    ),
    "golden-wav-qa": (
        "I'm **Golden WAV QA** — I run DSP golden regression so plugin quality backs "
        "your brand when you cross-sell Instruments."
    ),
    "competitive-intel": (
        "I'm **Competitive Intel** — I map ILLUGEN-shaped gaps to your WO backlog with "
        "tagged evidence, not hype."
    ),
    "license-compliance": (
        "I'm **License & Compliance** — I gate engine swaps on model licenses so "
        "commercial launch doesn't inherit NC surprises."
    ),
    "social-clip-factory": (
        "I'm **Social Clip Factory** — I cut short previews from kits for social so "
        "top-of-funnel content scales."
    ),
    "analytics-interpreter": (
        "I'm **Analytics Interpreter** — I summarize weekly MRR, gen volume, and credit burn "
        "from Stripe + Supabase with [executed] numbers."
    ),
    "release-notes": (
        "I'm **Release Notes** — I turn merged PRs into customer-facing changelog lines "
        "so upgrades feel trustworthy."
    ),
    "hardware-preset-designer": (
        "I'm **Hardware Preset Designer** — I run SynthForge to batch hardware patches from "
        "prompts and samples, with safety clamps and librarian-ready export staging."
    ),
    "audio-plugin-coder": (
        "I'm **Audio Plugin Coder (APC)** — I drive the Noizefield APC Dream→Ship workflow and "
        "wire it to this monorepo's JUCE CMake targets so VST3s ship with tested DSP."
    ),
    "code-project-planner": (
        "I'm **Code Project Planner** — I write PRDs and phase gates (Audio Programmer style) "
        "before any agent touches code, and I keep product memory in docs/RAG."
    ),
    "ddsp-ml-engineer": (
        "I'm **DDSP / ML Engineer** — I script PyTorch/DDSP losses and batch drum synthesis "
        "so 808/kick timbre matches targets without hand-tuning every sample."
    ),
    "audio-rd": (
        "I'm **Audio R&D** — I design experiments and ablations for ML-backed drums, log promote/kill "
        "decisions, and hand off training to DDSP and shipping to the JUCE (APC) agent."
    ),
}

WORKFLOWS: dict[str, dict] = {
    "workflow-automation": {
        "platform": "github",
        "trigger": "workflow_dispatch",
        "workflow_file": ".github/workflows/scaffold-agent-workflows.yml",
        "steps": ["Run scaffold_workflows.py", "Open PR if manifest diff"],
    },
    "pm-agent": {
        "platform": "github",
        "trigger": "daily + push fleet docs",
        "workflow_file": ".github/workflows/agent-fleet-governance.yml",
        "steps": [
            "sync-disklordz-agent-fleet.sh",
            "scaffold_agents.py --check",
            "Fail if DISKLORDZ_AGENTS.md drift",
        ],
    },
    "conversion-qa": {
        "platform": "github",
        "trigger": "schedule + post-deploy",
        "workflow_file": ".github/workflows/disklordz-go-live.yml",
        "steps": ["verify-go-live.sh", "Optional Playwright MCP job"],
    },
    "billing-ops": {
        "platform": "manual",
        "trigger": "weekly + stripe webhook",
        "workflow_file": "disklordz/agents/workflows/n8n/billing-ops-stub.json",
        "steps": ["Stripe MCP read", "Human approve writes"],
    },
    "ship-velocity": {
        "platform": "github",
        "trigger": "repository_dispatch",
        "workflow_file": ".github/workflows/airtable-antigravity-handoff.yml",
        "steps": ["Airtable WO", "Issue + inbox JSON"],
    },
    "async-generation": {
        "platform": "inngest",
        "trigger": "disklordz/generate.requested",
        "workflow_file": "disklordz/website/src/inngest/functions.ts",
        "steps": ["POST /api/generate/async", "buildVariationBatch step"],
    },
    "prompt-coach": {
        "platform": "github",
        "trigger": "push docs/**",
        "workflow_file": ".github/workflows/rag-reindex.yml",
        "steps": ["chunk_corpus", "embed_and_upsert optional"],
    },
    "product-factory": {
        "platform": "api",
        "trigger": "POST /api/factory/batch",
        "workflow_file": "disklordz/website/src/app/api/factory/batch/route.ts",
        "steps": ["buildProductPack", "ZIP download"],
    },
    "spec-validator": {
        "platform": "api",
        "trigger": "pre-generate",
        "workflow_file": "disklordz/integrations/agents/pydantic_spec_agent.py",
        "steps": ["parseGenerationSpec", "402/400 on invalid"],
    },
    "lane-workflow": {
        "platform": "script",
        "trigger": "cli",
        "workflow_file": "disklordz/integrations/agents/langgraph_spec_flow.py",
        "steps": ["prompt in", "GenerationSpec JSON out"],
    },
    "ops-schema": {
        "platform": "github",
        "trigger": "workflow_dispatch",
        "workflow_file": ".github/workflows/activate-integrations.yml",
        "steps": ["supabase db push", "migration list in DEPLOY.md"],
    },
    "marketing-glue": {
        "platform": "n8n",
        "trigger": "cron",
        "workflow_file": "disklordz/agents/workflows/n8n/marketing-glue-stub.json",
        "steps": ["Lead webhook", "Email sequence"],
    },
    "support-macro": {
        "platform": "api",
        "trigger": "POST /api/rag/suggest",
        "workflow_file": "disklordz/website/src/app/api/rag/suggest/route.ts",
        "steps": ["hybrid retrieve", "optional AI polish"],
    },
    "desktop-ops": {
        "platform": "external",
        "trigger": "manual",
        "workflow_file": "docs/BYTEBOT_SETUP.md",
        "steps": ["Local Docker Bytebot", "No repo auto-commit"],
    },
    "factory-batch-gpu": {
        "platform": "trigger.dev",
        "trigger": "long batch",
        "workflow_file": "disklordz/integrations/engines/README.md",
        "steps": ["GPU worker", "Storage upload"],
    },
    "engine-swap": {
        "platform": "docker",
        "trigger": "DISKLORDZ_ENGINE=remote",
        "workflow_file": "disklordz/integrations/docker-compose.optional.yml",
        "steps": ["Engine stubs :8765/:8766", "remote-wav.ts fallback"],
    },
    "integration-health": {
        "platform": "github",
        "trigger": "daily",
        "workflow_file": ".github/workflows/agent-fleet-health.yml",
        "steps": ["verify-integrations.sh", "Slack optional"],
    },
    "churn-winback": {
        "platform": "n8n",
        "trigger": "weekly",
        "workflow_file": "disklordz/agents/workflows/n8n/churn-winback-stub.json",
        "steps": ["Query lapsed Pro", "Opt-in email + credit"],
    },
    "seo-kit-pages": {
        "platform": "vercel",
        "trigger": "build",
        "workflow_file": "disklordz/website/next build",
        "steps": ["Static/ISR kit pages TBD", "sitemap"],
    },
    "referral-affiliate": {
        "platform": "stripe",
        "trigger": "checkout",
        "workflow_file": "STRIPE_PRO_PRICE_ID + coupon",
        "steps": ["Referral code in manifest", "Webhook ledger"],
    },
    "fraud-abuse": {
        "platform": "api",
        "trigger": "every POST /api/generate",
        "workflow_file": "disklordz/website/src/lib/rate-limit.ts",
        "steps": ["IP cap", "429 guest tier"],
    },
    "pricing-experiment": {
        "platform": "manual",
        "trigger": "/premortem",
        "workflow_file": ".claude/skills/premortem/SKILL.md",
        "steps": ["Cohort flag", "Stripe price swap"],
    },
    "onboarding-concierge": {
        "platform": "inngest",
        "trigger": "auth signup",
        "workflow_file": "disklordz/website/src/inngest/functions.ts",
        "steps": ["Event TBD user.signed_up", "RAG suggest drip"],
    },
    "daw-inbox-copilot": {
        "platform": "cli",
        "trigger": "local watch",
        "workflow_file": "disklordz/daw-inbox/package.json",
        "steps": ["inbox-watch", "Browser save to folder"],
    },
    "airtable-wo-triage": {
        "platform": "github",
        "trigger": "repository_dispatch",
        "workflow_file": ".github/workflows/airtable-antigravity-handoff.yml",
        "steps": ["wo_to_antigravity_handoff.py"],
    },
    "golden-wav-qa": {
        "platform": "github",
        "trigger": "pull_request paths Source/**",
        "workflow_file": ".github/workflows/build.yml",
        "steps": ["run_business.py ci profile"],
    },
    "competitive-intel": {
        "platform": "manual",
        "trigger": "monthly",
        "workflow_file": "docs/DISKLORDZ_ILLUGEN_RESEARCH.md",
        "steps": ["Research append", "R8 tags"],
    },
    "license-compliance": {
        "platform": "gate",
        "trigger": "pre-engine-swap",
        "workflow_file": "disklordz/agents/profit/license-compliance/agent.md",
        "steps": ["Checklist sign-off", "Block remote engine"],
    },
    "social-clip-factory": {
        "platform": "script",
        "trigger": "manual",
        "workflow_file": "disklordz/agents/workflows/scripts/social-clip-stub.sh",
        "steps": ["ffmpeg 15s", "Upload to marketing bucket TBD"],
    },
    "analytics-interpreter": {
        "platform": "n8n",
        "trigger": "weekly Monday",
        "workflow_file": "disklordz/agents/workflows/n8n/analytics-weekly-stub.json",
        "steps": ["Stripe + Supabase export", "Slack summary"],
    },
    "release-notes": {
        "platform": "github",
        "trigger": "release published",
        "workflow_file": "disklordz/agents/workflows/scripts/release-notes-from-prs.sh",
        "steps": ["gh pr list merged", "Customer-facing MD"],
    },
    "hardware-preset-designer": {
        "platform": "github",
        "trigger": "pull_request paths tools/synth-forge/**",
        "workflow_file": ".github/workflows/synth-forge.yml",
        "steps": [
            "pytest tools/synth-forge/tests",
            "hardware_preset_designer.py --self-test",
            "Optional uvicorn studio smoke",
        ],
    },
}


def yaml_escape(s: str) -> str:
    return s.replace('"', '\\"')


def write_automation(agent_id: str, title: str, wf: dict) -> None:
    dest = AGENTS_DIR / agent_id
    dest.mkdir(parents=True, exist_ok=True)
    lines = [
        f"# Automation — {title} (`{agent_id}`)",
        "",
        f"platform: {wf.get('platform', 'manual')}",
        f"trigger: {wf.get('trigger', 'manual')}",
        f"primary: {wf.get('workflow_file', 'TBD')}",
        "",
        "## Steps",
    ]
    for i, step in enumerate(wf.get("steps") or [], 1):
        lines.append(f"{i}. {step}")
    lines.extend(
        [
            "",
            "## Invoke agent",
            f"Read: disklordz/agents/profit/{agent_id}/agent.md",
            "",
            "## PM ADD",
            f"See: disklordz/agents/workflows/PM_ADD.md#{agent_id}",
            "",
        ]
    )
    (dest / "automation.yaml").write_text("\n".join(lines), encoding="utf-8")


def build_pm_add(entries: list[dict]) -> str:
    lines = [
        "# PM ADD — Profit agent roster",
        "",
        "Product management **ADD** board: who we are, how we help Disklordz, and where the automation lives.",
        "",
        "Maintained by **Workflow Automation** — regenerate:",
        "",
        "```bash",
        "python3 disklordz/agents/workflows/scaffold_workflows.py",
        "```",
        "",
        "---",
        "",
    ]
    for e in entries:
        aid = e["id"]
        title = e["title"]
        ann = ANNOUNCE.get(aid, f"I'm **{title}** — assigned to profit lever: {e.get('profit_lever', 'TBD')}.")
        wf = WORKFLOWS.get(aid, {})
        lines.append(f"## {aid}")
        lines.append("")
        lines.append(ann)
        lines.append("")
        if wf:
            lines.append(
                f"- **Automation:** `{wf.get('workflow_file', 'TBD')}` ({wf.get('platform', 'manual')}, {wf.get('trigger', 'manual')})"
            )
        lines.append(f"- **Agent docs:** [`profit/{aid}/agent.md`](../profit/{aid}/agent.md)")
        lines.append(f"- **Workflow file:** [`agents/{aid}/automation.yaml`](agents/{aid}/automation.yaml)")
        lines.append("")
    return "\n".join(lines)


def build_disklordz_agents_md(entries: list[dict], manifest_agents: list[dict]) -> str:
    lines = [
        "# Disklordz agent fleet (repo-wide)",
        "",
        "Company-wide index for **Instruments / Disklordz**. Every Cursor Cloud Agent, Claude Code session, "
        "and engineer should discover agents here — not only under `disklordz/agents/`.",
        "",
        "| Resource | Purpose |",
        "|----------|---------|",
        "| [PM ADD roster](disklordz/agents/workflows/PM_ADD.md) | Who we are + automation pointers |",
        "| [Agent fleet table](docs/DISKLORDZ_AGENT_FLEET.md) | Activation status + paths |",
        "| [`.github/agents/fleet.json`](.github/agents/fleet.json) | Machine index for CI and tooling |",
        "| [Profit trees](disklordz/agents/profit/) | `agent.md`, `skill.md`, … per agent |",
        "| [GitHub Copilot skills](.github/skills/) | `disklordz-<id>/SKILL.md` |",
        "",
        "## Orchestration (read first)",
        "",
        "| ID | Role |",
        "|----|------|",
        "| **workflow-automation** | Scaffolds PM ADD + per-agent `automation.yaml` |",
        "| **pm-agent** | Repo-wide fleet governance + scheduled execution checks |",
        "",
        f"**Fleet size:** {len(manifest_agents)} agents (includes orchestration).",
        "",
        "## Active CI (executing now)",
        "",
        "| Workflow | Serves |",
        "|----------|--------|",
    ]
    for wf, serves in CI_WORKFLOWS:
        lines.append(f"| `{wf}` | {serves} |")
    lines.extend(
        [
            "",
            "## Regenerate (Workflow Automation + PM Agent)",
            "",
            "```bash",
            "./scripts/sync-disklordz-agent-fleet.sh",
            "```",
            "",
            "Maintained by `scaffold_workflows.py` — do not hand-edit sections below the marker.",
            "",
            "<!-- FLEET_ROSTER_BEGIN -->",
            "",
        ]
    )
    for e in manifest_agents:
        aid = e["id"]
        title = e.get("title") or aid
        act = ACTIVATION.get(aid, "manual")
        lines.append(f"- `{aid}` — **{title}** (`{act}`)")
    lines.extend(["", "<!-- FLEET_ROSTER_END -->", ""])
    return "\n".join(lines)


def build_docs_fleet_md(entries: list[dict], manifest_agents: list[dict]) -> str:
    lines = [
        "# Disklordz agent fleet — activation matrix",
        "",
        "Repo-wide companion to [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) and "
        "[PM ADD](../disklordz/agents/workflows/PM_ADD.md).",
        "",
        "| ID | Title | Activation | Primary automation | Skill |",
        "|----|-------|------------|--------------------|-------|",
    ]
    for e in manifest_agents:
        aid = e["id"]
        title = e.get("title") or aid
        act = ACTIVATION.get(aid, "manual")
        wf = WORKFLOWS.get(aid, {})
        primary = wf.get("workflow_file", "TBD")
        skill = f"`.github/skills/disklordz-{aid}/SKILL.md`"
        lines.append(f"| `{aid}` | {title} | `{act}` | `{primary}` | {skill} |")
    lines.extend(
        [
            "",
            "### Activation legend",
            "",
            "| Code | Meaning |",
            "|------|---------|",
            "| `ci_scheduled` | GitHub Actions cron runs fleet health / governance |",
            "| `ci_on_push` | Workflow runs when mapped paths change |",
            "| `ci_on_pr` | Plugin / website CI on pull request |",
            "| `runtime_api` | Live on Vercel API routes |",
            "| `runtime_inngest` | Inngest functions in production |",
            "| `manual_stub` | n8n JSON stub — import + secrets required |",
            "",
            "Regenerate: `./scripts/sync-disklordz-agent-fleet.sh`",
            "",
        ]
    )
    return "\n".join(lines)


def build_github_fleet_json(manifest_agents: list[dict]) -> dict:
    return {
        "version": 2,
        "scope": "repo-wide",
        "entrypoints": {
            "human": "DISKLORDZ_AGENTS.md",
            "pm_add": "disklordz/agents/workflows/PM_ADD.md",
            "architecture": "disklordz/agents/ARCHITECTURE.md",
        },
        "orchestration": ["workflow-automation", "pm-agent"],
        "agent_count": len(manifest_agents),
        "agents": [
            {
                "id": e["id"],
                "title": e.get("title"),
                "activation": ACTIVATION.get(e["id"], "manual"),
                "announcement": e.get("announcement", ""),
                "automation": e.get("automation", {}),
                "profit_entry": f"disklordz/agents/profit/{e['id']}/agent.md",
                "copilot_skill": f".github/skills/disklordz-{e['id']}/SKILL.md",
                "automation_path": e.get("automation_path"),
            }
            for e in manifest_agents
        ],
        "ci_workflows": [{"path": p, "serves": s} for p, s in CI_WORKFLOWS],
    }


def main() -> None:
    registry = json.loads(REGISTRY.read_text(encoding="utf-8"))
    agents = registry.get("agents") or []

    # Ensure orchestration agents in roster even before registry regen
    extras = [
        {
            "id": "workflow-automation",
            "title": "Workflow Automation",
            "profit_lever": "Agents run without manual babysitting",
        },
        {
            "id": "pm-agent",
            "title": "PM Agent (Fleet ADD)",
            "profit_lever": "No hidden agents — fleet is discoverable and running",
        },
    ]
    for extra in reversed(extras):
        if not any(a["id"] == extra["id"] for a in agents):
            agents = [extra] + agents

    manifest_agents = []
    for a in agents:
        aid = a["id"]
        wf = WORKFLOWS.get(aid, {})
        write_automation(aid, a.get("title", aid), wf)
        manifest_agents.append(
            {
                "id": aid,
                "title": a.get("title"),
                "announcement": ANNOUNCE.get(aid, ""),
                "automation": wf,
                "automation_path": f"disklordz/agents/workflows/agents/{aid}/automation.yaml",
            }
        )

    manifest = {"version": 1, "agent_count": len(manifest_agents), "agents": manifest_agents}
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    PM_ADD.write_text(build_pm_add(agents), encoding="utf-8")

    GITHUB_FLEET.parent.mkdir(parents=True, exist_ok=True)
    GITHUB_FLEET.write_text(
        json.dumps(build_github_fleet_json(manifest_agents), indent=2) + "\n", encoding="utf-8"
    )
    REPO_FLEET_ENTRY.write_text(build_disklordz_agents_md(agents, manifest_agents), encoding="utf-8")
    DOCS_FLEET.write_text(build_docs_fleet_md(agents, manifest_agents), encoding="utf-8")

    print(
        f"Wrote PM_ADD + fleet entrypoints + {len(manifest_agents)} automation.yaml files"
    )


if __name__ == "__main__":
    main()
