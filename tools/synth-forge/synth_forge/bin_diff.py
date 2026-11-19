"""Binary diff + offset-map helpers for template-based patch forges."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Literal

from pydantic import BaseModel, Field


class OffsetField(BaseModel):
    id: str
    offset: int = Field(ge=0)
    scale: Literal["u8", "u16", "float32", "params_block"] = "u8"
    length: int | None = None
    note: str = ""


class OffsetMap(BaseModel):
    version: int = 1
    container: str = "generic"
    fields: list[OffsetField] = Field(default_factory=list)

    def field_ids(self) -> set[str]:
        return {f.id for f in self.fields}

    def by_id(self, field_id: str) -> OffsetField | None:
        return next((f for f in self.fields if f.id == field_id), None)


class ByteChange(BaseModel):
    offset: int
    before: int
    after: int


def compare_bytes(left: bytes, right: bytes, *, max_results: int = 512) -> list[ByteChange]:
    if len(left) != len(right):
        limit = min(len(left), len(right))
    else:
        limit = len(left)
    changes: list[ByteChange] = []
    for idx in range(limit):
        a, b = left[idx], right[idx]
        if a != b:
            changes.append(ByteChange(offset=idx, before=a, after=b))
            if len(changes) >= max_results:
                break
    return changes


def group_contiguous(changes: list[ByteChange]) -> list[tuple[int, int]]:
    if not changes:
        return []
    ranges: list[tuple[int, int]] = []
    start = changes[0].offset
    end = start
    for ch in changes[1:]:
        if ch.offset == end + 1:
            end = ch.offset
        else:
            ranges.append((start, end))
            start = ch.offset
            end = start
    ranges.append((start, end))
    return ranges


def load_offset_map(path: Path | str) -> OffsetMap:
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    return OffsetMap.model_validate(data)


def save_offset_map(path: Path | str, offset_map: OffsetMap) -> None:
    Path(path).write_text(
        json.dumps(offset_map.model_dump(), indent=2) + "\n",
        encoding="utf-8",
    )


def suggest_fields_from_diff(
    changes: list[ByteChange],
    *,
    prefix: str = "field",
) -> list[OffsetField]:
    suggestions: list[OffsetField] = []
    for idx, (start, end) in enumerate(group_contiguous(changes)):
        span = end - start + 1
        scale: Literal["u8", "u16", "float32", "params_block"]
        if span == 1:
            scale = "u8"
        elif span == 2:
            scale = "u16"
        elif span == 4:
            scale = "float32"
        elif span >= 56:
            scale = "params_block"
        else:
            scale = "u8"
        suggestions.append(
            OffsetField(
                id=f"{prefix}_{start:04x}",
                offset=start,
                scale=scale,
                length=span if scale == "params_block" else None,
                note="auto-suggested from binary diff",
            )
        )
    return suggestions


def float_to_u8(value: float) -> int:
    return int(max(0.0, min(1.0, value)) * 255) & 0xFF


def write_scaled_field(blob: bytearray, field: OffsetField, value_0_1: float) -> None:
    if field.offset >= len(blob):
        raise ValueError(f"offset {field.offset} out of range")
    if field.scale == "u8":
        blob[field.offset] = float_to_u8(value_0_1)
    elif field.scale == "u16":
        raw = float_to_u8(value_0_1) | (float_to_u8(value_0_1) << 8)
        blob[field.offset] = raw & 0xFF
        if field.offset + 1 < len(blob):
            blob[field.offset + 1] = (raw >> 8) & 0xFF
    elif field.scale == "float32":
        import struct

        end = field.offset + 4
        if end > len(blob):
            raise ValueError("float32 field exceeds blob")
        blob[field.offset:end] = struct.pack("<f", float(value_0_1))


def mask_unmapped_regions(blob: bytes, offset_map: OffsetMap) -> bytes:
    """Return a copy with only mapped offsets preserved (for regression tests)."""
    masked = bytearray(len(blob))
    for field in offset_map.fields:
        if field.scale == "params_block":
            length = field.length or 88
            end = min(len(blob), field.offset + length)
            masked[field.offset:end] = blob[field.offset:end]
        elif field.scale == "u8":
            if field.offset < len(blob):
                masked[field.offset] = blob[field.offset]
        elif field.scale == "u16" and field.offset + 1 < len(blob):
            masked[field.offset : field.offset + 2] = blob[field.offset : field.offset + 2]
        elif field.scale == "float32" and field.offset + 4 <= len(blob):
            masked[field.offset : field.offset + 4] = blob[field.offset : field.offset + 4]
    return bytes(masked)
