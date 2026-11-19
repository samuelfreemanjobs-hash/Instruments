"""Minimal GitHub PR lookups for the autonomous coder queue."""

from __future__ import annotations

import json
import os
import urllib.error
import urllib.request


def github_token() -> str | None:
    for name in ("GITHUB_TOKEN", "GH_TOKEN", "GITHUB_PAT"):
        val = (os.environ.get(name) or "").strip()
        if val:
            return val
    return None


def fetch_pr(owner: str, repo: str, number: int, token: str | None) -> dict | None:
    url = f"https://api.github.com/repos/{owner}/{repo}/pulls/{number}"
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "instruments-autonomous-coder",
    }
    if token:
        headers["Authorization"] = f"Bearer {token}"
    req = urllib.request.Request(url, headers=headers)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            return json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        if exc.code == 404:
            return None
        print(f"github PR #{number} HTTP {exc.code}")
        return None


def pr_is_open(pr: dict | None) -> bool:
    if not pr:
        return False
    state = (pr.get("state") or "").upper()
    return state == "OPEN" and not pr.get("merged_at")


def search_open_pr_by_title(
    owner: str, repo: str, keywords: list[str], token: str | None
) -> dict | None:
    q = "+".join(keywords)
    url = (
        f"https://api.github.com/search/issues?q=repo:{owner}/{repo}+"
        f"type:pr+state:open+{q}"
    )
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "instruments-autonomous-coder",
    }
    if token:
        headers["Authorization"] = f"Bearer {token}"
    req = urllib.request.Request(url, headers=headers)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            data = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError:
        return None
    items = data.get("items") or []
    if not items:
        return None
    number = items[0].get("number")
    if not number:
        return None
    return fetch_pr(owner, repo, int(number), token)