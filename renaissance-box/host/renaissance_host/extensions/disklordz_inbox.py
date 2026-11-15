from __future__ import annotations

import time
from pathlib import Path

import mido

from renaissance_host.config import DisklordzInboxConfig


class DisklordzInboxExtension:
    """Poll Disklordz inbox for new kit folders; log (future: trigger MPC import)."""

    def __init__(self, cfg: DisklordzInboxConfig) -> None:
        self._cfg = cfg
        self._seen: set[str] = set()
        self._last_poll = 0.0

    def tick(self) -> None:
        now = time.monotonic()
        if now - self._last_poll < self._cfg.poll_seconds:
            return
        self._last_poll = now
        inbox = self._cfg.inbox_dir
        if not inbox.is_dir():
            return
        for child in sorted(inbox.iterdir()):
            if not child.is_dir() or child.name.startswith("_"):
                continue
            key = str(child.resolve())
            if key in self._seen:
                continue
            self._seen.add(key)
            print(f"[disklordz] new inbox folder: {child.name}")

    def on_midi(self, msg: mido.Message) -> None:
        del msg  # unused in v0
