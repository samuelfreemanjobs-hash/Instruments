from __future__ import annotations

from vst_factory import modules


def test_load_product_taxonomy_includes_fx_metering() -> None:
    """Contract: product taxonomy exposes fx.metering for MeterTap."""
    tax = modules.load_product_taxonomy()
    assert tax["schema_version"] == "product-taxonomy/v1"
    ids = [c["id"] for c in tax["categories"]]
    assert "fx.metering" in ids
    metering = next(c for c in tax["categories"] if c["id"] == "fx.metering")
    assert "analysis.peak_rms" in metering["suggested_dsp_modules"]
    assert "gui.meter" in metering["suggested_gui_modules"]


def test_build_spec_applies_taxonomy_modules_for_metertap() -> None:
    """Contract: fx.metering category overrides effect defaults to analysis.peak_rms."""
    spec = modules.build_spec_from_modules(
        plugin_name="MeterTap",
        plugin_class="effect",
        prompt="stereo peak RMS meter dogfood",
        product_category="fx.metering",
    )
    assert spec["schema_version"] == "factory-spec/v2"
    assert spec["product_category"] == "fx.metering"
    assert spec["dsp_modules"] == ["analysis.peak_rms"]
    assert "gui.meter" in spec["gui_modules"]
