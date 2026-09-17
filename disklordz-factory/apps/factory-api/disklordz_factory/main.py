from __future__ import annotations

from datetime import datetime, timezone
from typing import Annotated

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import Field

from disklordz_factory.enums import BatchStage
from disklordz_factory.models import (
    ApprovalItem,
    CatalogAsset,
    CatalogAssetCreate,
    FactoryDashboard,
    FactoryStatus,
    NightShiftRequest,
    NightShiftRun,
    NightShiftStep,
    ProductionBatch,
    ProductionBatchCreate,
)
from disklordz_factory import night_shift
from disklordz_factory import slack_agents
from disklordz_factory.research import (
    AARDecision,
    AARDecisionIn,
    MarketOpportunity,
    MarketOpportunityIn,
    ResearchStatus,
    research_store,
)
from disklordz_factory.artist_agents import build_artist_agent_profile, list_artist_agent_profiles
from disklordz_factory.collective import load_collective_manifest, list_collective_artists
from disklordz_factory.research_bootstrap import bootstrap_research_if_empty, greenlights_summary
from disklordz_factory.store import store

app = FastAPI(
    title="DiskLordz Factory API",
    description="Batch orchestration, night shift, catalog, and human approval queue.",
    version="0.2.0",
)

@app.on_event("startup")
def _bootstrap_hq_greenlights() -> None:
    import os

    if os.environ.get("FACTORY_SKIP_RESEARCH_BOOTSTRAP", "").strip().lower() in (
        "1",
        "true",
        "yes",
    ):
        return
    bootstrap_research_if_empty()


app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://127.0.0.1:5173",
        "http://localhost:5173",
        "http://127.0.0.1:4173",
        "http://localhost:4173",
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok", "service": "disklordz-factory-api"}


@app.get("/collective/manifest")
def collective_manifest() -> dict:
    return load_collective_manifest()


@app.get("/collective/artists")
def collective_artists() -> list[dict]:
    return list_collective_artists()


@app.get("/collective/artist-agents")
def collective_artist_agents(include_prompts: bool = False) -> list[dict]:
    profiles = list_artist_agent_profiles()
    if not include_prompts:
        compact: list[dict] = []
        for p in profiles:
            row = dict(p)
            row.pop("agent_contract", None)
            row.pop("system_prompt", None)
            compact.append(row)
        return compact
    return profiles


@app.get("/collective/artist-agents/{artist_id}")
def collective_artist_agent(artist_id: str) -> dict:
    profile = build_artist_agent_profile(artist_id.upper())
    if not profile:
        raise HTTPException(status_code=404, detail="artist agent not found or not greenlit")
    return profile


@app.get("/factory/status", response_model=FactoryStatus)
def factory_status() -> FactoryStatus:
    pending = sum(1 for a in store.approval.values() if a.status == "pending")
    return FactoryStatus(
        active_batches=len(store.batches),
        catalog_assets=len(store.assets),
        approval_pending=pending,
    )


@app.get("/factory/dashboard", response_model=FactoryDashboard)
def factory_dashboard() -> FactoryDashboard:
    return store.dashboard()


@app.post("/batches", response_model=ProductionBatch)
def create_batch(body: ProductionBatchCreate) -> ProductionBatch:
    return store.create_batch(body)


@app.get("/batches", response_model=list[ProductionBatch])
def list_batches() -> list[ProductionBatch]:
    return list(store.batches.values())


@app.get("/batches/{batch_id}", response_model=ProductionBatch)
def get_batch(batch_id: str) -> ProductionBatch:
    batch = store.batches.get(batch_id)
    if not batch:
        raise HTTPException(status_code=404, detail="batch not found")
    return batch


@app.patch("/batches/{batch_id}/stage", response_model=ProductionBatch)
def advance_batch_stage(
    batch_id: str,
    stage: Annotated[BatchStage, Field(description="Next factory OS stage")],
) -> ProductionBatch:
    if batch_id not in store.batches:
        raise HTTPException(status_code=404, detail="batch not found")
    return store.set_batch_stage(batch_id, stage)


@app.post("/catalog/assets", response_model=CatalogAsset)
def create_asset(body: CatalogAssetCreate) -> CatalogAsset:
    return store.create_asset(body)


@app.get("/catalog/assets", response_model=list[CatalogAsset])
def list_assets() -> list[CatalogAsset]:
    return list(store.assets.values())


@app.get("/approval-queue", response_model=list[ApprovalItem])
def approval_queue() -> list[ApprovalItem]:
    return [a for a in store.approval.values() if a.status == "pending"]


@app.post("/approval-queue/seed-demo", response_model=list[ApprovalItem])
def seed_approval_demo() -> list[ApprovalItem]:
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
        store.upsert_approval(item)
        out.append(item)
    return out


@app.post("/approval-queue/{asset_id}/approve", response_model=ApprovalItem)
def approve_asset(asset_id: str, approved_by: str = "operator") -> ApprovalItem:
    item = store.approval.get(asset_id)
    if not item:
        raise HTTPException(status_code=404, detail="not in approval queue")
    updated = item.model_copy(update={"status": "approved"})
    store.upsert_approval(updated)
    store.today["approved"] += 1
    store.queue["publishing"] = max(0, store.queue["publishing"] - 5)
    slack_agents.notify_approval(asset_id, item.title)
    return updated


@app.post("/approval-queue/{asset_id}/reject", response_model=ApprovalItem)
def reject_asset(asset_id: str) -> ApprovalItem:
    item = store.approval.get(asset_id)
    if not item:
        raise HTTPException(status_code=404, detail="not in approval queue")
    updated = item.model_copy(update={"status": "rejected"})
    store.upsert_approval(updated)
    return updated


@app.get("/night-shift/schedule", response_model=list[NightShiftStep])
def night_shift_schedule() -> list[NightShiftStep]:
    return night_shift.list_schedule()


@app.get("/night-shift/runs/latest", response_model=NightShiftRun | None)
def night_shift_latest() -> NightShiftRun | None:
    return night_shift.get_last_night_shift_run()


@app.get("/research/status", response_model=ResearchStatus)
def research_status() -> ResearchStatus:
    return research_store.status()


@app.get("/research/greenlights")
def research_greenlights() -> dict:
    """HQ locked roster: four artists + YouTube brands + live store linkage."""
    return greenlights_summary()


@app.post("/research/opportunities", response_model=MarketOpportunity)
def research_add_opportunity(body: MarketOpportunityIn) -> MarketOpportunity:
    return research_store.add_opportunity(body)


@app.get("/research/opportunities", response_model=list[MarketOpportunity])
def research_list_opportunities() -> list[MarketOpportunity]:
    return research_store.list_opportunities()


@app.post("/research/aar-decisions", response_model=AARDecision)
def research_aar_decision(body: AARDecisionIn) -> AARDecision:
    try:
        return research_store.record_aar_decision(body)
    except KeyError:
        raise HTTPException(status_code=404, detail="opportunity not found") from None
    except ValueError as e:
        raise HTTPException(status_code=422, detail=str(e)) from e


@app.post("/night-shift/run", response_model=NightShiftRun)
def night_shift_run(body: NightShiftRequest | None = None, notify_slack: bool = True) -> NightShiftRun:
    req = body or NightShiftRequest()
    run = night_shift.run_night_shift(store, req)
    if notify_slack:
        slack_agents.notify_night_shift_slack(run, req)
    return run


@app.post("/factory/slack/checkin")
def factory_slack_checkin() -> dict:
    """Post all factory agents to Slack (stand-up / verify webhook)."""
    return slack_agents.notify_agent_checkin()


@app.get("/factory/slack/preview")
def factory_slack_preview() -> dict:
    """Dry-run Slack payloads without sending (dev)."""
    req = NightShiftRequest()
    fake_run = night_shift.get_last_night_shift_run()
    return {
        "checkin": slack_agents.build_agent_checkin_blocks(),
        "night_shift": slack_agents.build_night_shift_slack_blocks(
            fake_run,
            req,
        )
        if fake_run
        else None,
        "webhook_configured": slack_agents.slack_webhook_url() is not None,
    }
