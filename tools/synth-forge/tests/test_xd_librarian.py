import pytest

from synth_forge.generator import generate_batch
from synth_forge.synths.minilogue_xd_librarian import MinilogueXdLibrarian
from synth_forge.xd_profiles import resolve_xd_profile


def test_xd_profile_resolution():
    assert resolve_xd_profile("xd_bass") == "xd_bass"
    assert resolve_xd_profile("sci-fi lead") == "xd_scifi_fx"


def test_batch_mnlgxdprog_export():
    lib = MinilogueXdLibrarian()
    presets = generate_batch("minilogue_xd", 4, category="xd_pad", seed=7)
    archives = [lib.pack_mnlgxdprog(p.parameters, p.name) for p in presets]
    assert all(a[:2] == b"PK" for a in archives)
    for archive, preset in zip(archives, presets):
        restored = lib.unpack_parameters(archive)
        assert restored.filter_cutoff == pytest.approx(preset.parameters.filter_cutoff, abs=1e-4)
