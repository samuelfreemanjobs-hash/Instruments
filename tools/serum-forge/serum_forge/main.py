from __future__ import annotations

import os
import tempfile
from pathlib import Path

from fastapi import FastAPI, HTTPException, Request
from pydantic import BaseModel, Field

from serum_forge import __version__
from serum_forge.dx7 import Dx7IncompatibilityError, reject_dx7_sysex
from serum_forge.models import PatchArchetype, SerumSymbolPatch
from serum_forge.param_spec import param_openapi_schema, validate_symbolic_payload
from serum_forge.pipeline import run_pipeline

DEFAULT_OUT = Path(os.environ.get("SERUM_FORGE_OUT_DIR", tempfile.gettempdir())) / "serum-forge-api"


class PromptPipelineRequest(BaseModel):
    prompt: str = Field(..., min_length=1, max_length=2000)
    archetype: PatchArchetype = PatchArchetype.pad_ambient
    compile_binary: bool = False


class SymbolicPatchRequest(BaseModel):
    patch: dict


app = FastAPI(title="Serum Forge", version=__version__)


@app.get("/api/health")
def health() -> dict[str, str]:
    return {"status": "ok", "version": __version__}


@app.get("/api/schema")
def openapi_param_schema() -> dict:
    return param_openapi_schema()


@app.get("/api/archetypes")
def archetypes() -> list[dict[str, str]]:
    return [{"id": a.value, "label": a.name} for a in PatchArchetype]


@app.post("/api/pipeline/prompt")
def pipeline_from_prompt(req: PromptPipelineRequest) -> dict:
    out_dir = DEFAULT_OUT / "runs"
    result = run_pipeline(
        req.prompt,
        archetype=req.archetype,
        out_dir=out_dir,
        compile_binary=req.compile_binary,
    )
    return {
        "patch": result.patch.model_dump(),
        "intermediate_json": str(result.intermediate_json) if result.intermediate_json else None,
        "serum_preset": str(result.serum_preset) if result.serum_preset else None,
        "evaluation": result.evaluation,
        "warnings": result.warnings,
    }


@app.post("/api/patch/validate")
def validate_patch(req: SymbolicPatchRequest) -> dict:
    try:
        patch = validate_symbolic_payload(req.patch)
    except Exception as exc:  # noqa: BLE001 — return 422 detail to client
        raise HTTPException(status_code=422, detail=str(exc)) from exc
    return {"ok": True, "patch": patch.model_dump()}


@app.post("/api/dx7/reject")
async def dx7_reject_probe(request: Request) -> dict:
    """Reject DX7 SysEx uploads (guardrail self-test endpoint)."""
    payload = await request.body()
    try:
        reject_dx7_sysex(payload)
    except Dx7IncompatibilityError as exc:
        return {"rejected": True, "reason": str(exc)}
    return {"rejected": False}
