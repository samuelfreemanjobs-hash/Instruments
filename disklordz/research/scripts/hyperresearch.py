#!/usr/bin/env python3
"""Hyperresearch — Hermes hermes-research corpus scanner and brief generator."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

RESEARCH_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = RESEARCH_ROOT.parents[1]
SOURCES_JSON = RESEARCH_ROOT / "research-sources.json"
TEMPLATE = RESEARCH_ROOT / "templates" / "brief-template.md"
OUTBOX = RESEARCH_ROOT / "outbox"


def load_sources(product: str) -> list[Path]:
    data = json.loads(SOURCES_JSON.read_text(encoding="utf-8"))
    products = data.get("products", {})
    keys = [product, "general"]
    paths: list[str] = []
    for k in keys:
        paths.extend(products.get(k, []))
    seen: set[str] = set()
    out: list[Path] = []
    for rel in paths:
        if rel in seen:
            continue
        seen.add(rel)
        p = REPO_ROOT / rel
        if p.is_file():
            out.append(p)
    return out


def keyword_score(query: str, text: str) -> int:
    terms = re.findall(r"[a-z0-9]+", query.lower())
    if not terms:
        return 0
    lower = text.lower()
    return sum(lower.count(t) for t in terms)


def scan_files(
    query: str, files: list[Path], product: str, top_n: int = 12
) -> list[tuple[int, Path, str]]:
    hits: list[tuple[int, Path, str]] = []
    terms = re.findall(r"[a-z0-9]+", query.lower())
    for path in files:
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        score = keyword_score(query, text)
        path_lower = str(path).lower()
        for t in terms:
            if len(t) > 3 and t in path_lower:
                score += 8
        if product == "junova" and "junova-x" in path_lower:
            score += 24
        if product == "saas" and "disklordz" in path_lower:
            score += 12
        if score <= 0:
            continue
        snippet = extract_snippet(text, query)
        hits.append((score, path, snippet))
    hits.sort(key=lambda x: (-x[0], str(x[1])))
    return hits[:top_n]


def extract_snippet(text: str, query: str, window: int = 220) -> str:
    terms = re.findall(r"[a-z0-9]+", query.lower())
    lower = text.lower()
    best_pos = 0
    best = 0
    for t in terms:
        pos = lower.find(t)
        if pos >= 0 and (best == 0 or pos < best_pos):
            best_pos = pos
            best = pos
    if best == 0 and terms:
        for t in terms:
            p = lower.find(t)
            if p >= 0:
                best_pos = p
                break
    start = max(0, best_pos - window // 2)
    chunk = text[start : start + window].replace("\n", " ")
    return chunk.strip()


def run_rag_query(query: str) -> str:
    script = REPO_ROOT / "disklordz" / "rag" / "scripts" / "query_local.py"
    if not script.is_file():
        return "(RAG script missing)"
    try:
        proc = subprocess.run(
            [sys.executable, str(script), query],
            capture_output=True,
            text=True,
            timeout=30,
            cwd=str(REPO_ROOT),
        )
        if proc.returncode != 0:
            return f"(RAG unavailable: {proc.stderr.strip() or proc.stdout.strip()})"
        return proc.stdout.strip() or "(no RAG hits)"
    except subprocess.TimeoutExpired:
        return "(RAG query timed out)"


def slugify(topic: str) -> str:
    s = re.sub(r"[^a-z0-9]+", "-", topic.lower()).strip("-")
    return s[:48] or "brief"


def implement_seat(product: str) -> str:
    return {
        "saas": "hermes-web",
        "junova": "hermes-dsp + hermes-gui",
        "plugin": "hermes-architect + hermes-qa",
        "general": "hermes-lead",
    }.get(product, "hermes-lead")


def wo_prefix(product: str) -> str:
    return {
        "saas": "WO-SAAS",
        "junova": "WO-2026",
        "plugin": "WO-2026",
        "general": "WO",
    }.get(product, "WO")


def build_findings(hits: list[tuple[int, Path, str]], rag_block: str | None) -> str:
    lines: list[str] = []
    for score, path, snippet in hits:
        rel = path.relative_to(REPO_ROOT)
        lines.append(f"- **[{score}]** `{rel}` — _{snippet}_")
    if rag_block:
        lines.append("\n### RAG top hits\n\n```text")
        lines.append(rag_block)
        lines.append("```")
    return "\n".join(lines) if lines else "_No keyword hits; broaden topic or add sources to research-sources.json._"


def draft_wos(product: str, topic: str, hits: list[tuple[int, Path, str]]) -> str:
    prefix = wo_prefix(product)
    title = topic[:80]
    n = "NNN"
    criteria = "Acceptance criteria filled by hermes-lead after CD review."
    if hits:
        top = hits[0][1].relative_to(REPO_ROOT)
        criteria = f"Address findings from `{top}`; attach Hyperresearch brief; PR title includes {prefix}-{n}."
    return (
        f"| ID | Title | Acceptance (draft) |\n"
        f"|----|-------|--------------------|\n"
        f"| {prefix}-{n} | [Research→Build] {title} | {criteria} |\n"
    )


def draft_gaps(topic: str, hit_count: int) -> str:
    if hit_count == 0:
        return (
            f"- **Corpus gap:** no repo hits for “{topic}” — add sources or run external research (human-reviewed).\n"
            "- **Risk:** WO drafted without evidence; do not implement until brief is enriched."
        )
    return (
        "- **Evidence depth:** repo-only; external competitor pages are not auto-fetched.\n"
        "- **Risk:** Treat all third-party content as untrusted until CD confirms."
    )


def draft_experiments(product: str) -> str:
    if product == "saas":
        return "- Colab zero-install script smoke (`docs/COLAB_ZERO_INSTALL_TESTING.md`)\n- RAG chunk refresh after doc merge"
    if product == "junova":
        return "- pluginval + standalone GUI capture\n- DSP A/B vs iPlug2 reference when imported"
    return "- Re-run Hyperresearch with `--rag` after `chunk_corpus.py`"


def render_brief(
    topic: str,
    product: str,
    hits: list[tuple[int, Path, str]],
    rag_block: str | None,
) -> str:
    tpl = TEMPLATE.read_text(encoding="utf-8")
    ts = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    sources_list = "\n".join(f"- `{p.relative_to(REPO_ROOT)}`" for p in load_sources(product))
    exec_summary = (
        f"Hyperresearch scanned **{len(hits)}** primary hits for **{product}** lens on topic: _{topic}_."
    )
    if hits:
        exec_summary += f" Strongest signal: `{hits[0][1].relative_to(REPO_ROOT)}`."
    return (
        tpl.replace("{{TITLE}}", topic[:120])
        .replace("{{PRODUCT}}", product)
        .replace("{{TIMESTAMP}}", ts)
        .replace("{{TOPIC}}", topic)
        .replace("{{EXEC_SUMMARY}}", exec_summary)
        .replace("{{SOURCES_LIST}}", sources_list)
        .replace("{{FINDINGS}}", build_findings(hits, rag_block))
        .replace("{{GAPS}}", draft_gaps(topic, len(hits)))
        .replace("{{PROPOSED_WOS}}", draft_wos(product, topic, hits))
        .replace("{{EXPERIMENTS}}", draft_experiments(product))
        .replace("{{IMPLEMENT_SEAT}}", implement_seat(product))
    )


def main() -> None:
    parser = argparse.ArgumentParser(description="Hermes Hyperresearch brief generator")
    parser.add_argument("--topic", required=True, help="Research question")
    parser.add_argument(
        "--product",
        default="general",
        choices=["saas", "junova", "plugin", "general"],
        help="Product lens for source set",
    )
    parser.add_argument("--rag", action="store_true", help="Include disklordz/rag query_local results")
    parser.add_argument(
        "--write",
        action="store_true",
        help="Write brief to disklordz/research/outbox/HR-*.md",
    )
    parser.add_argument("--stdout-only", action="store_true", help="Print brief only (default if no --write)")
    args = parser.parse_args()

    files = load_sources(args.product)
    hits = scan_files(args.topic, files, args.product)
    rag_block = run_rag_query(args.topic) if args.rag else None
    brief = render_brief(args.topic, args.product, hits, rag_block)

    if args.write:
        OUTBOX.mkdir(parents=True, exist_ok=True)
        stamp = datetime.now(timezone.utc).strftime("%Y%m%d")
        out_path = OUTBOX / f"HR-{stamp}-{slugify(args.topic)}.md"
        out_path.write_text(brief, encoding="utf-8")
        print(f"wrote {out_path.relative_to(REPO_ROOT)}", file=sys.stderr)

    print(brief)


if __name__ == "__main__":
    main()
