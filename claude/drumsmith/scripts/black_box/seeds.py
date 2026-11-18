"""Deterministic seed helpers for Black Box kits (stable across processes)."""

from __future__ import annotations

import zlib


def sku_seed(sku: str, salt: int = 0) -> int:
    """Stable unsigned 32-bit seed from SKU + salt (not Python's salted hash())."""
    return (zlib.crc32(sku.encode("utf-8")) + int(salt)) & 0xFFFFFFFF
