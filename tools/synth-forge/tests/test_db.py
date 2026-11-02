from pathlib import Path

from synth_forge.db import SynthMemory
from synth_forge.models import BatchRun, Preset, SynthParameters


def test_save_and_list_presets(tmp_path: Path):
    db = SynthMemory(tmp_path / "test.db")
    preset = Preset(name="test", synth_id="dx7", parameters=SynthParameters())
    db.save_preset(preset)
    listed = db.list_presets()
    assert len(listed) == 1
    assert listed[0].id == preset.id
    db.close()


def test_set_rating(tmp_path: Path):
    db = SynthMemory(tmp_path / "test.db")
    preset = Preset(name="rate", synth_id="dx7", parameters=SynthParameters())
    db.save_preset(preset)
    assert db.set_rating(preset.id, "favorite")
    listed = db.list_presets()[0]
    assert listed.rating == "favorite"
    db.close()


def test_save_batch(tmp_path: Path):
    db = SynthMemory(tmp_path / "test.db")
    batch = BatchRun(synth_id="minilogue_xd", count=2, preset_ids=["a", "b"])
    db.save_batch(batch)
    db.close()
