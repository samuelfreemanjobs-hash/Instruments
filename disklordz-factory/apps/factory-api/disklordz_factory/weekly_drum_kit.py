from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from pydantic import BaseModel, Field

from disklordz_factory.enums import AssetKind, BatchStage, RightsStatus
from disklordz_factory.models import ApprovalItem, CatalogAssetCreate, ProductionBatchCreate
from disklordz_factory.store import FactoryStore

_FACTORY_ROOT = Path(__file__).resolve().parents[3]
ROTATION_PATH = _FACTORY_ROOT / "database" / "supply_weekly_drum_rotation.json"

SUPPLY_WEEKLY_MISSION = "SUPPLY-WEEKLY-DRUM"
DRUM_ONE_SHOTS_PER_KIT = 8


class WeeklyDrumKitStatus(BaseModel):
    mission: str = SUPPLY_WEEKLY_MISSION
    cadence: str = "weekly"
    kits_per_run: int = 1
    iso_year: int
    iso_week: int
    due_this_week: bool
    rotation_index: int
    rotation_length: int
    next_machine: dict[str, Any] | None = None
    last_run_id: str | None = None
    last_run_week: str | None = None
    last_kit_asset_id: str | None = None
    independent_of_collective: bool = True


class WeeklyDrumKitRun(BaseModel):
    run_id: str
    batch_id: str
    iso_year: int
    iso_week: int
    mission: str
    machine_id: str
    kit_asset_id: str
    drum_asset_ids: list[str] = Field(default_factory=list)
    skipped: bool = False
    skip_reason: str | None = None
    started_at: datetime
    finished_at: datetime | None = None


def _utc_now() -> datetime:
    return datetime.now(timezone.utc)


def _iso_week_key(dt: datetime | None = None) -> tuple[int, int, str]:
    dt = dt or _utc_now()
    iso = dt.isocalendar()
    return iso.year, iso.week, f"{iso.year}-W{iso.week:02d}"


def load_rotation_config() -> dict[str, Any]:
    return json.loads(ROTATION_PATH.read_text(encoding="utf-8"))


def weekly_drum_status(store: FactoryStore) -> WeeklyDrumKitStatus:
    cfg = load_rotation_config()
    machines = cfg.get("machine_rotation") or []
    year, week, week_key = _iso_week_key()
    supply = store.supply_weekly_drum
    last_week = supply.get("last_iso_week")
    due = last_week != week_key
    idx = int(supply.get("rotation_index", 0)) % max(1, len(machines))
    next_machine = machines[idx] if machines else None
    return WeeklyDrumKitStatus(
        iso_year=year,
        iso_week=week,
        due_this_week=due,
        rotation_index=idx,
        rotation_length=len(machines),
        kits_per_run=int(cfg.get("kits_per_run", 1)),
        next_machine=next_machine,
        last_run_id=supply.get("last_run_id"),
        last_run_week=last_week,
        last_kit_asset_id=supply.get("last_kit_asset_id"),
    )


def run_weekly_drum_kit(
    store: FactoryStore,
    *,
    force: bool = False,
) -> WeeklyDrumKitRun:
    started = _utc_now()
    year, week, week_key = _iso_week_key(started)
    run_id = f"DL-SUPPLY-{started.strftime('%Y%m%d-%H%M%S')}"
    supply = store.supply_weekly_drum

    if not force and supply.get("last_iso_week") == week_key:
        return WeeklyDrumKitRun(
            run_id=run_id,
            batch_id="",
            iso_year=year,
            iso_week=week,
            mission=SUPPLY_WEEKLY_MISSION,
            machine_id="",
            kit_asset_id="",
            skipped=True,
            skip_reason=f"Already produced for {week_key}; use force=true to override",
            started_at=started,
            finished_at=_utc_now(),
        )

    cfg = load_rotation_config()
    machines = cfg.get("machine_rotation") or []
    if not machines:
        raise ValueError("supply_weekly_drum_rotation.json has empty machine_rotation")

    idx = int(supply.get("rotation_index", 0)) % len(machines)
    machine = machines[idx]
    machine_id = str(machine.get("machine_id", "DL-808"))
    title_suffix = str(machine.get("title_suffix", "Weekly Kit"))
    tags = machine.get("tags") or []

    batch = store.create_batch(
        ProductionBatchCreate(
            mission=SUPPLY_WEEKLY_MISSION,
            target_count=1,
            artist_ids=[],
        )
    )
    store.set_batch_stage(batch.batch_id, BatchStage.produce)

    kit_title = f"Week {week_key} — {machine_id} {title_suffix}"
    kit = store.create_asset(
        CatalogAssetCreate(
            kind=AssetKind.kit,
            title=kit_title,
            batch_id=batch.batch_id,
            rights_status=RightsStatus.review,
        )
    )

    drum_ids: list[str] = []
    for i in range(DRUM_ONE_SHOTS_PER_KIT):
        tag = tags[i % len(tags)] if tags else "one-shot"
        drum = store.create_asset(
            CatalogAssetCreate(
                kind=AssetKind.drum,
                title=f"{machine_id} — {tag} {i + 1:02d}",
                batch_id=batch.batch_id,
                rights_status=RightsStatus.review,
            )
        )
        drum_ids.append(drum.asset_id)

    store.append_batch_assets(batch.batch_id, [kit.asset_id, *drum_ids])
    store.upsert_approval(
        ApprovalItem(
            asset_id=kit.asset_id,
            title=kit.title,
            audio_qa="pass",
            visual_qa="n/a",
            product_qa="pass",
            rights_qa="pass",
            status="pending",
        )
    )
    store.today["extracted"] += 1
    store.queue["publishing"] = min(100, store.queue["publishing"] + 10)

    supply["last_iso_week"] = week_key
    supply["last_run_id"] = run_id
    supply["last_kit_asset_id"] = kit.asset_id
    supply["rotation_index"] = (idx + 1) % len(machines)

    finished = _utc_now()
    return WeeklyDrumKitRun(
        run_id=run_id,
        batch_id=batch.batch_id,
        iso_year=year,
        iso_week=week,
        mission=SUPPLY_WEEKLY_MISSION,
        machine_id=machine_id,
        kit_asset_id=kit.asset_id,
        drum_asset_ids=drum_ids,
        started_at=started,
        finished_at=finished,
    )
