"""LLM adapter port (hexagonal boundary)."""

from __future__ import annotations

from typing import Protocol


class ChatModel(Protocol):
    def chat(self, system: str, messages: list[dict[str, str]]) -> str:
        """Return assistant text. messages: [{role, content}, ...] user/assistant only."""
