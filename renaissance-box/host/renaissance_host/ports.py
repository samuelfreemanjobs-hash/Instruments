from __future__ import annotations

import mido


def list_io_names() -> tuple[list[str], list[str]]:
    return mido.get_input_names(), mido.get_output_names()


def pick_port(
    names: list[str],
    substring: str,
    fallback_index: int | None,
) -> str | None:
    sub = substring.lower()
    for name in names:
        if sub in name.lower():
            return name
    if fallback_index is not None and 0 <= fallback_index < len(names):
        return names[fallback_index]
    return None
