from __future__ import annotations

from datetime import datetime, timezone

from disklordz_factory.enums import AssetKind, BatchStage, RightsStatus
from disklordz_factory.models import (
    ApprovalItem,
    CatalogAsset,
    CatalogAssetCreate,
    FactoryDashboard,
    ProductionBatch,
    ProductionBatchCreate,
)


class FactoryStore:
    """In-memory catalog + batch state (dev); replace with SQLite/Postgres."""

    def __init__(self) -> None:
        self.batches: dict[str, ProductionBatch] = {}
        self.assets: dict[str, CatalogAsset] = {}
        self.approval: dict[str, ApprovalItem] = {}
        self.counters = {"batch": 0, "asset": 0}
        self.today = {"generated": 0, "approved": 0, "extracted": 0}
        self.queue = {"production": 0, "qa": 0, "publishing": 0}

    def _next_id(self, prefix: str, counter_key: str) -> str:
        self.counters[counter_key] += 1
        return f"{prefix}-{self.counters[counter_key]:06d}"

    def create_batch(self, body: ProductionBatchCreate) -> ProductionBatch:
        batch_id = self._next_id("DL-BATCH", "batch")
        batch = ProductionBatch(batch_id=batch_id, **body.model_dump())
        self.batches[batch_id] = batch
        return batch

    def set_batch_stage(self, batch_id: str, stage: BatchStage) -> ProductionBatch:
        batch = self.batches[batch_id]
        updated = batch.model_copy(update={"stage": stage, "updated_at": datetime.now(timezone.utc)})
        self.batches[batch_id] = updated
        return updated

    def append_batch_assets(self, batch_id: str, asset_ids: list[str]) -> ProductionBatch:
        batch = self.batches[batch_id]
        merged = list(dict.fromkeys([*batch.asset_ids, *asset_ids]))
        updated = batch.model_copy(
            update={"asset_ids": merged, "updated_at": datetime.now(timezone.utc)}
        )
        self.batches[batch_id] = updated
        return updated

    def create_asset(self, body: CatalogAssetCreate) -> CatalogAsset:
        prefix = {
            AssetKind.track: "DL-TRK",
            AssetKind.drum: "DL-DRM",
            AssetKind.eight_oh_eight: "DL-808",
            AssetKind.loop: "DL-LOP",
            AssetKind.preset: "DL-PRS",
            AssetKind.fx: "DL-FX",
            AssetKind.visual: "DL-VIS",
            AssetKind.video: "DL-VID",
            AssetKind.midi: "DL-MID",
            AssetKind.kit: "DL-KIT",
        }[body.kind]
        asset_id = self._next_id(prefix, "asset")
        asset = CatalogAsset(asset_id=asset_id, **body.model_dump())
        self.assets[asset_id] = asset
        return asset

    def upsert_approval(self, item: ApprovalItem) -> ApprovalItem:
        self.approval[item.asset_id] = item
        return item

    def dashboard(self) -> FactoryDashboard:
        pending = sum(1 for a in self.approval.values() if a.status == "pending")
        active_projects = len(self.batches)
        tracks = sum(1 for a in self.assets.values() if a.kind == AssetKind.track)
        products = sum(
            1
            for a in self.assets.values()
            if a.kind in (AssetKind.kit, AssetKind.drum, AssetKind.eight_oh_eight, AssetKind.preset)
        )
        return FactoryDashboard(
            active_batches=active_projects,
            catalog_assets=len(self.assets),
            approval_pending=pending,
            active_projects=active_projects,
            active_eps=max(0, active_projects // 4),
            active_products=products,
            today_generated=self.today["generated"],
            today_approved=self.today["approved"],
            today_extracted=self.today["extracted"],
            catalog_audio_assets=len(self.assets),
            catalog_tracks=tracks,
            catalog_products=products,
            queue_production=self.queue["production"],
            queue_qa=self.queue["qa"],
            queue_publishing=self.queue["publishing"],
        )


store = FactoryStore()
