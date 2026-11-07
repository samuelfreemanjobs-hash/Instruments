from __future__ import annotations

from functools import lru_cache
from pathlib import Path

from pydantic_settings import BaseSettings, SettingsConfigDict


def _default_repo_root() -> Path:
    here = Path(__file__).resolve()
    for parent in here.parents:
        if (parent / "ARCHITECTURE.md").is_file() and (parent / "vst-testing-ops").is_dir():
            return parent
    return here.parents[2]


class Settings(BaseSettings):
    model_config = SettingsConfigDict(env_file=".env", env_file_encoding="utf-8", extra="ignore")

    repo_root: Path = _default_repo_root()
    openai_api_key: str = ""
    openai_base_url: str = ""
    anthropic_api_key: str = ""
    agent_model: str = "gpt-4o-mini"
    agent_max_turns: int = 12
    e2b_api_key: str = ""
    cors_origins: str = "http://127.0.0.1:8790,http://localhost:8790"
    langfuse_public_key: str = ""
    langfuse_secret_key: str = ""
    langfuse_host: str = "https://cloud.langfuse.com"

    # Grok Bot — ACP / webhook ingress (your existing Grokbot channel)
    grok_webhook_url: str = ""
    grok_bot_token: str = ""
    grok_channel_id: str = ""
    grok_webhook_secret: str = ""
    grok_http_timeout_s: float = 45.0
    grok_routines_json: str = ""
    agents_profile_rel: str = "AGENTS.md"

    @property
    def grok_configured(self) -> bool:
        return bool(self.grok_webhook_url.strip())

    @property
    def cors_origin_list(self) -> list[str]:
        return [o.strip() for o in self.cors_origins.split(",") if o.strip()]


@lru_cache
def get_settings() -> Settings:
    return Settings()
