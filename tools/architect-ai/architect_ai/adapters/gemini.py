"""Google Gemini adapter."""

from __future__ import annotations

import os

from architect_ai.adapters.base import ChatModel


class GeminiChatModel:
    def __init__(self, model: str | None = None, api_key: str | None = None) -> None:
        key = api_key or os.environ.get("GEMINI_API_KEY") or os.environ.get("GOOGLE_API_KEY")
        if not key:
            raise RuntimeError(
                "Missing GEMINI_API_KEY or GOOGLE_API_KEY. See tools/architect-ai/.env.example"
            )
        self._model_name = model or os.environ.get("ARCHITECTAI_GEMINI_MODEL", "gemini-2.0-flash")
        try:
            from google import genai
        except ImportError as exc:
            raise RuntimeError(
                "Install google-genai: pip install -r tools/architect-ai/requirements.txt"
            ) from exc
        self._client = genai.Client(api_key=key)
        self._genai = genai

    def chat(self, system: str, messages: list[dict[str, str]]) -> str:
        contents: list = []
        for msg in messages:
            role = msg["role"]
            if role == "assistant":
                role = "model"
            contents.append(
                self._genai.types.Content(
                    role=role,
                    parts=[self._genai.types.Part(text=msg["content"])],
                )
            )
        response = self._client.models.generate_content(
            model=self._model_name,
            contents=contents,
            config=self._genai.types.GenerateContentConfig(
                system_instruction=system,
                temperature=0.4,
                max_output_tokens=8192,
            ),
        )
        text = getattr(response, "text", None)
        if text:
            return text.strip()
        return "(No text in model response.)"
