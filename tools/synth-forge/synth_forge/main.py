from __future__ import annotations

import io
import os
import zipfile
from pathlib import Path

from fastapi import FastAPI, File, HTTPException, UploadFile
from fastapi.responses import FileResponse, Response
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

from synth_forge.bin_diff import compare_bytes, group_contiguous, suggest_fields_from_diff

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
from synth_forge.synths.minilogue_xd_librarian import MinilogueXdLibrarian, default_fixtures_dir
from synth_forge.synths.zenology_svz import ZenologySvzEngine
from synth_forge.zen_forge_presets import ZEN_PROFILES, apply_zen_profile, procedural_name

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


class LibrarianBatchRequest(BaseModel):
    count: int = Field(default=8, ge=1, le=32)
    category: str = "xd_pad"
    seed: int | None = None


class ZenForgeBatchRequest(BaseModel):
    profile: str = "memphis_lead"
    count: int = Field(default=8, ge=1, le=64)
    seed: int | None = None
    bank: bool = False


@app.post("/api/bin-diff")
async def bin_diff(file_a: UploadFile = File(...), file_b: UploadFile = File(...)) -> dict[str, object]:
    left = await file_a.read()
    right = await file_b.read()
    if len(left) > 5_000_000 or len(right) > 5_000_000:
        raise HTTPException(status_code=413, detail="file too large")
    changes = compare_bytes(left, right)
    ranges = group_contiguous(changes)
    suggestions = suggest_fields_from_diff(changes)
    return {
        "length_a": len(left),
        "length_b": len(right),
        "change_count": len(changes),
        "ranges": [{"start": s, "end": e, "span": e - s + 1} for s, e in ranges],
        "suggestions": [s.model_dump() for s in suggestions],
    }


@app.post("/api/librarian/xd/batch")
def librarian_xd_batch(req: LibrarianBatchRequest) -> Response:
    lib = MinilogueXdLibrarian()
    seed_path = default_fixtures_dir() / "seed_init.mnlgxdprog"
    seed_prog = lib.load_seed_file(seed_path) if seed_path.is_file() else None
    presets = generate_batch("minilogue_xd", req.count, category=req.category, seed=req.seed)
    memory.save_presets(presets)
    buf = io.BytesIO()
    with zipfile.ZipFile(buf, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for preset in presets:
            arc = lib.pack_mnlgxdprog(preset.parameters, preset.name, seed_prog=seed_prog)
            zf.writestr(f"{preset.name}.mnlgxdprog", arc)
    filename = f"xd_forge_{req.category}_{req.count}.zip"
    return Response(
        content=buf.getvalue(),
        media_type="application/zip",
        headers={"Content-Disposition": f'attachment; filename="{filename}"'},
    )


@app.post("/api/librarian/zen/batch")
def librarian_zen_batch(req: ZenForgeBatchRequest) -> Response:
    if req.profile not in ZEN_PROFILES:
        raise HTTPException(status_code=400, detail=f"unknown profile: {req.profile}")
    engine = ZenologySvzEngine()
    seed_path = ZenologySvzEngine.default_fixtures_dir() / "seed_init.svz"
    seed = seed_path.read_bytes() if seed_path.is_file() else None
    import random

    rng = random.Random(req.seed)
    pairs: list[tuple] = []
    for i in range(req.count):
        params = apply_zen_profile(req.profile, rng)
        pairs.append((params, procedural_name(req.profile, i + 1)))
    if req.bank or req.count > 1:
        payload = engine.pack_svz_bank(pairs, seed=seed)
        filename = f"zen_forge_{req.profile}_{req.count}.zip"
        media_type = "application/zip"
    else:
        payload = engine.pack_svz(pairs[0][0], pairs[0][1], seed=seed)
        filename = f"{pairs[0][1]}.svz"
        media_type = "application/octet-stream"
    return Response(
        content=payload,
        media_type=media_type,
        headers={"Content-Disposition": f'attachment; filename="{filename}"'},
    )


@app.get("/api/librarian/profiles")
def librarian_profiles() -> dict[str, object]:
    return {
        "minilogue_xd": ["xd_bass", "xd_pad", "xd_pluck", "xd_lead", "xd_scifi_fx"],
        "zenology": list(ZEN_PROFILES.keys()),
    }


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


@app.get("/api/export/{preset_id}/download")
def export_preset_download(preset_id: str, format: str = "prog") -> Response:
    presets = memory.list_presets(limit=500)
    preset = next((p for p in presets if p.id == preset_id), None)
    if not preset:
        raise HTTPException(status_code=404, detail="preset not found")
    fmt = format.lower()
    if preset.synth_id == "minilogue_xd" and fmt in {"mnlgxdprog", "librarian"}:
        lib = MinilogueXdLibrarian()
        seed_path = default_fixtures_dir() / "seed_init.mnlgxdprog"
        seed_prog = lib.load_seed_file(seed_path) if seed_path.is_file() else None
        blob = lib.pack_mnlgxdprog(preset.parameters, preset.name, seed_prog=seed_prog)
        filename = f"{preset.name}.mnlgxdprog"
        media = "application/octet-stream"
    elif preset.synth_id == "zenology" and fmt in {"svz", "librarian"}:
        engine = ZenologySvzEngine()
        blob = engine.pack_svz(preset.parameters, preset.name)
        filename = f"{preset.name}.svz"
        media = "application/octet-stream"
    else:
        adapter = get_adapter(preset.synth_id)
        blob = adapter.pack(preset.parameters)
        filename = f"{preset.name}.bin"
        media = "application/octet-stream"
    return Response(
        content=blob,
        media_type=media,
        headers={"Content-Disposition": f'attachment; filename="{filename}"'},
    )


if STATIC.is_dir():
    app.mount("/", StaticFiles(directory=str(STATIC), html=True), name="static")
