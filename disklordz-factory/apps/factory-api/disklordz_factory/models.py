from datetime import datetime, timezone
from typing import Optional

from pydantic import BaseModel, Field

from disklordz_factory.enums import AssetKind, BatchStage, RightsStatus


def _utc_now() -> datetime:
    return datetime.now(timezone.utc)


class FactoryStatus(BaseModel):
    active_batches: int = 0
    catalog_assets: int = 0
    approval_pending: int = 0
    stages: list[str] = Field(default_factory=lambda: [s.value for s in BatchStage])


class FactoryDashboard(BaseModel):
    """Operator dashboard aggregates."""

    active_batches: int = 0
    catalog_assets: int = 0
    approval_pending: int = 0
    active_projects: int = 0
    active_eps: int = 0
    active_products: int = 0
    today_generated: int = 0
    today_approved: int = 0
    today_extracted: int = 0
    catalog_audio_assets: int = 0
    catalog_tracks: int = 0
    catalog_products: int = 0
    queue_production: int = Field(0, ge=0, le=100)
    queue_qa: int = Field(0, ge=0, le=100)
    queue_publishing: int = Field(0, ge=0, le=100)
    revenue_total_usd: float = 0.0
    revenue_tracks_usd: float = 0.0
    revenue_products_usd: float = 0.0


class ProductionBatchCreate(BaseModel):
    mission: str = Field(..., min_length=3, examples=["Develop dark Detroit electro/phonk product line"])
    target_count: int = Field(25, ge=1, le=500)
    artist_ids: list[str] = Field(default_factory=list)


class ProductionBatch(ProductionBatchCreate):
    batch_id: str
    stage: BatchStage = BatchStage.discover
    asset_ids: list[str] = Field(default_factory=list)
    created_at: datetime = Field(default_factory=_utc_now)
    updated_at: datetime = Field(default_factory=_utc_now)


class ApprovalItem(BaseModel):
    asset_id: str
    title: str
    audio_qa: str = "pending"
    visual_qa: str = "pending"
    product_qa: str = "pending"
    rights_qa: str = "pending"
    status: str = "pending"


class CatalogAssetCreate(BaseModel):
    kind: AssetKind
    title: str
    artist_id: Optional[str] = None
    batch_id: Optional[str] = None
    rights_status: RightsStatus = RightsStatus.draft


class CatalogAsset(CatalogAssetCreate):
    asset_id: str


class NightShiftRequest(BaseModel):
    mission: str = Field(
        default="Autonomous night shift batch — market-selected phonk/electro line",
        min_length=3,
    )
    target_count: int = Field(25, ge=1, le=500)
    artist_ids: list[str] = Field(default_factory=list)


class NightShiftStep(BaseModel):
    schedule_time: str
    label: str
    factory_stage: BatchStage
    status: str = "pending"


class NightShiftRun(BaseModel):
    run_id: str
    batch_id: str
    mission: str
    status: str
    started_at: datetime
    finished_at: datetime | None = None
    steps: list[NightShiftStep] = Field(default_factory=list)
    assets_created: int = 0
    approval_items: int = 0
    logs: list[str] = Field(default_factory=list)
