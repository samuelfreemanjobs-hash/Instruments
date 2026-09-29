"""Robots-aware download helpers for royalty-free phonk one-shots."""

from __future__ import annotations

import json
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass, asdict
from pathlib import Path
from urllib.robotparser import RobotFileParser


DEFAULT_USER_AGENT = "DisklordzPhonkIngest/1.0 (+https://github.com/samuelfreemanjobs-hash/Instruments; research ingest)"


@dataclass
class DownloadRecord:
    url: str
    path: str
    bytes: int
    license_note: str


def _origin(url: str) -> str:
    p = urllib.parse.urlparse(url)
    return f"{p.scheme}://{p.netloc}"


def robots_allows(url: str, user_agent: str = DEFAULT_USER_AGENT) -> bool:
    """Return True if robots.txt permits GET on url for this agent."""
    origin = _origin(url)
    rp = RobotFileParser()
    rp.set_url(urllib.parse.urljoin(origin, "/robots.txt"))
    try:
        rp.read()
    except (urllib.error.URLError, OSError):
        return False
    return rp.can_fetch(user_agent, url)


def download_file(
    url: str,
    dest: Path,
    *,
    user_agent: str = DEFAULT_USER_AGENT,
    min_interval_sec: float = 1.5,
    last_fetch_at: list[float] | None = None,
    license_note: str = "operator must confirm royalty-free license",
) -> DownloadRecord:
    """
    Fetch a single file URL if robots.txt allows it.

    Rate-limited; does not bypass auth or paywalls.
    """
    if not robots_allows(url, user_agent):
        raise PermissionError(f"robots.txt disallows fetch: {url}")

    if last_fetch_at is not None and last_fetch_at[0] > 0:
        elapsed = time.time() - last_fetch_at[0]
        if elapsed < min_interval_sec:
            time.sleep(min_interval_sec - elapsed)

    dest.parent.mkdir(parents=True, exist_ok=True)
    req = urllib.request.Request(url, headers={"User-Agent": user_agent})
    with urllib.request.urlopen(req, timeout=60) as resp:
        data = resp.read()
    dest.write_bytes(data)
    if last_fetch_at is not None:
        last_fetch_at[0] = time.time()
    return DownloadRecord(url=url, path=str(dest), bytes=len(data), license_note=license_note)


def download_url_list(
    urls: list[str],
    output_dir: Path,
    *,
    manifest_path: Path | None = None,
    license_note: str = "operator confirmed royalty-free",
) -> list[DownloadRecord]:
    output_dir.mkdir(parents=True, exist_ok=True)
    last: list[float] = [0.0]
    records: list[DownloadRecord] = []
    for i, url in enumerate(urls):
        name = Path(urllib.parse.urlparse(url).path).name or f"sample_{i:04d}.wav"
        if not name.lower().endswith(".wav"):
            name = f"{name}.wav"
        rec = download_file(
            url,
            output_dir / name,
            last_fetch_at=last,
            license_note=license_note,
        )
        records.append(rec)
    out_manifest = manifest_path or (output_dir / "download_manifest.json")
    out_manifest.write_text(json.dumps([asdict(r) for r in records], indent=2), encoding="utf-8")
    return records
