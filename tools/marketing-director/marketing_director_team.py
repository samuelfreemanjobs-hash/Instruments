#!/usr/bin/env python3
"""Marketing Director — team mode scaffold and validate (no API calls)."""

from __future__ import annotations

import argparse
import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
RUNS_ROOT = REPO_ROOT / "business-agents" / "marketing-director" / "runs"
BRAND_MEMORY = Path(__file__).resolve().parent / "brand_memory" / "instruments_disklordz.json"
SPECIALISTS = (
    "research_agent",
    "creative_agent",
    "copy_agent",
    "media_agent",
    "analytics_agent",
    "compliance_agent",
)

sys.path.insert(0, str(REPO_ROOT / "scripts" / "business-agents"))
from lib import schema_validate  # noqa: E402


def slugify(text: str) -> str:
    s = re.sub(r"[^a-z0-9]+", "-", text.lower()).strip("-")
    return (s[:64] or "run").rstrip("-")


def plan_run(request: str, slug: str | None = None, force: bool = False) -> Path:
    """Create run folder with director task and specialist output slots."""
    run_slug = slug or slugify(request[:80])
    dest = RUNS_ROOT / run_slug
    if dest.exists() and not force:
        raise FileExistsError(f"Run exists: {dest} (use --force)")
    dest.mkdir(parents=True, exist_ok=True)
    (dest / "specialists").mkdir(exist_ok=True)

    brand = {}
    if BRAND_MEMORY.is_file():
        brand = json.loads(BRAND_MEMORY.read_text(encoding="utf-8"))
        (dest / "brand_memory_snapshot.json").write_text(
            json.dumps(brand, indent=2), encoding="utf-8"
        )

    manifest = {
        "schema_version": "1",
        "slug": run_slug,
        "created_at": datetime.now(timezone.utc).isoformat(),
        "status": "planned",
        "user_request": request,
        "brand_memory_path": str(BRAND_MEMORY.relative_to(REPO_ROOT)),
        "specialists_expected": [],
        "external_facing": None,
        "compliance_required": True,
        "escalated": False,
        "human_review_ids": [],
    }
    (dest / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    (dest / "REQUEST.md").write_text(
        f"# Marketing Director run `{run_slug}`\n\n## User request\n\n{request}\n",
        encoding="utf-8",
    )

    director_task = f"""## Goal
Act as **marketing-director** (supervisory orchestrator). **No repo API keys.**

## Read
- docs/business-agents/prompts/marketing-director-system.md
- tools/marketing-director/brand_memory/instruments_disklordz.json (or run snapshot)
- docs/business-agents/TEAM_ROLES.md
- .claude/agents/marketing-director/AGENT.md

## User request
{request}

## Plan
1. CLASSIFY the request; write a brief plan inside `<plan></plan>` in `director-notes.md`.
2. Call **brand_memory_read** conceptually — use `brand_memory_snapshot.json` before delegating.
3. Delegate only needed specialists; each writes JSON under `specialists/<name>.json` using prompts in `docs/business-agents/prompts/specialists/`.
4. **Never skip compliance_agent** for external-facing copy; HIGH/CRITICAL blocks publish.
5. Synthesize `deliverable.md` (deliverable, rationale, decisions, next steps).
6. Update `manifest.json`: `status`, `specialists_expected`, `external_facing`, `escalated`.
7. Run: `python3 scripts/business-agents/business_agent.py director validate --run {dest.relative_to(REPO_ROOT)}`

## Escalate (document in escalation.json)
- Budget > $25,000 without authorization
- Compliance HIGH/CRITICAL
- Same specialist retried >3 times without convergence
"""
    (dest / "DIRECTOR_TASK.md").write_text(director_task, encoding="utf-8")
    (dest / "director-notes.md").write_text(
        "<!-- Director: plan in <plan></plan>, delegation log below -->\n\n",
        encoding="utf-8",
    )
    (dest / "deliverable.md").write_text(
        "<!-- Final synthesis: deliverable, rationale, decisions, next steps -->\n\n",
        encoding="utf-8",
    )

    for name in SPECIALISTS:
        readme = dest / "specialists" / f"{name}.README.md"
        readme.write_text(
            f"# `{name}`\n\n"
            f"Teammate writes `{name}.json` per "
            f"`docs/business-agents/prompts/specialists/{name}.md`.\n"
            f"Delete this README when JSON is committed.\n",
            encoding="utf-8",
        )

    readme = dest / "README.md"
    readme.write_text(
        f"# Run `{run_slug}`\n\n"
        f"1. Assign Cloud Agent using `DIRECTOR_TASK.md`\n"
        f"2. Specialists fill `specialists/*.json`\n"
        f"3. Director completes `deliverable.md` + `manifest.json`\n"
        f"4. Validate: `python3 scripts/business-agents/business_agent.py director validate "
        f"--run {dest.relative_to(REPO_ROOT)}`\n",
        encoding="utf-8",
    )
    return dest


def validate_run(run_path: Path) -> list[str]:
    errors: list[str] = []
    if not run_path.is_dir():
        return [f"not a directory: {run_path}"]

    manifest_path = run_path / "manifest.json"
    if not manifest_path.is_file():
        errors.append("missing manifest.json")
        return errors

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        return [f"manifest.json invalid: {e}"]

    errors.extend(schema_validate.validate_director_manifest(manifest))

    specialists_dir = run_path / "specialists"
    present: list[str] = []
    for p in specialists_dir.glob("*.json"):
        name = p.stem
        present.append(name)
        try:
            data = json.loads(p.read_text(encoding="utf-8"))
        except json.JSONDecodeError as e:
            errors.append(f"{p.name}: invalid JSON: {e}")
            continue
        if name == "compliance_agent":
            errors.extend(
                schema_validate.validate_compliance_verdict(data, prefix=f"{p.name}: ")
            )
        else:
            errors.extend(
                schema_validate.validate_specialist_output(data, prefix=f"{p.name}: ")
            )

    expected = manifest.get("specialists_expected") or []
    if isinstance(expected, list) and expected:
        for name in expected:
            if name not in present:
                errors.append(f"missing specialist output: specialists/{name}.json")

    external = manifest.get("external_facing")
    compliance_required = manifest.get("compliance_required", True)
    if external is True or (
        compliance_required and external is not False and manifest.get("status") == "complete"
    ):
        comp = run_path / "specialists" / "compliance_agent.json"
        if not comp.is_file():
            errors.append("external-facing run requires specialists/compliance_agent.json")
        elif comp.is_file():
            try:
                verdict = json.loads(comp.read_text(encoding="utf-8"))
                sev = verdict.get("severity", "HIGH")
                if sev in ("HIGH", "CRITICAL"):
                    errors.append(
                        f"compliance blocks publish: severity={sev} "
                        "(fix content or escalate; do not ship)"
                    )
            except json.JSONDecodeError:
                pass

    if manifest.get("status") == "complete":
        deliverable = run_path / "deliverable.md"
        if not deliverable.is_file() or len(deliverable.read_text(encoding="utf-8").strip()) < 80:
            errors.append("status=complete requires deliverable.md (min ~80 chars)")

    return errors


def cmd_plan(args: argparse.Namespace) -> int:
    try:
        dest = plan_run(args.request, slug=args.slug, force=args.force)
    except FileExistsError as e:
        print(str(e), file=sys.stderr)
        return 1
    print(dest.relative_to(REPO_ROOT))
    return 0


def cmd_validate(args: argparse.Namespace) -> int:
    run_path = Path(args.run)
    if not run_path.is_absolute():
        run_path = REPO_ROOT / run_path
    errs = validate_run(run_path)
    if errs:
        for e in errs:
            print(e, file=sys.stderr)
        return 1
    print("OK: marketing director run")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Marketing Director team scaffold/validate")
    sub = parser.add_subparsers(dest="command", required=True)
    p = sub.add_parser("plan", help="Scaffold run folder")
    p.add_argument("--request", required=True)
    p.add_argument("--slug", help="Folder name under runs/")
    p.add_argument("--force", action="store_true")
    p.set_defaults(func=cmd_plan)
    v = sub.add_parser("validate", help="Validate run artifacts")
    v.add_argument("--run", required=True, help="Path to run folder")
    v.set_defaults(func=cmd_validate)
    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
