"""Dispatch autonomous jobs to Gemini, Aider, Jules, or Cursor triggers."""

from __future__ import annotations

import json
import os
import subprocess
import urllib.error
import urllib.request
from typing import Any

from agent_rules import agent_rules_block
from github_pr import github_token

JULES_LABEL = "jules"


def _request_json(
    method: str,
    url: str,
    token: str,
    payload: dict | None = None,
) -> tuple[int, Any]:
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "instruments-autonomous-coder",
        "Authorization": f"Bearer {token}",
        "X-GitHub-Api-Version": "2022-11-28",
    }
    data = json.dumps(payload).encode("utf-8") if payload is not None else None
    if payload is not None:
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=45) as resp:
            body = resp.read().decode("utf-8")
            return resp.status, json.loads(body) if body else {}
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        try:
            parsed = json.loads(body) if body else {"error": body}
        except json.JSONDecodeError:
            parsed = {"error": body}
        return exc.code, parsed


def add_pr_label(owner: str, repo: str, pr_number: int, label: str, token: str) -> bool:
    url = f"https://api.github.com/repos/{owner}/{repo}/issues/{pr_number}/labels"
    status, data = _request_json("POST", url, token, {"labels": [label]})
    if status >= 400:
        print(f"label PR #{pr_number} with {label} failed HTTP {status}: {data}")
        return False
    print(f"labeled PR #{pr_number} with {label}")
    return True


def workflow_dispatch(
    owner: str,
    repo: str,
    workflow_file: str,
    inputs: dict[str, str],
    token: str,
    *,
    ref: str = "main",
) -> bool:
    url = f"https://api.github.com/repos/{owner}/{repo}/actions/workflows/{workflow_file}/dispatches"
    status, data = _request_json("POST", url, token, {"ref": ref, "inputs": inputs})
    if status >= 400:
        print(f"workflow_dispatch {workflow_file} HTTP {status}: {data}")
        return False
    print(f"dispatched workflow {workflow_file} inputs={inputs}")
    return True


def dispatch_gemini(
    owner: str,
    repo: str,
    pr_number: int,
    task: str,
    token: str,
    *,
    ref: str,
) -> bool:
    ok = workflow_dispatch(
        owner,
        repo,
        "agent-gemini.yml",
        {"pr_number": str(pr_number), "task": task[:6000]},
        token,
        ref=ref,
    )
    if ok:
        return True
    return add_pr_label(owner, repo, pr_number, "agent:gemini", token)


def dispatch_aider(
    owner: str,
    repo: str,
    pr_number: int,
    task: str,
    token: str,
    *,
    ref: str,
) -> bool:
    ok = workflow_dispatch(
        owner,
        repo,
        "agent-aider.yml",
        {"pr_number": str(pr_number), "task": task[:6000]},
        token,
        ref=ref,
    )
    if ok:
        return True
    return add_pr_label(owner, repo, pr_number, "agent:aider", token)


def dispatch_cline(
    owner: str,
    repo: str,
    pr_number: int,
    task: str,
    token: str,
    *,
    ref: str,
) -> bool:
    if not (os.environ.get("OPENROUTER_API_KEY") or os.environ.get("CLINE_API_KEY")):
        print("SKIP cline workflow: set OPENROUTER_API_KEY or CLINE_API_KEY")
        return False
    ok = workflow_dispatch(
        owner,
        repo,
        "agent-cline.yml",
        {"pr_number": str(pr_number), "task": task[:6000]},
        token,
        ref=ref,
    )
    if ok:
        return True
    return add_pr_label(owner, repo, pr_number, "agent:cline", token)


def _find_issue_by_title(
    owner: str, repo: str, title: str, token: str
) -> int | None:
    q_title = title.replace('"', "")
    url = (
        f"https://api.github.com/search/issues?q=repo:{owner}/{repo}+"
        f'is:issue+in:title+"{q_title}"'
    )
    status, data = _request_json("GET", url, token)
    if status >= 400:
        return None
    for it in data.get("items") or []:
        if it.get("pull_request"):
            continue
        if (it.get("title") or "").strip() == title.strip():
            return int(it["number"])
    return None


def dispatch_jules_issue(
    owner: str,
    repo: str,
    *,
    pr_number: int | None,
    slice_id: str,
    task: str,
    token: str,
    state: dict,
) -> bool:
    title = f"[autonomous] {slice_id}" + (f" (PR #{pr_number})" if pr_number else "")
    body = "\n".join(
        [
            f"Autonomous coder routed this **easy** job to **Jules**.",
            "",
            f"- **Queue id:** `{slice_id}`",
            f"- **PR:** #{pr_number}" if pr_number else "- **PR:** (none)",
            "",
            "## Task",
            task,
            "",
            "## Rules",
            agent_rules_block(),
            "",
            "---",
            "This issue was created by `scripts/autonomous-coder/tick.py`. "
            "Apply the GitHub label `jules` (case insensitive) per "
            "https://jules.google/docs/running-tasks/",
        ]
    )

    existing = state.get("jules_issues") or {}
    issue_key = f"pr:{pr_number}" if pr_number else f"slice:{slice_id}"
    issue_num = existing.get(issue_key)

    if issue_num is None:
        issue_num = _find_issue_by_title(owner, repo, title, token)

    if issue_num is None:
        url = f"https://api.github.com/repos/{owner}/{repo}/issues"
        status, data = _request_json(
            "POST",
            url,
            token,
            {"title": title, "body": body, "labels": [JULES_LABEL]},
        )
        if status >= 400:
            print(f"create Jules issue HTTP {status}: {data}")
            return False
        issue_num = data.get("number")
    else:
        url = f"https://api.github.com/repos/{owner}/{repo}/issues/{issue_num}"
        status, data = _request_json("PATCH", url, token, {"body": body})
        if status >= 400:
            print(f"update Jules issue #{issue_num} HTTP {status}: {data}")
        add_issue_label(owner, repo, int(issue_num), JULES_LABEL, token)

    if issue_num:
        state.setdefault("jules_issues", {})[issue_key] = issue_num
        print(f"Jules issue #{issue_num} ready (label `{JULES_LABEL}`)")
        optional_jules_api(owner, repo, task, pr_number)
        return True
    return False


def add_issue_label(owner: str, repo: str, issue_number: int, label: str, token: str) -> None:
    url = f"https://api.github.com/repos/{owner}/{repo}/issues/{issue_number}/labels"
    _request_json("POST", url, token, {"labels": [label]})


def optional_jules_api(owner: str, repo: str, task: str, pr_number: int | None) -> None:
    """Optional REST API path when JULES_API_KEY is set (alpha API)."""
    key = (os.environ.get("JULES_API_KEY") or "").strip()
    if not key:
        return
    source = f"sources/github/{owner}/{repo}"
    branch = "main"
    payload = {
        "prompt": task[:8000],
        "title": f"Autonomous slice PR #{pr_number}" if pr_number else "Autonomous slice",
        "sourceContext": {
            "source": source,
            "githubRepoContext": {"startingBranch": branch},
        },
    }
    req = urllib.request.Request(
        "https://jules.googleapis.com/v1alpha/sessions",
        data=json.dumps(payload).encode("utf-8"),
        headers={
            "Content-Type": "application/json",
            "x-goog-api-key": key,
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            print(f"Jules API session created: {data.get('id') or data.get('name')}")
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        print(f"Jules API optional call failed HTTP {exc.code}: {body[:500]}")


def gh_pr_comment(owner: str, repo: str, pr_number: int, body: str) -> bool:
    token = github_token()
    if not token:
        return False
    return subprocess.run(
        [
            "gh",
            "pr",
            "comment",
            str(pr_number),
            "--repo",
            f"{owner}/{repo}",
            "--body",
            body,
        ],
        env={**os.environ, "GH_TOKEN": token, "GITHUB_TOKEN": token},
        check=False,
    ).returncode == 0