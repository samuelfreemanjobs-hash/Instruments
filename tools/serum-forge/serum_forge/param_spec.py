"""OpenAPI-style JSON schema for constrained LLM output."""

from __future__ import annotations

from serum_forge.models import SerumSymbolPatch


def param_openapi_schema() -> dict:
    return SerumSymbolPatch.model_json_schema()


def validate_symbolic_payload(data: dict) -> SerumSymbolPatch:
    return SerumSymbolPatch.model_validate(data)
