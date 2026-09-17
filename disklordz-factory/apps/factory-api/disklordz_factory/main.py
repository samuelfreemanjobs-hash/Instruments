from __future__ import annotations

from datetime import datetime, timezone
from typing import Annotated

from fastapi import FastAPI, HTTPException
from pydantic import Field

from disklordz_factory.enums import BatchStage
from disklordz_factory.models import (
    ApprovalItem,
    CatalogAsset,
    CatalogAssetCreate,
    FactoryStatus,
    ProductionBatch,
    ProductionBatchCreate,
)

app = FastAPI(
    title="DiskLordz Factory API",
    description="Batch orchestration, catalog, and human approval queue (in-memory dev store).",
    version="0.1.0",
)

_batches: dict[str, ProductionBatch] = {}
_assets: dict[str, CatalogAsset] = {}
_approval: dict[str, ApprovalItem] = {}
_counters = {"batch": 0, "track": 0, "approval_seed": 0}


def _next_id(prefix: str, counter_key: str) -> str:
    _counters[counter_key] += 1
    return f"{prefix}-{_counters[counter_key]:06d}"


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok", "service": "disklordz-factory-api"}


@app.get("/factory/status", response_model=FactoryStatus)
def factory_status() -> FactoryStatus:
    pending = sum(1 for a in _approval.values() if a.status == "pending")
    return FactoryStatus(
        active_batches=len(_batches),
        catalog_assets=len(_assets),
        approval_pending=pending,
    )


@app.post("/batches", response_model=ProductionBatch)
def create_batch(body: ProductionBatchCreate) -> ProductionBatch:
    batch_id = _next_id("DL-BATCH", "batch")
    batch = ProductionBatch(batch_id=batch_id, **body.model_dump())
    _batches[batch_id] = batch
    return batch


@app.get("/batches", response_model=list[ProductionBatch])
def list_batches() -> list[ProductionBatch]:
    return list(_batches.values())


@app.get("/batches/{batch_id}", response_model=ProductionBatch)
def get_batch(batch_id: str) -> ProductionBatch:
    batch = _batches.get(batch_id)
    if not batch:
        raise HTTPException(status_code=404, detail="batch not found")
    return batch


@app.patch("/batches/{batch_id}/stage", response_model=ProductionBatch)
def advance_batch_stage(
    batch_id: str,
    stage: Annotated[BatchStage, Field(description="Next factory OS stage")],
) -> ProductionBatch:
    batch = _batches.get(batch_id)
    if not batch:
        raise HTTPException(status_code=404, detail="batch not found")
    updated = batch.model_copy(update={"stage": stage, "updated_at": datetime.now(timezone.utc)})
    _batches[batch_id] = updated
    return updated


@app.post("/catalog/assets", response_model=CatalogAsset)
def create_asset(body: CatalogAssetCreate) -> CatalogAsset:
    prefix = {
        "track": "DL-TRK",
        "drum": "DL-DRM",
        "808": "DL-808",
        "loop": "DL-LOP",
        "preset": "DL-PRS",
        "fx": "DL-FX",
        "visual": "DL-VIS",
        "video": "DL-VID",
        "midi": "DL-MID",
        "kit": "DL-KIT",
    }[body.kind.value]
    _counters["track"] += 1
    asset_id = f"{prefix}-{_counters['track']:06d}"
    asset = CatalogAsset(asset_id=asset_id, **body.model_dump())
    _assets[asset_id] = asset
    return asset


@app.get("/catalog/assets", response_model=list[CatalogAsset])
def list_assets() -> list[CatalogAsset]:
    return list(_assets.values())


@app.get("/approval-queue", response_model=list[ApprovalItem])
def approval_queue() -> list[ApprovalItem]:
    return [a for a in _approval.values() if a.status == "pending"]


@app.post("/approval-queue/seed-demo", response_model=list[ApprovalItem])
def seed_approval_demo() -> list[ApprovalItem]:
    """Populate demo approval rows for dashboard development."""
    demos = [
        ("DL-TRK-000034", "Track 034 — Dark 90s Digital Phonk"),
        ("DL-VIS-000034", "Artwork 034"),
        ("DL-KIT-000018", "Product 018 — DL-PHONK Starter"),
    ]
    out: list[ApprovalItem] = []
    for asset_id, title in demos:
        item = ApprovalItem(
            asset_id=asset_id,
            title=title,
            audio_qa="pass",
            visual_qa="pass",
            product_qa="pass",
            rights_qa="pass",
        )
        _approval[asset_id] = item
        out.append(item)
    return out


@app.post("/approval-queue/{asset_id}/approve", response_model=ApprovalItem)
def approve_asset(asset_id: str, approved_by: str = "operator") -> ApprovalItem:
    item = _approval.get(asset_id)
    if not item:
        raise HTTPException(status_code=404, detail="not in approval queue")
    updated = item.model_copy(update={"status": "approved"})
    _approval[asset_id] = updated
    return updated


@app.post("/approval-queue/{asset_id}/reject", response_model=ApprovalItem)
def reject_asset(asset_id: str) -> ApprovalItem:
    item = _approval.get(asset_id)
    if not item:
        raise HTTPException(status_code=404, detail="not in approval queue")
    updated = item.model_copy(update={"status": "rejected"})
    _approval[asset_id] = updated
    return updated
