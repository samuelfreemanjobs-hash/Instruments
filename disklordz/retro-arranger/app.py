#!/usr/bin/env python3
"""FastAPI server for Retro Arranger stems + static UI."""

from __future__ import annotations

import base64
from pathlib import Path

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import Response
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

from retro_arranger.models import ArrangementSpec
from retro_arranger.midi_writer import multitrack_to_bytes, stem_to_bytes, stems_zip_bytes
from retro_arranger.pipeline import compose

ROOT = Path(__file__).resolve().parent
STATIC = ROOT / "static"

app = FastAPI(title="Retro Arranger", version="0.1.0")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


class GenerateRequest(BaseModel):
    title: str = Field(default="Quiet Storm Night Drive", max_length=80)
    key_root: str = Field(default="A", max_length=3)
    scale: str = Field(default="dorian", pattern="^(dorian|mixolydian|aeolian)$")
    bpm: int = Field(default=98, ge=60, le=140)
    swing_pct: float = Field(default=58.0, ge=50.0, le=66.0)
    bass_mode: str = Field(default="synth", pattern="^(synth|electric)$")


class StemPayload(BaseModel):
    file_name: str
    stem_id: str
    data_base64: str


class GenerateResponse(BaseModel):
    title: str
    total_bars: int
    stems: list[StemPayload]
    full_mid_base64: str


@app.get("/api/health")
def health():
    return {"ok": True}


@app.post("/api/generate", response_model=GenerateResponse)
def generate(body: GenerateRequest):
    spec = ArrangementSpec(
        title=body.title,
        key_root=body.key_root,
        scale=body.scale,
        bpm=body.bpm,
        swing_pct=body.swing_pct,
    )
    result = compose(spec, bass_mode=body.bass_mode)
    stems_out: list[StemPayload] = []
    for stem in result.stems:
        raw = stem_to_bytes(stem, result.ticks_per_beat, result.spec.bpm)
        stems_out.append(
            StemPayload(
                file_name=stem.file_name,
                stem_id=stem.stem_id,
                data_base64=base64.b64encode(raw).decode("ascii"),
            )
        )
    full = base64.b64encode(multitrack_to_bytes(result)).decode("ascii")
    return GenerateResponse(
        title=spec.title,
        total_bars=spec.total_bars,
        stems=stems_out,
        full_mid_base64=full,
    )


@app.post("/api/generate/zip")
def generate_zip(body: GenerateRequest):
    spec = ArrangementSpec(
        title=body.title,
        key_root=body.key_root,
        scale=body.scale,
        bpm=body.bpm,
        swing_pct=body.swing_pct,
    )
    result = compose(spec, bass_mode=body.bass_mode)
    data = stems_zip_bytes(result)
    return Response(
        content=data,
        media_type="application/zip",
        headers={"Content-Disposition": 'attachment; filename="Retro_Arranger_Stems.zip"'},
    )


if STATIC.is_dir():
    app.mount("/", StaticFiles(directory=str(STATIC), html=True), name="static")


def main():
    import uvicorn

    uvicorn.run("app:app", host="127.0.0.1", port=8790, reload=False)


if __name__ == "__main__":
    main()
