import pytest

from synth_forge.bin_diff import (
    OffsetMap,
    compare_bytes,
    load_offset_map,
    suggest_fields_from_diff,
)
from synth_forge.synths.minilogue_xd_librarian import MinilogueXdLibrarian


def test_compare_bytes_detects_single_change():
    left = bytes(16)
    right = bytes(16)
    right = right[:4] + bytes([9]) + right[5:]
    changes = compare_bytes(left, right)
    assert len(changes) == 1
    assert changes[0].offset == 4
    suggestions = suggest_fields_from_diff(changes)
    assert suggestions[0].offset == 4


def test_offset_map_roundtrip(tmp_path):
    path = tmp_path / "offsets.json"
    om = OffsetMap(version=1, container="test", fields=[])
    from synth_forge.bin_diff import save_offset_map

    save_offset_map(path, om)
    loaded = load_offset_map(path)
    assert loaded.container == "test"


def test_mnlgxdprog_roundtrip():
    lib = MinilogueXdLibrarian()
    from synth_forge.models import SynthParameters

    params = SynthParameters(filter_cutoff=0.61, category="xd_bass")
    packed = lib.pack_mnlgxdprog(params, "Forge Bass 01")
    restored = lib.unpack_parameters(packed)
    assert restored.filter_cutoff == pytest.approx(params.filter_cutoff, abs=1e-5)
