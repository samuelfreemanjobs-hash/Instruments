"""Optional Anthropic/OpenAI completion — skipped if no API key."""

from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from typing import Any


def _extract_json(text: str) -> dict[str, Any]:
    text = text.strip()
    if text.startswith("```"):
        parts = text.split("```")
        for part in parts:
            part = part.strip()
            if part.startswith("json"):
                part = part[4:].strip()
            if part.startswith("{"):
                return json.loads(part)
    return json.loads(text)


def complete_json(messages: list[dict[str, str]], max_tokens: int = 4096) -> dict[str, Any]:
    key = os.environ.get("ANTHROPIC_API_KEY")
    if key:
        return _anthropic(messages, key, max_tokens)
    key = os.environ.get("OPENAI_API_KEY")
    if key:
        return _openai(messages, key, max_tokens)
    raise RuntimeError(
        "Set ANTHROPIC_API_KEY or OPENAI_API_KEY for automated runs, "
        "or use `prompt` subcommand and paste into Cursor/Claude."
    )


def _anthropic(messages: list[dict[str, str]], api_key: str, max_tokens: int) -> dict[str, Any]:
    system = ""
    user_parts: list[str] = []
    for m in messages:
        if m["role"] == "system":
            system = m["content"]
        else:
            user_parts.append(m["content"])
    body = {
        "model": os.environ.get("BUSINESS_AGENT_ANTHROPIC_MODEL", "claude-sonnet-4-20250514"),
        "max_tokens": max_tokens,
        "system": system,
        "messages": [{"role": "user", "content": "\n\n".join(user_parts)}],
    }
    req = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps(body).encode("utf-8"),
        headers={
            "Content-Type": "application/json",
            "x-api-key": api_key,
            "anthropic-version": "2023-06-01",
        },
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=120) as resp:
        payload = json.loads(resp.read().decode())
    text = payload["content"][0]["text"]
    return _extract_json(text)


def _openai(messages: list[dict[str, str]], api_key: str, max_tokens: int) -> dict[str, Any]:
    body = {
        "model": os.environ.get("BUSINESS_AGENT_OPENAI_MODEL", "gpt-4o"),
        "max_tokens": max_tokens,
        "messages": messages,
        "response_format": {"type": "json_object"},
    }
    req = urllib.request.Request(
        "https://api.openai.com/v1/chat/completions",
        data=json.dumps(body).encode("utf-8"),
        headers={
            "Content-Type": "application/json",
            "Authorization": f"Bearer {api_key}",
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=120) as resp:
            payload = json.loads(resp.read().decode())
    except urllib.error.HTTPError as e:
        raise RuntimeError(f"OpenAI API error: {e.read().decode()}") from e
    text = payload["choices"][0]["message"]["content"]
    return _extract_json(text)
