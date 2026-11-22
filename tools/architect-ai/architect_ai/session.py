"""Conversation session over a ChatModel port."""

from __future__ import annotations

from architect_ai.adapters.base import ChatModel
from architect_ai.prompts import ARCHITECTAI_SYSTEM_PROMPT, COSI_REVIEW_APPENDIX


class ArchitectSession:
    def __init__(
        self,
        model: ChatModel,
        *,
        system: str | None = None,
        force_cosi: bool = False,
    ) -> None:
        self._model = model
        self._system = system or ARCHITECTAI_SYSTEM_PROMPT
        self._force_cosi = force_cosi
        self._messages: list[dict[str, str]] = []

    @property
    def history(self) -> list[dict[str, str]]:
        return list(self._messages)

    def ask(self, user_text: str, *, extra_system: str | None = None) -> str:
        prompt = user_text
        if self._force_cosi:
            prompt = user_text + "\n\n" + COSI_REVIEW_APPENDIX
        if extra_system:
            prompt = f"### Project context\n{extra_system}\n\n### Question\n{prompt}"

        self._messages.append({"role": "user", "content": prompt})
        reply = self._model.chat(self._system, self._messages)
        self._messages.append({"role": "assistant", "content": reply})
        return reply

    def reset(self) -> None:
        self._messages.clear()
