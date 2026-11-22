"""OpenAI Chat Completions adapter (optional)."""

from __future__ import annotations

import os

from architect_ai.adapters.base import ChatModel


class OpenAIChatModel:
    def __init__(self, model: str | None = None, api_key: str | None = None) -> None:
        key = api_key or os.environ.get("OPENAI_API_KEY")
        if not key:
            raise RuntimeError("Missing OPENAI_API_KEY")
        self._model = model or os.environ.get("ARCHITECTAI_OPENAI_MODEL", "gpt-4o-mini")
        try:
            from openai import OpenAI
        except ImportError as exc:
            raise RuntimeError("pip install openai") from exc
        self._client = OpenAI(api_key=key)

    def chat(self, system: str, messages: list[dict[str, str]]) -> str:
        payload = [{"role": "system", "content": system}]
        payload.extend(messages)
        resp = self._client.chat.completions.create(
            model=self._model,
            messages=payload,
            temperature=0.4,
            max_tokens=8192,
        )
        choice = resp.choices[0].message.content
        return (choice or "").strip()
