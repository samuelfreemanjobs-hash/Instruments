from __future__ import annotations

from vst_factory.portfolio import load_product_library, merge_with_db_projects


def test_load_product_library_includes_metertap() -> None:
    """Contract: product library JSON includes MeterTap dogfood SKU."""
    lib = load_product_library()
    assert lib["schema_version"] == "product-library/v1"
    names = [p["plugin_name"] for p in lib["products"]]
    assert "MeterTap" in names


def test_merge_with_db_projects_overlays_factory_state() -> None:
    """Contract: merge_with_db_projects attaches factory_project for matching names."""
    lib = load_product_library()
    projects = [
        {
            "id": 42,
            "plugin_name": "MeterTap",
            "plugin_class": "effect",
            "state": "dream",
            "apc_path": "plugins/MeterTap",
            "vst_path": None,
            "spec": {"product_category": "fx.metering", "track": "apc_greenfield"},
        }
    ]
    products = merge_with_db_projects(lib, projects)
    mt = next(p for p in products if p["plugin_name"] == "MeterTap")
    assert mt["factory_project"]["id"] == 42
    assert mt["factory_project"]["state"] == "dream"
    assert mt["source"] == "library+db"


def test_merge_with_db_projects_includes_db_only_skus() -> None:
    """Contract: DB projects absent from library still appear in portfolio products."""
    lib = {"schema_version": "product-library/v1", "products": []}
    projects = [
        {
            "id": 7,
            "plugin_name": "SoloDBOnly",
            "plugin_class": "synth",
            "state": "intake",
            "apc_path": None,
            "vst_path": None,
            "spec": {},
        }
    ]
    products = merge_with_db_projects(lib, projects)
    assert any(p["plugin_name"] == "SoloDBOnly" and p["source"] == "db" for p in products)
