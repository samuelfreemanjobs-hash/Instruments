#!/usr/bin/env python3
"""Hermes lead: scan agent-repo proposals and open draft promotion PRs."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
AGENT_REPOS = REPO_ROOT / "disklordz" / "hermes" / "agent-repos"
BRANCH_SUFFIX = "-a77b"
PROMOTE_PREFIX = "cursor/hermes-promote-"


@dataclass
class Proposal:
    seat: str
    path: Path
    slug: str
    status: str
    promotion_pr: str
    title: str


def parse_proposal(seat: str, path: Path) -> Proposal:
    text = path.read_text(encoding="utf-8")
    status_m = re.search(r"\*\*Status:\*\*\s*(\S+)", text)
    status = (status_m.group(1).lower() if status_m else "draft").strip("*")
    pr_m = re.search(r"\*\*Promotion PR:\*\*\s*(.+)", text)
    promotion_pr = pr_m.group(1).strip() if pr_m else ""
    title_m = re.search(r"^#\s+Proposal\s+—\s+(.+)$", text, re.M)
    title = title_m.group(1).strip() if title_m else path.stem
    return Proposal(seat, path, path.stem, status, promotion_pr, title)


def iter_proposals() -> list[Proposal]:
    out: list[Proposal] = []
    for seat_dir in sorted(AGENT_REPOS.glob("hermes-*")):
        if not seat_dir.is_dir():
            continue
        prop_dir = seat_dir / "proposals"
        if not prop_dir.is_dir():
            continue
        for path in sorted(prop_dir.glob("*.md")):
            out.append(parse_proposal(seat_dir.name, path))
    return out


def run(cmd: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd or REPO_ROOT, text=True, capture_output=True)


def git_default_branch() -> str:
    r = run(["git", "symbolic-ref", "refs/remotes/origin/HEAD"])
    if r.returncode == 0 and r.stdout.strip():
        return r.stdout.strip().split("/")[-1]
    return "main"


def branch_for(proposal: Proposal) -> str:
    short = re.sub(r"[^a-z0-9]+", "-", proposal.slug.lower())[:48].strip("-")
    return f"{PROMOTE_PREFIX}{proposal.seat.replace('hermes-', '')}-{short}{BRANCH_SUFFIX}"


def cmd_scan(_: argparse.Namespace) -> int:
    print("# Hermes promotion queue\n")
    for p in iter_proposals():
        if p.status == "merged":
            continue
        flag = "needs PR" if not p.promotion_pr else "PR logged"
        print(f"- **{p.seat}** `{p.path.name}` · status={p.status} · {flag}")
    return 0


def proposal_pr_body(proposal: Proposal) -> str:
    rel = proposal.path.relative_to(REPO_ROOT)
    text = proposal.path.read_text(encoding="utf-8")
    return (
        "## Hermes skill promotion (auto-opened by lead)\n\n"
        f"**Seat:** `{proposal.seat}`  \n"
        f"**Proposal:** `{rel}`  \n"
        f"**Status in file:** `{proposal.status}`\n\n"
        "CD: set `**Status:** approved` in the proposal file when ready to merge skill edits. "
        "After merge, set `**Status:** merged` and link this PR under `**Promotion PR:**`.\n\n"
        "---\n\n"
        f"{text}\n"
    )


def cmd_open(args: argparse.Namespace) -> int:
    proposals = iter_proposals()
    if args.seat:
        proposals = [p for p in proposals if p.seat == args.seat]
    if args.proposal:
        proposals = [p for p in proposals if p.path.name == args.proposal or p.slug == args.proposal]

    base = args.base or git_default_branch()
    opened = 0
    for proposal in proposals:
        if proposal.status == "merged":
            continue
        if proposal.promotion_pr and not args.force:
            print(f"skip {proposal.path.name} (promotion PR already set)")
            continue
        if proposal.status not in ("draft", "approved") and not args.force:
            print(f"skip {proposal.path.name} (status={proposal.status})")
            continue

        branch = branch_for(proposal)
        rel = proposal.path.relative_to(REPO_ROOT)

        run(["git", "fetch", "origin", base])
        if run(["git", "show-ref", "--verify", f"refs/heads/{branch}"]).returncode != 0:
            if run(["git", "checkout", "-b", branch, f"origin/{base}"]).returncode != 0:
                run(["git", "checkout", "-b", branch, base])

        run(["git", "checkout", args.source, "--", str(rel)])
        run(["git", "add", str(rel)])

        if proposal.status == "approved":
            # Approved proposals may include skill edits on the source branch; stage skill if changed.
            skill_glob = list(REPO_ROOT.glob(f".cursor/skills/hermes-elite-*/SKILL.md"))
            for skill in skill_glob:
                seat_skill = proposal.seat.replace("hermes-", "hermes-elite-")
                if seat_skill in str(skill):
                    r = run(["git", "checkout", args.source, "--", str(skill.relative_to(REPO_ROOT))])
                    if r.returncode == 0:
                        run(["git", "add", str(skill.relative_to(REPO_ROOT))])

        commit = run(
            ["git", "commit", "-m", f"hermes(promote): {proposal.seat} — {proposal.slug}"],
        )
        if commit.returncode != 0 and "nothing to commit" in (commit.stdout + commit.stderr):
            print(f"skip {proposal.path.name} (no diff vs {base})")
            continue
        if commit.returncode != 0:
            print(commit.stderr or commit.stdout, file=sys.stderr)
            return 1

        push = run(["git", "push", "-u", "origin", branch])
        if push.returncode != 0:
            print(push.stderr or push.stdout, file=sys.stderr)
            return 1

        title = f"Hermes promote: {proposal.seat} — {proposal.title[:60]}"
        body = proposal_pr_body(proposal)
        pr = run(
            [
                "gh",
                "pr",
                "create",
                "--base",
                base,
                "--head",
                branch,
                "--title",
                title,
                "--body",
                body,
                "--draft",
            ],
        )
        if pr.returncode != 0:
            print(pr.stderr or pr.stdout, file=sys.stderr)
            return 1
        url = pr.stdout.strip()
        print(url)
        opened += 1

        if args.write_back:
            text = proposal.path.read_text(encoding="utf-8")
            if "**Promotion PR:**" in text:
                text = re.sub(r"\*\*Promotion PR:\*\*\s*.+", f"**Promotion PR:** {url}", text)
            else:
                text = text.replace(
                    f"**Status:** {proposal.status}",
                    f"**Status:** {proposal.status}\n**Promotion PR:** {url}",
                    1,
                )
            proposal.path.write_text(text, encoding="utf-8")

    if opened == 0:
        print("No promotion PRs opened.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Hermes agent proposal promotion (lead)")
    sub = parser.add_subparsers(dest="cmd", required=True)
    sub.add_parser("scan", help="List non-merged proposals").set_defaults(func=cmd_scan)

    op = sub.add_parser("open", help="Open draft promotion PR(s) via gh")
    op.add_argument("--seat", default="")
    op.add_argument("--proposal", default="", help="Filename or slug")
    op.add_argument("--base", default="", help="Target branch (default: origin HEAD)")
    op.add_argument("--source", default="HEAD", help="Branch/commit to copy files from")
    op.add_argument("--force", action="store_true", help="Open even if Promotion PR set")
    op.add_argument(
        "--write-back",
        action="store_true",
        help="Write Promotion PR URL into proposal file (local only)",
    )
    op.set_defaults(func=cmd_open)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
