#!/usr/bin/env python3
"""Business agents CLI — SKU research, marketing content, ship evaluation."""

from __future__ import annotations

import argparse
import json
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BRIEFS = REPO_ROOT / "business-agents" / "sku-briefs"
CONTENT = REPO_ROOT / "business-agents" / "content-drafts"
EVAL = REPO_ROOT / "business-agents" / "ship-evaluations"
TEMPLATE = REPO_ROOT / "docs" / "business-agents" / "intake" / "sku-intake.template.yaml"

sys.path.insert(0, str(Path(__file__).resolve().parent))
from lib import llm_optional, prompts, schema_validate  # noqa: E402


def _load_json_file(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _write_json(path: Path, data: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


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
        f"# SKU brief `{pid}`\n\n1. Edit `intake.yaml`\n"
        f"2. `python3 scripts/business-agents/business_agent.py sku prompt --intake {intake}`\n"
        f"3. Save model output as `brief.json`\n"
        f"4. `python3 scripts/business-agents/business_agent.py sku validate --file {dest}/brief.json`\n",
        encoding="utf-8",
    )
    print(dest)
    return 0


def cmd_sku_prompt(args: argparse.Namespace) -> int:
    intake_path = Path(args.intake)
    intake = prompts.parse_intake_text(intake_path)
    user = json.dumps(intake, indent=2)
    if args.run:
        messages = prompts.build_messages("sku-research-system.md", user)
        data = llm_optional.complete_json(messages)
        data.setdefault("generated_at", datetime.now(timezone.utc).isoformat())
        out = Path(args.output) if args.output else intake_path.parent / "brief.json"
        _write_json(out, data)
        errs = schema_validate.validate_sku_brief(data)
        if errs:
            print("Validation warnings:", errs, file=sys.stderr)
        print(out)
        return 0 if not errs else 2
    print(prompts.format_prompt_bundle("sku", "sku-research-system.md", user))
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
    if args.run:
        messages = prompts.build_messages("content-marketing-system.md", user)
        raw = llm_optional.complete_json(messages)
        # Content agent may return draft-shaped JSON
        if "body_markdown" in raw:
            data = raw
        else:
            data = raw
        data.setdefault("schema_version", "1")
        data.setdefault("generated_at", datetime.now(timezone.utc).isoformat())
        slug = args.product.replace("/", "-")[:40]
        out = Path(args.output) if args.output else CONTENT / f"{slug}-{args.type}.json"
        _write_json(out, data)
        errs = schema_validate.validate_content_draft(data)
        if errs:
            print("Validation warnings:", errs, file=sys.stderr)
        print(out)
        return 0 if not errs else 2
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
    if args.run:
        messages = prompts.build_messages("ship-evaluator-system.md", user)
        data = llm_optional.complete_json(messages)
        data.setdefault("schema_version", "1")
        data.setdefault("generated_at", datetime.now(timezone.utc).isoformat())
        data.setdefault("evaluator", "business-agent-cli")
        slug = artifact.stem[:30]
        out = Path(args.output) if args.output else EVAL / f"{slug}-eval.json"
        _write_json(out, data)
        errs = schema_validate.validate_ship_evaluation(data)
        if errs:
            print("Validation warnings:", errs, file=sys.stderr)
        print(out)
        return 0 if not errs else 2
    print(prompts.format_prompt_bundle("evaluate", "ship-evaluator-system.md", user))
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


def cmd_check_pr(args: argparse.Namespace) -> int:
    """Validate all brief/eval JSON under business-agents/. Used in CI."""
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
        if "planner_recommendation" in data:
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
    parser = argparse.ArgumentParser(description="Instruments business agents")
    sub = parser.add_subparsers(dest="command", required=True)

    sku = sub.add_parser("sku", help="SKU research (Planner)")
    sku_sub = sku.add_subparsers(dest="sku_cmd", required=True)
    p_init = sku_sub.add_parser("init", help="Scaffold brief folder")
    p_init.add_argument("--product-id", required=True)
    p_init.add_argument("--name", required=True)
    p_init.add_argument("--force", action="store_true")
    p_init.set_defaults(func=cmd_sku_init)

    p_prompt = sku_sub.add_parser("prompt", help="Print or run research prompt")
    p_prompt.add_argument("--intake", required=True)
    p_prompt.add_argument("--run", action="store_true", help="Call LLM if API key set")
    p_prompt.add_argument("--output", help="brief.json path")
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
    cp.add_argument("--run", action="store_true")
    cp.add_argument("--output")
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
    ep.add_argument("--run", action="store_true")
    ep.add_argument("--output")
    ep.set_defaults(func=cmd_evaluate_prompt)

    evv = e_sub.add_parser("validate")
    evv.add_argument("--file", required=True)
    evv.set_defaults(func=cmd_evaluate_validate)

    chk = sub.add_parser("check-pr", help="Validate business-agents JSON in CI")
    chk.set_defaults(func=cmd_check_pr)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
