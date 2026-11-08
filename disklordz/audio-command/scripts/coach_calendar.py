#!/usr/bin/env python3
"""Write coach-daily.ics — subscribe on phone for passive reminders."""

from __future__ import annotations

import argparse
import sys
from datetime import datetime, timedelta, timezone
from pathlib import Path

_SCRIPTS = Path(__file__).resolve().parent
if str(_SCRIPTS) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS))

from coach_engine import format_outreach_markdown, next_task

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "exports" / "coach-daily.ics"


def _escape(s: str) -> str:
    return s.replace("\\", "\\\\").replace("\n", "\\n").replace(",", "\\,")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hour", type=int, default=9, help="Local reminder hour (UTC in file)")
    parser.add_argument("--write", action="store_true", help=f"Write {OUT}")
    args = parser.parse_args()

    task = next_task()
    summary = task["headline"] if task else "DiskLordz weekly review"
    desc = format_outreach_markdown(task)

    start = datetime.now(timezone.utc).replace(hour=args.hour, minute=0, second=0, microsecond=0)
    if start < datetime.now(timezone.utc):
        start += timedelta(days=1)
    end = start + timedelta(minutes=15)
    fmt = "%Y%m%dT%H%M%SZ"
    stamp = datetime.now(timezone.utc).strftime(fmt)

    ics = f"""BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//DiskLordz Audio//Coach//EN
CALSCALE:GREGORIAN
BEGIN:VEVENT
UID:disklordz-coach-daily@audio-command
DTSTAMP:{stamp}
DTSTART:{start.strftime(fmt)}
DTEND:{end.strftime(fmt)}
RRULE:FREQ=DAILY
SUMMARY:{_escape(summary)}
DESCRIPTION:{_escape(desc)}
END:VEVENT
END:VCALENDAR
"""

    if args.write:
        OUT.parent.mkdir(parents=True, exist_ok=True)
        OUT.write_text(ics, encoding="utf-8")
        print(f"wrote {OUT}")
    else:
        print(ics)


if __name__ == "__main__":
    main()
