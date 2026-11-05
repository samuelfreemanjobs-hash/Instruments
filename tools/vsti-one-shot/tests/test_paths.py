from pathlib import Path

from vsti_one_shot.paths import resolve_render_bin


def test_resolve_render_bin_override(tmp_path: Path):
    fake = tmp_path / "custom.exe"
    fake.write_bytes(b"")
    assert resolve_render_bin(tmp_path, fake) == fake
