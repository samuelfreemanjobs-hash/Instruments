from pathlib import Path

from renaissance_host.config import load_box_config, load_midi_map


def test_load_windows_box_config():
    root = Path(__file__).resolve().parents[2]
    cfg = load_box_config(root / "config" / "box.windows.json")
    assert cfg.mode == "sidecar"
    assert cfg.midi.input_port_substring == "Renaissance"
    assert cfg.midi_map_path.is_file()


def test_load_midi_map_template():
    root = Path(__file__).resolve().parents[2]
    m = load_midi_map(root / "config" / "midi-map.template.json")
    assert len(m["pads"]) == 16
    assert m["verifiedOnHardware"] is False
