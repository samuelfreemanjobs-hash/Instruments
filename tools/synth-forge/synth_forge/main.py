from __future__ import annotations

import os
from pathlib import Path

from fastapi import FastAPI, File, HTTPException, UploadFile
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

from synth_forge import __version__
from synth_forge.audio_engine import write_preview_wav
from synth_forge.cloner import analyze_wav, clone_from_wav
from synth_forge.db import SynthMemory
from synth_forge.generator import generate_batch
from synth_forge.models import (
    BatchGenerateRequest,
    BatchRun,
    CloneAudioRequest,
    Preset,
    PromptGenerateRequest,
    Rating,
)
from synth_forge.semantic_engine import generate_from_prompt
from synth_forge.synths import get_adapter, list_synths

STATIC = Path(__file__).resolve().parent / "static"
DB_PATH = os.environ.get("SYNTH_FORGE_DB_PATH", "synth_memory.db")

app = FastAPI(title="SynthForge", version=__version__)
memory = SynthMemory(DB_PATH)


@app.on_event("shutdown")
def _shutdown() -> None:
    memory.close()


@app.get("/api/health")
def health() -> dict[str, str]:
    return {"status": "ok", "version": __version__}


@app.get("/api/synths")
def synths() -> list[dict[str, object]]:
    return list_synths()


@app.post("/api/batch/generate")
def batch_generate(req: BatchGenerateRequest) -> dict[str, object]:
    get_adapter(req.synth_id)
    parent = None
    if req.parent_preset_id:
        stored = memory.list_presets(limit=500)
        match = next((p for p in stored if p.id == req.parent_preset_id), None)
        if match:
            parent = match.parameters
    presets = generate_batch(
        req.synth_id,
        req.count,
        category=req.category,
        seed=req.seed,
        parent=parent,
    )
    memory.save_presets(presets)
    batch = BatchRun(synth_id=req.synth_id, count=req.count, preset_ids=[p.id for p in presets])
    memory.save_batch(batch)
    return {"batch_id": batch.id, "presets": presets}


@app.post("/api/prompt/generate")
def prompt_generate(req: PromptGenerateRequest) -> dict[str, object]:
    get_adapter(req.synth_id)
    presets = generate_from_prompt(req)
    memory.save_presets(presets)
    return {"presets": presets}


@app.post("/api/clone")
async def clone_audio(
    synth_id: str = "minilogue_xd",
    count: int = 6,
    file: UploadFile = File(...),
) -> dict[str, object]:
    data = await file.read()
    if len(data) > 20_000_000:
        raise HTTPException(status_code=413, detail="file too large")
    req = CloneAudioRequest(synth_id=synth_id, count=min(count, 32))
    get_adapter(req.synth_id)
    presets = clone_from_wav(data, req)
    memory.save_presets(presets)
    analysis = analyze_wav(data)
    return {"analysis": analysis, "presets": presets}


@app.get("/api/memory")
def memory_vault(limit: int = 50) -> list[Preset]:
    return memory.list_presets(limit=min(limit, 200))


@app.patch("/api/memory/{preset_id}/rating")
def rate_preset(preset_id: str, rating: Rating) -> dict[str, bool]:
    ok = memory.set_rating(preset_id, rating)
    if not ok:
        raise HTTPException(status_code=404, detail="preset not found")
    return {"ok": True}


@app.get("/api/preview/{preset_id}")
def preview_audio(preset_id: str) -> dict[str, object]:
    presets = memory.list_presets(limit=500)
    preset = next((p for p in presets if p.id == preset_id), None)
    if not preset:
        raise HTTPException(status_code=404, detail="preset not found")
    path, metrics = write_preview_wav(preset.parameters, preset_id)
    return {"url": f"/api/preview/{preset_id}/wav", "metrics": metrics, "path": str(path.name)}


@app.get("/api/preview/{preset_id}/wav")
def preview_wav(preset_id: str) -> FileResponse:
    presets = memory.list_presets(limit=500)
    if not any(p.id == preset_id for p in presets):
        raise HTTPException(status_code=404, detail="preset not found")
    path, _ = write_preview_wav(
        next(p for p in presets if p.id == preset_id).parameters,
        preset_id,
    )
    return FileResponse(path, media_type="audio/wav")


@app.get("/api/export/{preset_id}")
def export_preset(preset_id: str, format: str = "prog") -> dict[str, object]:
    presets = memory.list_presets(limit=500)
    preset = next((p for p in presets if p.id == preset_id), None)
    if not preset:
        raise HTTPException(status_code=404, detail="preset not found")
    adapter = get_adapter(preset.synth_id)
    blob = adapter.pack(preset.parameters)
    return {
        "preset_id": preset_id,
        "synth_id": preset.synth_id,
        "format": format,
        "size": len(blob),
        "hex_preview": blob[:32].hex(),
    }


if STATIC.is_dir():
    app.mount("/", StaticFiles(directory=str(STATIC), html=True), name="static")
