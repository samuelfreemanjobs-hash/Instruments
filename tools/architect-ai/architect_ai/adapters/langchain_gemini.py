"""Optional LangChain wrapper (install langchain-google-genai)."""

from __future__ import annotations

import os

from architect_ai.adapters.base import ChatModel
from architect_ai.prompts import ARCHITECTAI_SYSTEM_PROMPT


class LangChainGeminiChatModel:
    """Thin adapter using LangChain ChatGoogleGenerativeAI."""

    def __init__(self, model: str | None = None) -> None:
        key = os.environ.get("GEMINI_API_KEY") or os.environ.get("GOOGLE_API_KEY")
        if not key:
            raise RuntimeError("Missing GEMINI_API_KEY")
        try:
            from langchain_google_genai import ChatGoogleGenerativeAI
            from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
        except ImportError as exc:
            raise RuntimeError(
                "pip install langchain-google-genai langchain-core"
            ) from exc
        self._HumanMessage = HumanMessage
        self._AIMessage = AIMessage
        self._SystemMessage = SystemMessage
        self._llm = ChatGoogleGenerativeAI(
            model=model or os.environ.get("ARCHITECTAI_GEMINI_MODEL", "gemini-2.0-flash"),
            google_api_key=key,
            temperature=0.4,
        )

    def chat(self, system: str, messages: list[dict[str, str]]) -> str:
        lc_messages = [self._SystemMessage(content=system or ARCHITECTAI_SYSTEM_PROMPT)]
        for msg in messages:
            if msg["role"] == "user":
                lc_messages.append(self._HumanMessage(content=msg["content"]))
            else:
                lc_messages.append(self._AIMessage(content=msg["content"]))
        result = self._llm.invoke(lc_messages)
        return (result.content or "").strip()
