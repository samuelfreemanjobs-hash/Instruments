from __future__ import annotations

import logging
import os
from contextlib import contextmanager
from typing import Iterator

from coding_agent_stack.config import get_settings

logger = logging.getLogger("coding_agent_stack")


def configure_observability() -> None:
    settings = get_settings()
    if settings.langfuse_public_key and settings.langfuse_secret_key:
        os.environ.setdefault("LANGFUSE_PUBLIC_KEY", settings.langfuse_public_key)
        os.environ.setdefault("LANGFUSE_SECRET_KEY", settings.langfuse_secret_key)
        os.environ.setdefault("LANGFUSE_HOST", settings.langfuse_host)


@contextmanager
def trace_span(name: str, metadata: dict | None = None) -> Iterator[None]:
    configure_observability()
    try:
        from langfuse.decorators import langfuse_context, observe  # type: ignore

        @observe(name=name)
        def _inner():
            if metadata:
                langfuse_context.update_current_trace(metadata=metadata)

        _inner()
        yield
    except ImportError:
        logger.debug("langfuse not installed; span=%s meta=%s", name, metadata)
        yield
