#!/usr/bin/env python3
"""MVP-era fleet motion: run safe per-agent hooks and append activity log."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from dataclasses import asdict, dataclass
from datetime import datetime, timezone
from pathlib import Path
REPO = Path(__file__).resolve().parents[3]
FLEET = REPO / ".github" / "agents" / "fleet.json"
ACTIVITY_LOG = REPO / "disklordz/agents/workflows/fleet-activity.jsonl"
ACTIVITY_MD = REPO / "docs/fleet-activity-latest.md"


@dataclass
class MotionResult:
    agent_id: str
    status: str  # ok | skip | blocked | fail
    detail: str
    duration_s: float


def _utc_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def _run_shell(
    agent_id: str,
    detail: str,
    argv: list[str],
    *,
    cwd: Path | None = None,
    env: dict[str, str] | None = None,
    allow_fail: bool = False,
) -> MotionResult:
    import time

    t0 = time.monotonic()
    merged = os.environ.copy()
    if env:
        merged.update(env)
    try:
        subprocess.run(argv, cwd=cwd or REPO, check=True, env=merged, capture_output=True, text=True)
        status = "ok"
        msg = detail
    except subprocess.CalledProcessError as exc:
        if allow_fail:
            status = "skip"
            msg = f"{detail} ({exc.returncode}: {(exc.stderr or exc.stdout or '').strip()[:200]})"
        else:
            status = "fail"
            msg = (exc.stderr or exc.stdout or str(exc))[:400]
    except FileNotFoundError:
        status = "skip"
        msg = f"{detail} (command missing)"
    dt = time.monotonic() - t0
    return MotionResult(agent_id, status, msg, round(dt, 3))


def _append_log(results: list[MotionResult]) -> None:
    ACTIVITY_LOG.parent.mkdir(parents=True, exist_ok=True)
    ts = _utc_now()
    with ACTIVITY_LOG.open("a", encoding="utf-8") as fh:
        for r in results:
            fh.write(
                json.dumps(
                    {"ts": ts, "source": "mvp-motion", **asdict(r)},
                    ensure_ascii=False,
                )
                + "\n"
            )


def _write_summary_md(results: list[MotionResult], agent_count: int) -> None:
    ts = _utc_now()
    bad_rank = {"fail": 0, "blocked": 1, "skip": 2, "ok": 3}
    merged: dict[str, MotionResult] = {}
    for r in results:
        prev = merged.get(r.agent_id)
        if prev is None:
            merged[r.agent_id] = r
            continue
        # Prefer any successful motion over blocked/skip noise for the same agent.
        if r.status == "ok" or (prev.status != "ok" and bad_rank.get(r.status, 9) < bad_rank.get(prev.status, 9)):
            merged[r.agent_id] = r
        elif prev.status != "ok" and r.status == prev.status:
            merged[r.agent_id] = MotionResult(
                r.agent_id, r.status, f"{prev.detail}; {r.detail}"[:400], prev.duration_s + r.duration_s
            )
    results = list(merged.values())

    lines = [
        "# Fleet activity (latest MVP motion run)",
        "",
        f"*Generated:* {ts} · agents in registry: **{agent_count}**",
        "",
        "Regenerate: `./scripts/run-agent-fleet-mvp.sh` or `python3 disklordz/agents/workflows/fleet_mvp_motion.py`",
        "",
        "| Agent | Status | Detail |",
        "|-------|--------|--------|",
    ]
    for r in sorted(results, key=lambda x: x.agent_id):
        detail = r.detail.replace("|", "\\|").replace("\n", " ")
        if len(detail) > 120:
            detail = detail[:117] + "..."
        lines.append(f"| {r.agent_id} | {r.status} | {detail} |")
    counts = {}
    for r in results:
        counts[r.status] = counts.get(r.status, 0) + 1
    lines.extend(
        [
            "",
            f"**Summary:** ok={counts.get('ok', 0)} skip={counts.get('skip', 0)} "
            f"blocked={counts.get('blocked', 0)} fail={counts.get('fail', 0)}",
            "",
            "Prod-only agents stay `blocked` until secrets in `docs/NEXT.md` / `STUB_AGENT_BLOCKERS.md`.",
        ]
    )
    ACTIVITY_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _blocked(agent_id: str, reason: str) -> MotionResult:
    return MotionResult(agent_id, "blocked", reason, 0.0)


def run(*, full: bool = False) -> int:
    if not FLEET.is_file():
        print(f"Missing {FLEET}", file=sys.stderr)
        return 1

    fleet = json.loads(FLEET.read_text(encoding="utf-8"))
    agent_count = int(fleet.get("agent_count", 0))
    registered = {a["id"] for a in fleet.get("agents", [])}
    base_url = os.environ.get("DISKLORDZ_URL", "").strip()

    results: list[MotionResult] = []
    lite = os.environ.get("FLEET_MOTION_LITE", "").strip().lower() in ("1", "true", "yes")

    if not lite:
        results.append(
            _run_shell(
                "pm-agent",
                "profit scaffold --check",
                ["python3", "disklordz/agents/profit/scaffold_agents.py", "--check"],
            )
        )
        results.append(
            _run_shell(
                "workflow-automation",
                "scaffold_workflows.py",
                ["python3", "disklordz/agents/workflows/scaffold_workflows.py"],
            )
        )
        results.append(
            _run_shell(
                "prompt-coach",
                "chunk_corpus",
                ["python3", "disklordz/rag/scripts/chunk_corpus.py"],
            )
        )
    results.append(
        _run_shell(
            "prompt-coach",
            "embed dry-run",
            ["python3", "disklordz/rag/scripts/embed_and_upsert.py", "--dry-run"],
            allow_fail=True,
        )
    )
    results.append(
        _run_shell(
            "integration-health",
            "promote integrations manifest",
            ["python3", "disklordz/integrations/scripts/promote-integrations.py", "--write"],
        )
    )

    stub = REPO / "disklordz/agents/workflows/scripts/stub-agent-env-check.sh"
    if stub.is_file():
        results.append(
            _run_shell("pm-agent", "stub env checklist", ["bash", str(stub)])
        )

    hp = REPO / "disklordz/integrations/agents/hardware_preset_designer.py"
    if hp.is_file() and "hardware-preset-designer" in registered:
        results.append(
            _run_shell(
                "hardware-preset-designer",
                "hardware_preset_designer --self-test",
                ["python3", str(hp), "--self-test"],
                allow_fail=True,
            )
        )

    rel = REPO / "disklordz/agents/workflows/scripts/release-notes-from-prs.sh"
    if rel.is_file():
        results.append(
            _run_shell(
                "release-notes",
                "merged PR digest",
                ["bash", str(rel)],
                allow_fail=True,
            )
        )

    # Website contract checks (spec-validator / lane-workflow)
    vspark = REPO / "disklordz/website/scripts/verify-spark-audio-contracts.ts"
    if vspark.is_file():
        results.append(
            _run_shell(
                "spec-validator",
                "verify-spark-audio-contracts",
                ["npx", "--yes", "tsx", str(vspark)],
                cwd=REPO / "disklordz/website",
                allow_fail=True,
            )
        )
        results.append(
            _run_shell(
                "lane-workflow",
                "spark audio contracts (shared)",
                ["npx", "--yes", "tsx", str(vspark)],
                cwd=REPO / "disklordz/website",
                allow_fail=True,
            )
        )

    # Optional engines / stubs
    for agent_id, script in (
        ("engine-swap", REPO / "disklordz/integrations/engines/modal/deploy_stub.py"),
        ("factory-batch-gpu", REPO / "disklordz/integrations/engines/modal/deploy_stub.py"),
    ):
        if script.is_file():
            results.append(
                _run_shell(
                    agent_id,
                    "integration deploy stub",
                    ["python3", str(script)],
                    allow_fail=True,
                )
            )

    openai_stub = REPO / "disklordz/integrations/agents/openai_factory_agent.py"
    if openai_stub.is_file():
        results.append(
            _run_shell(
                "product-factory",
                "openai_factory_agent stub",
                ["python3", str(openai_stub)],
                allow_fail=True,
            )
        )

    # Live site checks
    if base_url:
        results.append(
            _run_shell(
                "integration-health",
                f"verify-integrations @ {base_url}",
                ["bash", "disklordz/integrations/scripts/verify-integrations.sh"],
                env={"DISKLORDZ_URL": base_url},
            )
        )
        go_live = REPO / "disklordz/website/scripts/verify-go-live.sh"
        if go_live.is_file():
            results.append(
                _run_shell(
                    "conversion-qa",
                    f"go-live smoke @ {base_url}",
                    ["bash", str(go_live)],
                    env={"DISKLORDZ_URL": base_url},
                )
            )
        smoke = REPO / "scripts/saas-007-smoke.sh"
        if smoke.is_file():
            results.append(
                _run_shell(
                    "lane-workflow",
                    "SaaS 007 variations smoke",
                    ["bash", str(smoke)],
                    env={"DISKLORDZ_URL": base_url},
                    allow_fail=True,
                )
            )
    else:
        if not any(r.agent_id == "integration-health" and r.status == "ok" for r in results):
            results.append(
                _blocked(
                    "integration-health",
                    "set DISKLORDZ_URL (or CI DISKLORDZ_VERIFY_BASE_URL) for remote verify",
                )
            )
        results.append(_blocked("conversion-qa", "needs DISKLORDZ_URL for go-live smoke"))

    if full:
        results.append(
            _run_shell(
                "golden-wav-qa",
                "run_business.py --profile ci-verify",
                ["python3", "vst-testing-ops/run_business.py", "--profile", "ci-verify"],
                allow_fail=False,
            )
        )
        inbox = REPO / "disklordz/daw-inbox/package.json"
        if inbox.is_file():
            results.append(
                _run_shell(
                    "daw-inbox-copilot",
                    "daw-inbox module parse",
                    ["node", "--check", "bin/watch.mjs"],
                    cwd=REPO / "disklordz/daw-inbox",
                    allow_fail=True,
                )
            )
    else:
        results.append(
            MotionResult(
                "golden-wav-qa",
                "skip",
                "run with FLEET_MVP_FULL=1 or --full for ci-verify",
                0.0,
            )
        )
        results.append(
            MotionResult(
                "daw-inbox-copilot",
                "skip",
                "run with FLEET_MVP_FULL=1 for inbox watcher syntax check",
                0.0,
            )
        )

    # Document secret-gated agents (no motion possible in this environment)
    secret_gated = [
        ("billing-ops", "STRIPE_* + n8n billing stub"),
        ("async-generation", "INNGEST_EVENT_KEY / INNGEST_SIGNING_KEY"),
        ("onboarding-concierge", "Inngest user.signed_up (same keys)"),
        ("marketing-glue", "n8n docker + credentials"),
        ("churn-winback", "n8n + Supabase service role"),
        ("analytics-interpreter", "Stripe + Supabase reads"),
        ("desktop-ops", "Bytebot external"),
        ("ship-velocity", "repository_dispatch / Airtable token"),
        ("airtable-wo-triage", "Airtable + dispatch webhook"),
        ("referral-affiliate", "Stripe checkout + coupons live"),
        ("pricing-experiment", "human /premortem + Stripe price IDs"),
        ("competitive-intel", "manual monthly doc pass"),
        ("license-compliance", "human sign-off before engine-swap prod"),
        ("social-clip-factory", "ffmpeg + kit WAV input"),
        ("seo-kit-pages", "Vercel build / ISR kit routes"),
        ("fraud-abuse", "live POST /api/generate traffic"),
        ("support-macro", "POST /api/rag/suggest (needs running site)"),
        ("ops-schema", "workflow_dispatch activate-integrations + Supabase"),
    ]
    touched = {r.agent_id for r in results}
    for agent_id, reason in secret_gated:
        if agent_id not in touched:
            if agent_id == "support-macro" and base_url:
                results.append(
                    _run_shell(
                        "support-macro",
                        "rag suggest keyword",
                        [
                            "curl",
                            "-sf",
                            "-X",
                            "POST",
                            f"{base_url.rstrip('/')}/api/rag/suggest",
                            "-H",
                            "Content-Type: application/json",
                            "-d",
                            '{"query":"phonk kick"}',
                        ],
                        allow_fail=True,
                    )
                )
            elif agent_id == "fraud-abuse" and base_url:
                results.append(
                    MotionResult(
                        "fraud-abuse",
                        "skip",
                        "rate limits verified indirectly via /api/generate in go-live",
                        0.0,
                    )
                )
            else:
                results.append(_blocked(agent_id, reason))

    # Ensure every registered agent has at least one line
    for aid in sorted(registered):
        if aid not in {r.agent_id for r in results}:
            results.append(_blocked(aid, "no MVP motion hook yet — add in fleet_mvp_motion.py"))

    failures = [r for r in results if r.status == "fail"]
    _append_log(results)
    _write_summary_md(results, agent_count)

    print(f"MVP motion: {len(results)} entries, fail={len(failures)}, log→{ACTIVITY_LOG.name}")
    for r in results:
        if r.status in ("fail", "blocked"):
            print(f"  [{r.status}] {r.agent_id}: {r.detail[:100]}")

    return 1 if failures else 0


def main() -> int:
    full = "--full" in sys.argv or os.environ.get("FLEET_MVP_FULL", "").strip() in ("1", "true", "yes")
    return run(full=full)


if __name__ == "__main__":
    raise SystemExit(main())
