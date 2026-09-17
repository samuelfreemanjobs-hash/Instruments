from __future__ import annotations

import os

import pytest
from fastapi.testclient import TestClient

os.environ["FACTORY_SKIP_RESEARCH_BOOTSTRAP"] = "1"

from disklordz_factory.main import app  # noqa: E402
from disklordz_factory.prompt_validation import load_fixture, validate_all_fixtures  # noqa: E402
from disklordz_factory.store import store  # noqa: E402


@pytest.fixture(autouse=True)
def _reset_supply_state() -> None:
    store.supply_weekly_drum = {
        "last_iso_week": None,
        "last_run_id": None,
        "last_kit_asset_id": None,
        "rotation_index": 0,
    }
    yield


def test_compiled_prompt_gold_fixtures_validate() -> None:
    results = validate_all_fixtures()
    assert set(results.keys()) == {"DL001", "DL002", "DL004", "DL006"}
    for artist_id, errors in results.items():
        assert errors == [], f"{artist_id}: {errors}"


@pytest.mark.parametrize("artist_id", ["DL001", "DL002", "DL004", "DL006"])
def test_fixture_loads(artist_id: str) -> None:
    doc = load_fixture(artist_id)
    assert doc["artist_id"] == artist_id


def test_validate_endpoint_accepts_gold_fixture() -> None:
    with TestClient(app) as client:
        body = load_fixture("DL001")
        r = client.post("/validate/compiled-prompt", json=body)
        assert r.status_code == 200
        assert r.json()["valid"] is True


def test_weekly_drum_kit_independent_run() -> None:
    with TestClient(app) as client:
        status = client.get("/supply/weekly-drum-kit/status")
        assert status.status_code == 200
        data = status.json()
        assert data["independent_of_collective"] is True
        assert data["due_this_week"] is True

        run = client.post("/supply/weekly-drum-kit/run")
        assert run.status_code == 200
        payload = run.json()
        assert payload["skipped"] is False
        assert payload["kit_asset_id"].startswith("DL-KIT")
        assert len(payload["drum_asset_ids"]) == 8
        assert payload["mission"] == "SUPPLY-WEEKLY-DRUM"

        run2 = client.post("/supply/weekly-drum-kit/run")
        assert run2.json()["skipped"] is True

        run3 = client.post("/supply/weekly-drum-kit/run?force=true")
        assert run3.json()["skipped"] is False

        queue = client.get("/approval-queue")
        kit_ids = [a["asset_id"] for a in queue.json() if a["asset_id"].startswith("DL-KIT")]
        assert payload["kit_asset_id"] in kit_ids
