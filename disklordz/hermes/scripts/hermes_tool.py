#!/usr/bin/env python3
"""Hermes seat toolkit — ops, devops, handoff, gtm, presets, support, security, data."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

HERMES_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = HERMES_ROOT.parents[1]
OUTBOX = HERMES_ROOT / "outbox"
AGENT_REPOS = HERMES_ROOT / "agent-repos"
OPS_ROOT = REPO_ROOT / "disklordz" / "ops"
SOPS_ROOT = OPS_ROOT / "sops"
SOP_INDEX = SOPS_ROOT / "INDEX.json"
ANTIGRAVITY_INBOX = REPO_ROOT / "disklordz" / "antigravity" / "inbox"

AGENT_SEATS = (
    "hermes-lead",
    "hermes-architect",
    "hermes-dsp",
    "hermes-gui",
    "hermes-web",
    "hermes-qa",
    "hermes-research",
    "hermes-devops",
    "hermes-handoff",
    "hermes-ops",
    "hermes-gtm",
    "hermes-presets",
    "hermes-support",
    "hermes-security",
    "hermes-data",
    "hermes-sop",
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def cmd_ops_checklist(_: argparse.Namespace) -> int:
    tpl = HERMES_ROOT / "templates" / "ops-dispatch-checklist.md"
    print(_read(tpl))
    print("\n# Seed commands (human runs with credentials)\n")
    print("python3 disklordz/automation/scripts/seed_work_orders.py  # dry-run first")
    print("python3 disklordz/automation/scripts/seed_airtable_bundle.py --dry-run")
    return 0


def cmd_ops_validate_pr(args: argparse.Namespace) -> int:
    title = args.title or ""
    pattern = re.compile(r"WO-(?:SAAS-\d{3}|2026-\d{3})", re.I)
    ok = bool(pattern.search(title))
    print(f"PR title: {title!r}")
    print(f"WO id in title: {'YES' if ok else 'NO — add WO-2026-NNN or WO-SAAS-NNN'}")
    return 0 if ok or args.allow_missing else 1


def cmd_devops_summary(_: argparse.Namespace) -> int:
    wf = REPO_ROOT / ".github" / "workflows"
    print("# Hermes devops — CI surface\n")
    if wf.is_dir():
        for p in sorted(wf.glob("*.yml")):
            print(f"- `{p.relative_to(REPO_ROOT)}`")
    print("\n## Local parity\n")
    print("```bash")
    print("cmake --build build -j")
    print("python3 vst-testing-ops/run_business.py --profile ci-verify")
    print("cd disklordz/website && npm ci && npm run build")
    print("```")
    if shutil_which("gh"):
        print("\n## gh PR checks (when on a PR branch)\n")
        print("```bash")
        print("gh pr checks")
        print("gh run list --limit 5")
        print("```")
    else:
        print("\n(gh not installed — skip live PR checks)")
    return 0


def shutil_which(cmd: str) -> bool:
    from shutil import which

    return which(cmd) is not None


def cmd_handoff_draft(args: argparse.Namespace) -> int:
    day = datetime.now(timezone.utc).strftime("%Y%m%d")
    hid = f"HO-{day}-HERM"
    doc = {
        "handoff_id": hid,
        "direction": "cursor_to_antigravity",
        "from_agent": "hermes-handoff",
        "to_agent": "antigravity-hise",
        "created_at": datetime.now(timezone.utc).isoformat(),
        "work_order": {
            "id": args.wo,
            "title": args.title,
            "acceptance_criteria": args.criteria or "",
            "branch": args.branch,
        },
        "context_paths": args.path or ["docs/HISE_ANTIGRAVITY_LANE.md"],
        "artifacts": [],
        "status": "open",
        "notes": args.notes or "Created by hermes_tool handoff draft — review before push.",
    }
    OUTBOX.mkdir(parents=True, exist_ok=True)
    out = OUTBOX / f"{hid}.json"
    out.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")
    print(f"Draft handoff: {out.relative_to(REPO_ROOT)}")
    print("Prefer production send:")
    print(
        f"  ./scripts/antigravity-bridge/antigravity-bridge.sh send --wo {args.wo} "
        f"--title {json.dumps(args.title)} --push"
    )
    return 0


def cmd_gtm_brief(args: argparse.Namespace) -> int:
    products = {
        "junova": {
            "positioning": "Juno-class poly synth — Celestial UI, VST3+CLAP, $29→$49 ladder.",
            "early": "$29",
            "standard": "$49",
            "cross": "Disklordz sample lanes + SaaS kits; landing `junova-x-landing`.",
        },
        "saas": {
            "positioning": "Text/spec → parametric drum kits; ILLUGEN-shaped orchestration.",
            "early": "Free tier + credits",
            "standard": "Pro subscription",
            "cross": "Plugin demos → SaaS signup; A&R lane presets.",
        },
        "novadrum": {
            "positioning": "Circuit-class 808 (NovaDrum) — not sample ROMs.",
            "early": "TBD",
            "standard": "TBD",
            "cross": "Junova-X brand trust.",
        },
    }
    p = products.get(args.product, products["junova"])
    tpl = _read(HERMES_ROOT / "templates" / "gtm-brief.md")
    ts = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    body = (
        tpl.replace("{{PRODUCT}}", args.product)
        .replace("{{TIMESTAMP}}", ts)
        .replace("{{POSITIONING}}", p["positioning"])
        .replace("{{PRICE_EARLY}}", p["early"])
        .replace("{{PRICE_STANDARD}}", p["standard"])
        .replace("{{CROSS_SELL}}", p["cross"])
    )
    if args.write:
        OUTBOX.mkdir(parents=True, exist_ok=True)
        slug = args.product
        path = OUTBOX / f"GTM-{slug}-{datetime.now(timezone.utc).strftime('%Y%m%d')}.md"
        path.write_text(body, encoding="utf-8")
        print(f"wrote {path.relative_to(REPO_ROOT)}", file=sys.stderr)
    print(body)
    return 0


def cmd_presets_audit(args: argparse.Namespace) -> int:
    targets = {
        "junova": REPO_ROOT / "Junova-X",
        "wave909": REPO_ROOT / "Wave909",
        "jd": REPO_ROOT / "Source",
    }
    root = targets.get(args.product, REPO_ROOT / args.product)
    try:
        rel = root.relative_to(REPO_ROOT)
    except ValueError:
        rel = root
    print(f"# Presets audit — {rel}\n")
    if not root.is_dir():
        print("Path missing.")
        return 1
    patterns = list(root.rglob("*.json")) + list(root.rglob("*Preset*")) + list(root.rglob("presets/*"))
    files = sorted({p for p in patterns if p.is_file()})[:50]
    print(f"Found {len(files)} candidate preset-related files (cap 50 shown).")
    for f in files:
        print(f"- `{f.relative_to(REPO_ROOT)}`")
    mvp = 48 if args.product == "junova" else 0
    if mvp:
        print(f"\nJunova MVP target: **{mvp}** factory presets (WO-2026-003).")
    return 0


def cmd_support_rag_status(_: argparse.Namespace) -> int:
    chunks = REPO_ROOT / "disklordz" / "rag" / "data" / "chunks.jsonl"
    manifest = REPO_ROOT / "disklordz" / "rag" / "corpus" / "manifest.json"
    print("# Support / RAG status\n")
    print(f"- manifest: `{manifest.relative_to(REPO_ROOT)}` — {'OK' if manifest.is_file() else 'MISSING'}")
    if chunks.is_file():
        n = sum(1 for _ in chunks.open(encoding="utf-8"))
        print(f"- chunks: `{chunks.relative_to(REPO_ROOT)}` — **{n}** lines")
    else:
        print("- chunks: MISSING — run `python3 disklordz/rag/scripts/chunk_corpus.py`")
    print("\nQuery demo:")
    print('  python3 disklordz/rag/scripts/query_local.py "lane DL006 phonk"')
    return 0


SECRET_PATTERNS = [
    re.compile(r"sk_live_[a-zA-Z0-9]+"),
    re.compile(r"sk_test_[a-zA-Z0-9]+"),
    re.compile(r"SUPABASE_SERVICE_ROLE_KEY\s*=\s*['\"]?[a-zA-Z0-9._-]{20,}"),
    re.compile(r"BEGIN (?:RSA )?PRIVATE KEY"),
    re.compile(r"pat[a-zA-Z0-9]{10,}"),  # Airtable PAT-ish
]


def cmd_security_scan(args: argparse.Namespace) -> int:
    print("# Hermes security scan (heuristic)\n")
    if args.staged:
        proc = subprocess.run(
            ["git", "diff", "--cached", "--name-only"],
            capture_output=True,
            text=True,
            cwd=REPO_ROOT,
        )
        files = [REPO_ROOT / ln for ln in proc.stdout.splitlines() if ln.strip()]
    else:
        files = [
            p
            for p in REPO_ROOT.rglob("*")
            if p.is_file()
            and ".git" not in p.parts
            and "node_modules" not in p.parts
            and "build" not in p.parts
            and p.suffix in {".ts", ".tsx", ".js", ".py", ".md", ".json", ".env", ".yml"}
        ][:500]
    hits = 0
    for path in files:
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        if ".env.example" in str(path):
            continue
        for pat in SECRET_PATTERNS:
            if pat.search(text):
                print(f"ALERT `{path.relative_to(REPO_ROOT)}` matched {pat.pattern}")
                hits += 1
    if hits == 0:
        print("No heuristic secret patterns in scanned files.")
    else:
        print(f"\n**{hits}** alert(s) — rotate/revert before merge.")
    return 1 if hits else 0


def cmd_agent_init(_: argparse.Namespace) -> int:
    script = HERMES_ROOT / "scripts" / "scaffold_agent_repos.py"
    proc = subprocess.run([sys.executable, str(script)], cwd=str(REPO_ROOT))
    return proc.returncode


def cmd_agent_status(_: argparse.Namespace) -> int:
    print("# Hermes agent repos\n")
    print(f"Index: `disklordz/hermes/agent-repos/README.md`\n")
    for seat in AGENT_SEATS:
        root = AGENT_REPOS / seat
        ok = root.is_dir() and (root / "README.md").is_file()
        runs = list((root / "runs").glob("*.md")) if ok else []
        props = list((root / "proposals").glob("*.md")) if ok else []
        playbook_lines = 0
        if ok and (root / "PLAYBOOK.local.md").is_file():
            playbook_lines = sum(
                1
                for ln in (root / "PLAYBOOK.local.md").read_text(encoding="utf-8").splitlines()
                if ln.startswith("### ")
            )
        status = "OK" if ok else "MISSING — run: hermes_tool.py agent init"
        print(
            f"- **{seat}** — {status} · playbook entries: {playbook_lines} · "
            f"runs: {len(runs)} · proposals: {len(props)}"
        )
    return 0


def cmd_agent_record_run(args: argparse.Namespace) -> int:
    if args.seat not in AGENT_SEATS:
        print(f"Unknown seat {args.seat!r}", file=sys.stderr)
        return 1
    root = AGENT_REPOS / args.seat
    if not root.is_dir():
        print("Agent repo missing; run: python3 disklordz/hermes/scripts/hermes_tool.py agent init", file=sys.stderr)
        return 1
    runs = root / "runs"
    runs.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now(timezone.utc).strftime("%Y%m%d-%H%M")
    slug = re.sub(r"[^a-z0-9]+", "-", (args.wo or "run").lower())[:32].strip("-")
    path = runs / f"{stamp}-{slug}.md"
    body = (
        f"# Run — {args.seat}\n\n"
        f"- **UTC:** {datetime.now(timezone.utc).isoformat()}\n"
        f"- **WO:** {args.wo or 'n/a'}\n"
        f"- **Branch:** {args.branch or 'n/a'}\n\n"
        f"## Summary\n\n{args.summary}\n\n"
        f"## Evidence\n\n{args.evidence or '(none logged)'}\n\n"
        f"## Learnings (promote to PLAYBOOK.local.md?)\n\n{args.learnings or '(none)'}\n"
    )
    path.write_text(body, encoding="utf-8")
    print(path.relative_to(REPO_ROOT))
    return 0


def _load_sop_index() -> dict:
    if not SOP_INDEX.is_file():
        return {"version": 0, "sops": []}
    return json.loads(SOP_INDEX.read_text(encoding="utf-8"))


def cmd_sop_index(_: argparse.Namespace) -> int:
    data = _load_sop_index()
    print(f"# Disklordz SOP index (v{data.get('version', '?')})\n")
    print(f"Registry: `{SOP_INDEX.relative_to(REPO_ROOT)}`\n")
    for row in data.get("sops", []):
        print(
            f"- **{row['id']}** — {row['title']} · owner `{row['owner_seat']}` · "
            f"`{row['path']}`"
        )
    print("\nDocs: `docs/HERMES_SOP_OPERATIONS.md`")
    return 0


def cmd_sop_audit(_: argparse.Namespace) -> int:
    data = _load_sop_index()
    errors: list[str] = []
    ids: set[str] = set()
    for row in data.get("sops", []):
        sid = row.get("id", "")
        if not sid:
            errors.append("SOP row missing id")
            continue
        if sid in ids:
            errors.append(f"Duplicate SOP id: {sid}")
        ids.add(sid)
        rel = row.get("path", "")
        path = OPS_ROOT / rel
        if not path.is_file():
            errors.append(f"{sid}: missing file `{path.relative_to(REPO_ROOT)}`")
        owner = row.get("owner_seat", "")
        if owner and owner not in AGENT_SEATS:
            errors.append(f"{sid}: unknown owner_seat {owner!r}")
    readme = SOPS_ROOT / "README.md"
    if not readme.is_file():
        errors.append("Missing sops/README.md")
    skill = REPO_ROOT / ".cursor/skills/hermes-elite-sop/SKILL.md"
    if not skill.is_file():
        errors.append("Missing hermes-elite-sop skill")
    print("# SOP audit\n")
    if errors:
        for e in errors:
            print(f"FAIL: {e}")
        print(f"\n**{len(errors)}** error(s)")
        return 1
    print(f"OK — {len(data.get('sops', []))} SOPs indexed; all files present.")
    return 0


def cmd_sop_coverage(args: argparse.Namespace) -> int:
    data = _load_sop_index()
    by_owner: dict[str, list[str]] = {}
    by_consumer: dict[str, list[str]] = {}
    for row in data.get("sops", []):
        sid = row["id"]
        by_owner.setdefault(row["owner_seat"], []).append(sid)
        for seat in row.get("consumer_seats", []):
            by_consumer.setdefault(seat, []).append(sid)
    if args.seat:
        if args.seat not in AGENT_SEATS:
            print(f"Unknown seat {args.seat!r}", file=sys.stderr)
            return 1
        print(f"# SOP coverage — {args.seat}\n")
        print("## Owns\n")
        for sid in by_owner.get(args.seat, []):
            print(f"- {sid}")
        print("\n## Consumes\n")
        for sid in by_consumer.get(args.seat, []):
            print(f"- {sid}")
        if args.seat not in by_owner and args.seat not in by_consumer:
            print("(no SOP rows — hermes-sop should add coverage or document N/A)")
        return 0
    print("# SOP coverage — all seats\n")
    uncovered = []
    for seat in AGENT_SEATS:
        owns = by_owner.get(seat, [])
        consumes = by_consumer.get(seat, [])
        if not owns and not consumes and seat not in ("hermes-lead",):
            uncovered.append(seat)
        print(f"- **{seat}** — owns {len(owns)}, consumes {len(consumes)}")
    if uncovered:
        print("\n## Seats with zero SOP links (consider adding)\n")
        for s in uncovered:
            print(f"- {s}")
    return 0


def cmd_sop_draft(args: argparse.Namespace) -> int:
    tpl = _read(HERMES_ROOT / "templates" / "sop-procedure.md")
    today = datetime.now(timezone.utc).strftime("%Y-%m-%d")
    consumers = args.consumer or [args.owner]
    body = (
        tpl.replace("{{SOP_ID}}", args.id)
        .replace("{{TITLE}}", args.title)
        .replace("{{OWNER_SEAT}}", args.owner)
        .replace("{{CONSUMER_SEATS}}", ", ".join(consumers))
        .replace("{{CADENCE}}", args.cadence or "on_demand")
        .replace("{{DATE}}", today)
        .replace("{{PURPOSE}}", args.purpose or "(describe purpose)")
        .replace("{{SKILL_PATH}}", f".cursor/skills/hermes-elite-{args.owner.replace('hermes-', '')}/SKILL.md")
        .replace("{{RELATED_SOPS}}", args.related or "SOP-HERMES-002")
    )
    slug = re.sub(r"[^a-z0-9]+", "-", args.title.lower())[:48].strip("-")
    filename = f"{args.id}-{slug}.md"
    path = SOPS_ROOT / filename
    if path.exists() and not args.force:
        print(f"Exists: {path.relative_to(REPO_ROOT)} (use --force)", file=sys.stderr)
        return 1
    if args.write:
        path.write_text(body, encoding="utf-8")
        print(f"wrote {path.relative_to(REPO_ROOT)}", file=sys.stderr)
        print("Next: add row to sops/INDEX.json and README.md; run sop audit", file=sys.stderr)
    print(body)
    return 0


def cmd_data_checklist(_: argparse.Namespace) -> int:
    web = REPO_ROOT / "disklordz" / "website"
    print("# Hermes data — Supabase / storage checklist\n")
    mig = web / "supabase" / "migrations"
    if mig.is_dir():
        for p in sorted(mig.glob("*.sql")):
            print(f"- `{p.relative_to(REPO_ROOT)}`")
    else:
        print("- No `disklordz/website/supabase/migrations` directory found.")
    print("\nDocs:")
    print("- `disklordz/website/ARCHITECTURE.md`")
    print("- WO-SAAS-012 pgvector when enabled")
    print("\nNever commit service role keys; use `.env.example` names only.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Hermes seat toolkit")
    sub = parser.add_subparsers(dest="seat", required=True)

    ops = sub.add_parser("ops", help="hermes-ops")
    ops_sub = ops.add_subparsers(dest="ops_cmd", required=True)
    ops_sub.add_parser("checklist", help="Print dispatch checklist").set_defaults(
        func=cmd_ops_checklist
    )
    p_val = ops_sub.add_parser("validate-pr", help="Check WO in PR title")
    p_val.add_argument("--title", required=True)
    p_val.add_argument("--allow-missing", action="store_true")
    p_val.set_defaults(func=cmd_ops_validate_pr)

    devops = sub.add_parser("devops", help="hermes-devops")
    devops_sub = devops.add_subparsers(dest="devops_cmd", required=True)
    devops_sub.add_parser("summary", help="CI workflows + local commands").set_defaults(
        func=cmd_devops_summary
    )

    handoff = sub.add_parser("handoff", help="hermes-handoff")
    handoff_sub = handoff.add_subparsers(dest="handoff_cmd", required=True)
    hd = handoff_sub.add_parser("draft", help="Draft HO-*.json in hermes/outbox")
    hd.add_argument("--wo", required=True)
    hd.add_argument("--title", required=True)
    hd.add_argument("--criteria", default="")
    hd.add_argument("--branch", default="main")
    hd.add_argument("--path", action="append")
    hd.add_argument("--notes", default="")
    hd.set_defaults(func=cmd_handoff_draft)

    gtm = sub.add_parser("gtm", help="hermes-gtm")
    gtm_sub = gtm.add_subparsers(dest="gtm_cmd", required=True)
    gb = gtm_sub.add_parser("brief", help="GTM draft brief")
    gb.add_argument("--product", choices=["junova", "saas", "novadrum"], default="junova")
    gb.add_argument("--write", action="store_true")
    gb.set_defaults(func=cmd_gtm_brief)

    presets = sub.add_parser("presets", help="hermes-presets")
    presets_sub = presets.add_subparsers(dest="presets_cmd", required=True)
    pa = presets_sub.add_parser("audit", help="List preset-related files")
    pa.add_argument("--product", choices=["junova", "wave909", "jd"], default="junova")
    pa.set_defaults(func=cmd_presets_audit)

    support = sub.add_parser("support", help="hermes-support")
    support_sub = support.add_subparsers(dest="support_cmd", required=True)
    support_sub.add_parser("rag-status", help="RAG corpus health").set_defaults(
        func=cmd_support_rag_status
    )

    security = sub.add_parser("security", help="hermes-security")
    security_sub = security.add_subparsers(dest="security_cmd", required=True)
    sc = security_sub.add_parser("scan", help="Heuristic secret scan")
    sc.add_argument("--staged", action="store_true", help="Staged files only")
    sc.set_defaults(func=cmd_security_scan)

    data = sub.add_parser("data", help="hermes-data")
    data_sub = data.add_subparsers(dest="data_cmd", required=True)
    data_sub.add_parser("checklist", help="Supabase migration checklist").set_defaults(
        func=cmd_data_checklist
    )

    sop = sub.add_parser("sop", help="hermes-sop — procedures OS")
    sop_sub = sop.add_subparsers(dest="sop_cmd", required=True)
    sop_sub.add_parser("index", help="Print SOP catalog").set_defaults(func=cmd_sop_index)
    sop_sub.add_parser("audit", help="Validate INDEX.json and files").set_defaults(func=cmd_sop_audit)
    cov = sop_sub.add_parser("coverage", help="Map seats to SOPs")
    cov.add_argument("--seat", default="", help="Single seat detail")
    cov.set_defaults(func=cmd_sop_coverage)
    dr = sop_sub.add_parser("draft", help="Draft new SOP from template")
    dr.add_argument("--id", required=True, help="e.g. SOP-WEB-002")
    dr.add_argument("--title", required=True)
    dr.add_argument("--owner", required=True, help="owner_seat e.g. hermes-web")
    dr.add_argument("--consumer", action="append", help="consumer seat (repeatable)")
    dr.add_argument("--cadence", default="on_demand")
    dr.add_argument("--purpose", default="")
    dr.add_argument("--related", default="")
    dr.add_argument("--write", action="store_true")
    dr.add_argument("--force", action="store_true")
    dr.set_defaults(func=cmd_sop_draft)

    agent = sub.add_parser("agent", help="Per-seat agent repos (self-improvement)")
    agent_sub = agent.add_subparsers(dest="agent_cmd", required=True)
    agent_sub.add_parser("init", help="Scaffold all agent repos").set_defaults(func=cmd_agent_init)
    agent_sub.add_parser("status", help="List agent repo health").set_defaults(func=cmd_agent_status)

    def cmd_agent_promote_scan(_: argparse.Namespace) -> int:
        script = HERMES_ROOT / "scripts" / "agent_promote.py"
        return subprocess.run([sys.executable, str(script), "scan"], cwd=str(REPO_ROOT)).returncode

    def cmd_agent_promote_open(args: argparse.Namespace) -> int:
        script = HERMES_ROOT / "scripts" / "agent_promote.py"
        cmd = [sys.executable, str(script), "open", "--source", args.source or "HEAD"]
        if args.seat:
            cmd.extend(["--seat", args.seat])
        if args.proposal:
            cmd.extend(["--proposal", args.proposal])
        if args.force:
            cmd.append("--force")
        return subprocess.run(cmd, cwd=str(REPO_ROOT)).returncode

    agent_sub.add_parser("promote-scan", help="List pending skill proposals").set_defaults(
        func=cmd_agent_promote_scan
    )
    po = agent_sub.add_parser("promote-open", help="Open draft promotion PR(s) (needs gh)")
    po.add_argument("--seat", default="")
    po.add_argument("--proposal", default="")
    po.add_argument("--source", default="HEAD")
    po.add_argument("--force", action="store_true")
    po.set_defaults(func=cmd_agent_promote_open)
    ar = agent_sub.add_parser("record-run", help="Append run log under agent-repos/<seat>/runs/")
    ar.add_argument("--seat", required=True, choices=AGENT_SEATS)
    ar.add_argument("--wo", default="")
    ar.add_argument("--branch", default="")
    ar.add_argument("--summary", required=True)
    ar.add_argument("--evidence", default="")
    ar.add_argument("--learnings", default="")
    ar.set_defaults(func=cmd_agent_record_run)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
