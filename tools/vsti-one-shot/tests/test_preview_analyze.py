import struct
import wave
from pathlib import Path

from vsti_one_shot.preview_analyze import analyze_preview_wav, score_metrics_for_kind


def _write_sine(path: Path, *, decay: float, rate: int = 44100, frames: int = 22050) -> None:
    import math

    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        buf = bytearray()
        for i in range(frames):
            t = i / rate
            env = math.exp(-decay * t)
            s = int(max(-32767, min(32767, 16000 * env * math.sin(2 * math.pi * 440 * t))))
            buf += struct.pack("<h", s)
        w.writeframes(bytes(buf))


def test_stab_prefers_faster_decay(tmp_path: Path):
    fast = tmp_path / "fast.wav"
    slow = tmp_path / "slow.wav"
    _write_sine(fast, decay=8.0)
    _write_sine(slow, decay=0.5)
    fast_score = score_metrics_for_kind(analyze_preview_wav(fast), "stab")
    slow_score = score_metrics_for_kind(analyze_preview_wav(slow), "stab")
    assert fast_score > slow_score


def test_key_prefers_slower_decay(tmp_path: Path):
    fast = tmp_path / "fast.wav"
    slow = tmp_path / "slow.wav"
    _write_sine(fast, decay=8.0)
    _write_sine(slow, decay=0.5)
    fast_score = score_metrics_for_kind(analyze_preview_wav(fast), "key")
    slow_score = score_metrics_for_kind(analyze_preview_wav(slow), "key")
    assert slow_score > fast_score
