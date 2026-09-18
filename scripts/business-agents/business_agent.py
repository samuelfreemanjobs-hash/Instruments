#!/usr/bin/env python3
"""Business team CLI — scaffold, validate, and task handoffs (no API calls)."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BRIEFS = REPO_ROOT / "business-agents" / "sku-briefs"
CONTENT = REPO_ROOT / "business-agents" / "content-drafts"
EVAL = REPO_ROOT / "business-agents" / "ship-evaluations"
DIRECTOR_RUNS = REPO_ROOT / "business-agents" / "marketing-director" / "runs"
MD_TEAM = REPO_ROOT / "tools" / "marketing-director" / "marketing_director_team.py"
TASKS = REPO_ROOT / "business-agents" / "tasks"
TEMPLATE = REPO_ROOT / "docs" / "business-agents" / "intake" / "sku-intake.template.yaml"

sys.path.insert(0, str(Path(__file__).resolve().parent))
from lib import prompts, schema_validate  # noqa: E402

ROLE_META = {
    "sku-research": {
        "title": "business-sku-research",
        "prompt": "sku-research-system.md",
        "claude": ".claude/agents/business-sku-research/AGENT.md",
        "validate": "sku validate --file business-agents/sku-briefs/<id>/brief.json",
    },
    "content": {
        "title": "business-content",
        "prompt": "content-marketing-system.md",
        "claude": ".claude/agents/business-content/AGENT.md",
        "validate": "content validate --file business-agents/content-drafts/<file>.json",
    },
    "ship-eval": {
        "title": "business-ship-eval",
        "prompt": "ship-evaluator-system.md",
        "claude": ".claude/agents/business-ship-eval/AGENT.md",
        "validate": "evaluate validate --file business-agents/ship-evaluations/<file>-eval.json",
    },
    "marketing-director": {
        "title": "marketing-director",
        "prompt": "marketing-director-system.md",
        "claude": ".claude/agents/marketing-director/AGENT.md",
        "validate": "director validate --run business-agents/marketing-director/runs/<slug>",
    },
}


def _load_json_file(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def cmd_sku_init(args: argparse.Namespace) -> int:
    pid = args.product_id.strip()
    dest = BRIEFS / pid
    if dest.exists() and not args.force:
        print(f"Exists: {dest} (use --force)", file=sys.stderr)
        return 1
    dest.mkdir(parents=True, exist_ok=True)
    intake = dest / "intake.yaml"
    if TEMPLATE.is_file():
        text = TEMPLATE.read_text(encoding="utf-8")
        text = text.replace("DL-SKU-EXAMPLE", pid).replace("Example Product", args.name)
        intake.write_text(text, encoding="utf-8")
    readme = dest / "README.md"
    readme.write_text(
        f"# SKU brief `{pid}`\n\n"
        f"1. Edit `intake.yaml`\n"
        f"2. Assign **business-sku-research** teammate (Cursor Cloud / Claude) — see `docs/business-agents/TEAM_ROLES.md`\n"
        f"3. Teammate writes `brief.json` and runs validate\n"
        f"4. `python3 scripts/business-agents/business_agent.py sku validate --file {dest}/brief.json`\n",
        encoding="utf-8",
    )
    print(dest)
    return 0


def cmd_sku_prompt(args: argparse.Namespace) -> int:
    intake_path = Path(args.intake)
    intake = prompts.parse_intake_text(intake_path)
    user = json.dumps(intake, indent=2)
    print(prompts.format_prompt_bundle("sku-research", "sku-research-system.md", user))
    return 0


def cmd_sku_validate(args: argparse.Namespace) -> int:
    data = _load_json_file(Path(args.file))
    errs = schema_validate.validate_sku_brief(data)
    if errs:
        for e in errs:
            print(e, file=sys.stderr)
        return 1
    print("OK: sku research brief")
    return 0


def cmd_content_prompt(args: argparse.Namespace) -> int:
    intake = {
        "content_type": args.type,
        "sku_or_product": args.product,
        "topic": args.topic,
        "target_audience": args.audience,
        "keywords": args.keywords.split(",") if args.keywords else [],
        "spec_excerpt": Path(args.spec).read_text(encoding="utf-8")[:12000]
        if args.spec
        else "",
    }
    user = json.dumps(intake, indent=2)
    print(prompts.format_prompt_bundle("content", "content-marketing-system.md", user))
    return 0


def cmd_content_validate(args: argparse.Namespace) -> int:
    data = _load_json_file(Path(args.file))
    errs = schema_validate.validate_content_draft(data)
    if errs:
        for e in errs:
            print(e, file=sys.stderr)
        return 1
    print("OK: content draft")
    return 0


def cmd_evaluate_prompt(args: argparse.Namespace) -> int:
    artifact = Path(args.artifact)
    if not artifact.is_file():
        print(f"Missing artifact: {artifact}", file=sys.stderr)
        return 1
    text = artifact.read_text(encoding="utf-8")
    if len(text) > 20000:
        text = text[:20000] + "\n\n[truncated for evaluator]"
    intake = {
        "artifact_type": args.type,
        "artifact_paths": [str(artifact.relative_to(REPO_ROOT))],
        "original_request": args.request,
        "target_audience": args.audience,
        "retail_candidate": args.retail,
        "artifact_content": text,
    }
    user = json.dumps(intake, indent=2)
    print(prompts.format_prompt_bundle("ship-eval", "ship-evaluator-system.md", user))
    return 0


def cmd_evaluate_validate(args: argparse.Namespace) -> int:
    data = _load_json_file(Path(args.file))
    errs = schema_validate.validate_ship_evaluation(data)
    if errs:
        for e in errs:
            print(e, file=sys.stderr)
        return 1
    if data.get("quality_assessment") == "ACCEPT" and not data.get("ready_for_approval"):
        print("WARN: ACCEPT but ready_for_approval is false", file=sys.stderr)
    print("OK: ship evaluation")
    return 0


def cmd_director_plan(args: argparse.Namespace) -> int:
    import subprocess

    cmd = [
        sys.executable,
        str(MD_TEAM),
        "plan",
        "--request",
        args.request,
    ]
    if args.slug:
        cmd.extend(["--slug", args.slug])
    if args.force:
        cmd.append("--force")
    return subprocess.call(cmd, cwd=str(REPO_ROOT))


def cmd_director_validate(args: argparse.Namespace) -> int:
    import subprocess

    cmd = [sys.executable, str(MD_TEAM), "validate", "--run", args.run]
    return subprocess.call(cmd, cwd=str(REPO_ROOT))


def cmd_team_task(args: argparse.Namespace) -> int:
    meta = ROLE_META.get(args.role)
    if not meta:
        print(f"Unknown role: {args.role}", file=sys.stderr)
        return 1
    body = f"""## Goal
Execute business teammate role **{meta['title']}** (no repo API keys).

## Context
- Read: docs/business-agents/prompts/{meta['prompt']}
- Read: docs/business-agents/TEAM_ROLES.md
- Claude: {meta['claude']}
- Rule: .cursor/rules/business-agents-team.mdc

## User request
{args.request}

## Deliverable
- Write JSON artifact under `business-agents/` (see TEAM_ROLES.md)
- Run: `python3 scripts/business-agents/business_agent.py {meta['validate'].split()[0]} validate ...`
- Commit + push; draft PR with validation output in description

## Out of scope
- ANTHROPIC_API_KEY / OPENAI_API_KEY in scripts or CI for this role
- Publish to production or set human_approved without owner
"""
    if args.out:
        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(body, encoding="utf-8")
        print(out)
    else:
        print(body)
    return 0


def cmd_check_pr(args: argparse.Namespace) -> int:
    errors = 0
    for path in (REPO_ROOT / "business-agents").rglob("*.json"):
        if "node_modules" in path.parts:
            continue
        try:
            data = _load_json_file(path)
        except json.JSONDecodeError as e:
            print(f"{path}: invalid JSON: {e}", file=sys.stderr)
            errors += 1
            continue
        if path.name == "manifest.json" and "marketing-director" in path.parts:
            errs = schema_validate.validate_director_manifest(data)
            label = "director manifest"
        elif path.parent.name == "specialists" and "marketing-director" in path.parts:
            if path.stem == "compliance_agent":
                errs = schema_validate.validate_compliance_verdict(data)
                label = "compliance verdict"
            else:
                errs = schema_validate.validate_specialist_output(data)
                label = "specialist output"
        elif "planner_recommendation" in data:
            errs = schema_validate.validate_sku_brief(data)
            label = "sku brief"
        elif "quality_assessment" in data:
            errs = schema_validate.validate_ship_evaluation(data)
            label = "ship eval"
        elif "body_markdown" in data and "content_type" in data:
            errs = schema_validate.validate_content_draft(data)
            label = "content draft"
        else:
            continue
        if errs:
            errors += 1
            print(f"{path} ({label}):", ", ".join(errs), file=sys.stderr)
    if errors:
        return 1
    print("OK: business-agents JSON artifacts")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Business team — scaffold/validate only; teammates are Cursor/Claude"
    )
    sub = parser.add_subparsers(dest="command", required=True)

    team = sub.add_parser("team", help="Print Cloud Agent task for a business role")
    team.add_argument(
        "--role",
        required=True,
        choices=list(ROLE_META.keys()),
    )
    team.add_argument("--request", required=True, help="What the teammate should do")
    team.add_argument("--out", help="Write task markdown to path")
    team.set_defaults(func=cmd_team_task)

    sku = sub.add_parser("sku", help="SKU research (Planner)")
    sku_sub = sku.add_subparsers(dest="sku_cmd", required=True)
    p_init = sku_sub.add_parser("init", help="Scaffold brief folder")
    p_init.add_argument("--product-id", required=True)
    p_init.add_argument("--name", required=True)
    p_init.add_argument("--force", action="store_true")
    p_init.set_defaults(func=cmd_sku_init)

    p_prompt = sku_sub.add_parser("prompt", help="Print context bundle for teammate")
    p_prompt.add_argument("--intake", required=True)
    p_prompt.set_defaults(func=cmd_sku_prompt)

    p_val = sku_sub.add_parser("validate", help="Validate brief.json")
    p_val.add_argument("--file", required=True)
    p_val.set_defaults(func=cmd_sku_validate)

    content = sub.add_parser("content", help="Marketing content drafts")
    c_sub = content.add_subparsers(dest="content_cmd", required=True)
    cp = c_sub.add_parser("prompt")
    cp.add_argument("--type", required=True, choices=[
        "landing_hero", "store_listing", "social_thread", "email", "release_notes",
    ])
    cp.add_argument("--product", required=True)
    cp.add_argument("--topic", required=True)
    cp.add_argument("--audience", default="beat makers and producers")
    cp.add_argument("--keywords", default="")
    cp.add_argument("--spec", help="Path to PRODUCT_SPEC excerpt")
    cp.set_defaults(func=cmd_content_prompt)

    cv = c_sub.add_parser("validate")
    cv.add_argument("--file", required=True)
    cv.set_defaults(func=cmd_content_validate)

    ev = sub.add_parser("evaluate", help="Ship / customer-facing evaluator")
    e_sub = ev.add_subparsers(dest="eval_cmd", required=True)
    ep = e_sub.add_parser("prompt")
    ep.add_argument("--artifact", required=True)
    ep.add_argument("--type", required=True, choices=[
        "store_listing", "landing_copy", "product_spec", "preset_pack_readme",
        "saas_marketing", "release_ready_bundle",
    ])
    ep.add_argument("--request", default="Review for customer-facing accuracy before publish")
    ep.add_argument("--audience", default="music producers")
    ep.add_argument("--retail", action="store_true")
    ep.set_defaults(func=cmd_evaluate_prompt)

    evv = e_sub.add_parser("validate")
    evv.add_argument("--file", required=True)
    evv.set_defaults(func=cmd_evaluate_validate)

    director = sub.add_parser("director", help="Marketing Director runs (team mode)")
    d_sub = director.add_subparsers(dest="director_cmd", required=True)
    dp = d_sub.add_parser("plan", help="Scaffold run under business-agents/marketing-director/runs/")
    dp.add_argument("--request", required=True)
    dp.add_argument("--slug", help="Folder name")
    dp.add_argument("--force", action="store_true")
    dp.set_defaults(func=cmd_director_plan)
    dv = d_sub.add_parser("validate", help="Validate run folder")
    dv.add_argument("--run", required=True)
    dv.set_defaults(func=cmd_director_validate)

    chk = sub.add_parser("check-pr", help="Validate business-agents JSON in CI")
    chk.set_defaults(func=cmd_check_pr)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
