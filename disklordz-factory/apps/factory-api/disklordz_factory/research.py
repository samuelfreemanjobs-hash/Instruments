from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from pydantic import BaseModel, Field, field_validator

MIN_SOURCES = 1
MIN_SCORE_FOR_AAR = 50


class MarketOpportunityIn(BaseModel):
    title: str = Field(..., min_length=3)
    genre: str = Field(..., min_length=2)
    subgenre: str | None = None
    bpm_min: float | None = None
    bpm_max: float | None = None
    aesthetic: str | None = None
    audience: str | None = None
    use_case: str | None = None
    search_demand: str | None = None
    competition: str | None = None
    product_ideas: list[str] = Field(default_factory=list)
    sources: list[str] = Field(..., min_length=MIN_SOURCES, description="Evidence URLs or cited refs")
    score: float | None = Field(None, ge=0, le=100)
    notes: str | None = None

    @field_validator("sources")
    @classmethod
    def sources_non_empty(cls, v: list[str]) -> list[str]:
        cleaned = [s.strip() for s in v if s and s.strip()]
        if len(cleaned) < MIN_SOURCES:
            raise ValueError("at least one non-empty source required")
        return cleaned


class MarketOpportunity(MarketOpportunityIn):
    opportunity_id: str
    created_at: datetime
    research_status: str = "recorded"


class AARDecisionIn(BaseModel):
    opportunity_id: str
    artist_id: str = Field(..., pattern="^DL[0-9]{3}$")
    decision: str = Field(..., pattern="^(greenlight|reject)$")
    rationale: str = Field(..., min_length=10)


class AARDecision(AARDecisionIn):
    decided_at: datetime


class ResearchStatus(BaseModel):
    opportunities_count: int
    sourced_opportunities: int
    aar_decisions_count: int
    greenlit_artist_ids: list[str]
    ready_for_production: bool
    block_reason: str | None = None


class ResearchStore:
    def __init__(self) -> None:
        self.opportunities: dict[str, MarketOpportunity] = {}
        self.decisions: list[AARDecision] = []
        self._opp_counter = 0

    def add_opportunity(self, body: MarketOpportunityIn) -> MarketOpportunity:
        self._opp_counter += 1
        oid = f"DL-OPP-{self._opp_counter:06d}"
        opp = MarketOpportunity(
            opportunity_id=oid,
            created_at=datetime.now(timezone.utc),
            **body.model_dump(),
        )
        self.opportunities[oid] = opp
        return opp

    def list_opportunities(self) -> list[MarketOpportunity]:
        return list(self.opportunities.values())

    def get_opportunity(self, opportunity_id: str) -> MarketOpportunity | None:
        return self.opportunities.get(opportunity_id)

    def record_aar_decision(self, body: AARDecisionIn) -> AARDecision:
        opp = self.opportunities.get(body.opportunity_id)
        if not opp:
            raise KeyError("opportunity not found")
        if body.decision == "greenlight":
            if opp.score is None:
                raise ValueError("greenlight requires opportunity score (0-100) from research")
            if opp.score < MIN_SCORE_FOR_AAR:
                raise ValueError(f"greenlight requires opportunity score >= {MIN_SCORE_FOR_AAR}")
        decision = AARDecision(decided_at=datetime.now(timezone.utc), **body.model_dump())
        self.decisions.append(decision)
        return decision

    def greenlit_artist_ids(self) -> list[str]:
        return list(
            dict.fromkeys(
                d.artist_id for d in self.decisions if d.decision == "greenlight"
            )
        )

    def status(self) -> ResearchStatus:
        opps = self.list_opportunities()
        sourced = len(opps)
        greenlit = self.greenlit_artist_ids()
        ready = sourced >= 1 and len(greenlit) >= 1
        block: str | None = None
        if sourced == 0:
            block = "No sourced market opportunities (DL-OPP-* with citations)."
        elif len(greenlit) == 0:
            block = "Research recorded but A&R has not greenlit any artist on a sourced opportunity."
        return ResearchStatus(
            opportunities_count=len(opps),
            sourced_opportunities=sourced,
            aar_decisions_count=len(self.decisions),
            greenlit_artist_ids=greenlit,
            ready_for_production=ready,
            block_reason=block,
        )


research_store = ResearchStore()
