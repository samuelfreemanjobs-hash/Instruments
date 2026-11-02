from __future__ import annotations

from typing import Any

from renaissance_host.config import BoxConfig
from renaissance_host.extensions.disklordz_inbox import DisklordzInboxExtension
from renaissance_host.extensions.log_pads import LogPadsExtension


def build_extensions(cfg: BoxConfig) -> list[Any]:
    exts: list[Any] = []
    if cfg.extensions.log_pads:
        exts.append(LogPadsExtension())
    if cfg.extensions.disklordz_inbox.enabled:
        exts.append(DisklordzInboxExtension(cfg.extensions.disklordz_inbox))
    return exts
