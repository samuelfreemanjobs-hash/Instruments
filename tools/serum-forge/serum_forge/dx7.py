"""DX7 SysEx is not a Serum ingestion path."""

from __future__ import annotations


class Dx7IncompatibilityError(ValueError):
    pass


def reject_dx7_sysex(data: bytes) -> None:
    if len(data) < 4:
        return
    if data[0:1] == b"\xF0" and len(data) >= 155:
        if data[1:2] == b"\x43":
            raise Dx7IncompatibilityError(
                "Yamaha DX7 SysEx (F0 43 ...) cannot be mapped to Serum. "
                "Use symbolic SerumSymbolPatch → .SerumPreset compilation instead."
            )
