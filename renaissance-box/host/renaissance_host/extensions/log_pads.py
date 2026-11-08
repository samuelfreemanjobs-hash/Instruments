from __future__ import annotations

import mido


class LogPadsExtension:
    """Log note-on/off for pad discovery."""

    def on_midi(self, msg: mido.Message) -> None:
        if msg.type in ("note_on", "note_off") and getattr(msg, "velocity", 0) != 0:
            print(f"[pad] ch={msg.channel} note={msg.note} vel={msg.velocity}")
