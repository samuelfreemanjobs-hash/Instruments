from __future__ import annotations

import os
from pathlib import Path

from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

from vst_factory import __version__
from vst_factory.build_broker import windows_chord_instructions
from vst_factory.db import FactoryDB
from vst_factory.models import CursorPromptOut, ProjectCreate, ProjectOut, RunOut
from vst_factory import modules as modlib
from vst_factory.orchestrator import Orchestrator
from vst_factory.references import load_chord_engine_reference

PACKAGE_ROOT = Path(__file__).resolve().parents[1]
STATIC = PACKAGE_ROOT / "static"
DB_PATH = os.environ.get("VST_FACTORY_DB", str(PACKAGE_ROOT / "factory_projects.sqlite"))

app = FastAPI(title="DiskLordz VST Factory", version=__version__)
db = FactoryDB(DB_PATH)
orch = Orchestrator(db)


@app.on_event("shutdown")
def _shutdown() -> None:
    db.close()


def _project_out(p: dict) -> ProjectOut:
    return ProjectOut(
        id=p["id"],
        plugin_name=p["plugin_name"],
        plugin_class=p["plugin_class"],
        state=p["state"],
        spec=p["spec"],
        apc_path=p.get("apc_path"),
        vst_path=p.get("vst_path"),
    )


@app.get("/api/health")
def health() -> dict[str, str]:
    return {"status": "ok", "version": __version__}


@app.get("/api/modules/catalog")
def api_catalog() -> dict:
    return modlib.load_catalog()


@app.get("/api/modules/plugin-class/{plugin_class}")
def api_plugin_class(plugin_class: str) -> dict:
    try:
        return modlib.plugin_class_module(plugin_class)
    except ValueError as exc:
        raise HTTPException(status_code=404, detail=str(exc)) from exc


@app.get("/api/modules/dsp")
def api_dsp_blocks() -> list[dict]:
    return modlib.list_dsp_blocks()


@app.get("/api/modules/gui")
def api_gui_blocks() -> list[dict]:
    return modlib.list_gui_blocks()


@app.get("/api/references/chord-engine")
def api_chord_reference() -> dict:
    ref = load_chord_engine_reference()
    ref["build_broker"] = windows_chord_instructions()
    return ref


@app.get("/api/projects", response_model=list[ProjectOut])
def list_projects(limit: int = 50) -> list[ProjectOut]:
    return [_project_out(p) for p in db.list_projects(limit=min(limit, 100))]


@app.post("/api/projects", response_model=ProjectOut)
def create_project(body: ProjectCreate) -> ProjectOut:
    try:
        proj = orch.create_project(
            prompt=body.prompt,
            plugin_class=body.plugin_class,
            plugin_name=body.plugin_name,
        )
        return _project_out(proj)
    except ValueError as exc:
        raise HTTPException(status_code=409, detail=str(exc)) from exc


@app.get("/api/projects/{project_id}", response_model=ProjectOut)
def get_project(project_id: int) -> ProjectOut:
    proj = db.get_project(project_id)
    if not proj:
        raise HTTPException(status_code=404, detail="project not found")
    return _project_out(proj)


@app.post("/api/projects/{project_id}/scaffold", response_model=ProjectOut)
def scaffold_project(project_id: int) -> ProjectOut:
    try:
        proj = orch.scaffold(project_id)
        return _project_out(proj)
    except FileExistsError as exc:
        raise HTTPException(status_code=409, detail=str(exc)) from exc
    except RuntimeError as exc:
        raise HTTPException(status_code=500, detail=str(exc)) from exc


@app.post("/api/projects/{project_id}/build")
def build_project(project_id: int) -> dict:
    try:
        return orch.build(project_id)
    except KeyError as exc:
        raise HTTPException(status_code=404, detail="project not found") from exc


@app.post("/api/projects/{project_id}/test")
def test_project(project_id: int) -> dict:
    try:
        return orch.test(project_id)
    except KeyError as exc:
        raise HTTPException(status_code=404, detail="project not found") from exc


@app.get("/api/projects/{project_id}/runs", response_model=list[RunOut])
def project_runs(project_id: int, limit: int = 10) -> list[RunOut]:
    if not db.get_project(project_id):
        raise HTTPException(status_code=404, detail="project not found")
    return [RunOut(**r) for r in db.list_runs(project_id, limit=limit)]


@app.get("/api/projects/{project_id}/cursor-prompt", response_model=CursorPromptOut)
def cursor_prompt(project_id: int, role: str = "cpp") -> CursorPromptOut:
    try:
        text = orch.export_cursor_prompt(project_id, role=role)
    except KeyError as exc:
        raise HTTPException(status_code=404, detail="project or role not found") from exc
    return CursorPromptOut(project_id=project_id, role=role, prompt_text=text)


@app.get("/")
def index() -> FileResponse:
    return FileResponse(STATIC / "index.html")


if STATIC.is_dir():
    app.mount("/static", StaticFiles(directory=str(STATIC)), name="static")
