from __future__ import annotations

from datetime import datetime, timezone
from typing import Callable

from disklordz_factory.enums import AssetKind, BatchStage, RightsStatus
from disklordz_factory.models import (
    ApprovalItem,
    CatalogAssetCreate,
    NightShiftRequest,
    NightShiftRun,
    NightShiftStep,
    ProductionBatchCreate,
)
from disklordz_factory.research import research_store
from disklordz_factory.store import FactoryStore

# Illustrative schedule from workflows/night_shift.md
NIGHT_SHIFT_SCHEDULE: list[tuple[str, str, BatchStage]] = [
    ("00:00", "Market Intelligence scan", BatchStage.discover),
    ("00:30", "A&R concept selection", BatchStage.ideate),
    ("01:00", "Music Producer briefs", BatchStage.generate),
    ("01:30", "Generation / production queue", BatchStage.produce),
    ("03:00", "Sound Design supporting assets", BatchStage.produce),
    ("04:00", "Sample Miner product candidates", BatchStage.package),
    ("05:00", "Visual Director artwork", BatchStage.package),
    ("05:30", "Content Producer metadata", BatchStage.publish),
    ("06:00", "QA (audio, product, visual, rights)", BatchStage.qa),
    ("06:30", "Publishing queue prepared", BatchStage.publish),
    ("07:00", "Human approval queue ready", BatchStage.publish),
]

_last_run: NightShiftRun | None = None


def get_last_night_shift_run() -> NightShiftRun | None:
    return _last_run


def list_schedule() -> list[NightShiftStep]:
    return [
        NightShiftStep(schedule_time=t, label=label, factory_stage=stage)
        for t, label, stage in NIGHT_SHIFT_SCHEDULE
    ]


def run_night_shift(
    factory: FactoryStore,
    body: NightShiftRequest,
    on_step: Callable[[NightShiftStep], None] | None = None,
) -> NightShiftRun:
    global _last_run
    started = datetime.now(timezone.utc)
    run_id = f"DL-NIGHT-{started.strftime('%Y%m%d-%H%M%S')}"
    steps_completed: list[NightShiftStep] = []
    logs: list[str] = []

    # Artists only when research + A&R greenlit, or HQ explicitly passes artist_ids
    artist_ids = body.artist_ids or research_store.greenlit_artist_ids()
    batch = factory.create_batch(
        ProductionBatchCreate(
            mission=body.mission,
            target_count=body.target_count,
            artist_ids=artist_ids,
        )
    )
    logs.append(f"Created batch {batch.batch_id} for mission: {body.mission}")

    track_count = max(1, body.target_count // 6)
    kit_count = max(1, body.target_count // 12)
    visual_count = track_count

    generated_ids: list[str] = []
    approval_ids: list[str] = []

    for schedule_time, label, stage in NIGHT_SHIFT_SCHEDULE:
        step = NightShiftStep(
            schedule_time=schedule_time,
            label=label,
            factory_stage=stage,
            status="running",
        )
        if on_step:
            on_step(step)

        factory.set_batch_stage(batch.batch_id, stage)
        factory.queue["production"] = min(100, factory.queue["production"] + 8)
        factory.queue["qa"] = min(100, max(0, factory.queue["qa"] + 4))
        logs.append(f"[{schedule_time}] {label}")

        if "Sample Miner" in label:
            for i in range(kit_count):
                artist = (
                    artist_ids[i % len(artist_ids)] if artist_ids else None
                )
                asset = factory.create_asset(
                    CatalogAssetCreate(
                        kind=AssetKind.kit,
                        title=f"Night kit {i + 1} — {body.mission[:40]}",
                        artist_id=artist,
                        batch_id=batch.batch_id,
                        rights_status=RightsStatus.review,
                    )
                )
                generated_ids.append(asset.asset_id)
                factory.today["extracted"] += 1

        if "Visual Director" in label:
            for i in range(visual_count):
                artist = (
                    artist_ids[i % len(artist_ids)] if artist_ids else None
                )
                title_suffix = artist or "unassigned lane"
                asset = factory.create_asset(
                    CatalogAssetCreate(
                        kind=AssetKind.visual,
                        title=f"Artwork {asset_suffix(i)} — {title_suffix}",
                        artist_id=artist,
                        batch_id=batch.batch_id,
                        rights_status=RightsStatus.review,
                    )
                )
                generated_ids.append(asset.asset_id)

        if "Generation" in label:
            for i in range(track_count):
                artist = (
                    artist_ids[i % len(artist_ids)] if artist_ids else None
                )
                asset = factory.create_asset(
                    CatalogAssetCreate(
                        kind=AssetKind.track,
                        title=f"Track {asset_suffix(i)} — {body.mission[:48]}",
                        artist_id=artist,
                        batch_id=batch.batch_id,
                        rights_status=RightsStatus.review,
                    )
                )
                generated_ids.append(asset.asset_id)
                factory.today["generated"] += 1

        if "QA" in label:
            factory.queue["qa"] = min(100, factory.queue["qa"] + 25)
            factory.queue["production"] = max(0, factory.queue["production"] - 20)

        if "Human approval" in label:
            factory.queue["publishing"] = min(100, factory.queue["publishing"] + 40)
            for asset_id in generated_ids:
                if asset_id.startswith("DL-TRK") or asset_id.startswith("DL-VIS") or asset_id.startswith(
                    "DL-KIT"
                ):
                    asset = factory.assets[asset_id]
                    item = ApprovalItem(
                        asset_id=asset_id,
                        title=asset.title,
                        audio_qa="pass" if asset_id.startswith("DL-TRK") else "n/a",
                        visual_qa="pass" if asset_id.startswith("DL-VIS") else "n/a",
                        product_qa="pass" if asset_id.startswith("DL-KIT") else "n/a",
                        rights_qa="pass",
                        status="pending",
                    )
                    factory.upsert_approval(item)
                    approval_ids.append(asset_id)

        step.status = "done"
        steps_completed.append(step)

    factory.append_batch_assets(batch.batch_id, generated_ids)
    finished = datetime.now(timezone.utc)
    run = NightShiftRun(
        run_id=run_id,
        batch_id=batch.batch_id,
        mission=body.mission,
        status="awaiting_human_approval",
        started_at=started,
        finished_at=finished,
        steps=steps_completed,
        assets_created=len(generated_ids),
        approval_items=len(approval_ids),
        logs=logs,
    )
    _last_run = run
    return run


def asset_suffix(index: int) -> str:
    return f"{index + 1:03d}"
