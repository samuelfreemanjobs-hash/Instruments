from pathlib import Path

from vsti_one_shot.job_validator import validate_job


def test_validate_job_rejects_empty_renders():
    errs = validate_job({"version": 1, "output_dir": "/tmp", "defaults": {}, "renders": []})
    assert any("renders" in e for e in errs)


def test_validate_job_accepts_minimal(tmp_path: Path):
    vst3 = tmp_path / "Fake.vst3"
    vst3.mkdir()
    job = {
        "version": 1,
        "output_dir": str(tmp_path / "out"),
        "defaults": {
            "sample_rate": 44100,
            "velocity": 100,
            "channels": "mono",
            "normalize_peak_db": -1.0,
            "tail_silence_db": -60.0,
            "max_capture_sec": 8.0,
        },
        "renders": [
            {
                "slot_id": "test_001",
                "vst3_path": str(vst3),
                "program": 0,
                "root_midi": 60,
            }
        ],
    }
    assert validate_job(job) == []
