from __future__ import annotations

from pathlib import Path

from fastapi import FastAPI, Header, Request
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field

from coding_agent_stack.config import get_settings
from coding_agent_stack.graph import run_agent_llm, run_agent_stub
from coding_agent_stack.grok_bridge import DelegateRequest, build_handoff
from coding_agent_stack.mcp_bridge import mcp_style_tool_catalog
from coding_agent_stack.observability import trace_span
from coding_agent_stack.schema import ImplementationPlan
from coding_agent_stack.grok_acp import (
    build_acp_payload,
    load_routines,
    parse_inbound_event,
    send_to_grok_channel,
    verify_grok_webhook_signature,
)
from coding_agent_stack.grok_store import append_event, last_event
from coding_agent_stack.team_graph import run_team

STATIC = Path(__file__).resolve().parent.parent / "static"

app = FastAPI(title="Coding Agent Stack", version="0.1.0")

settings = get_settings()
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.cors_origin_list or ["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


class AgentQuery(BaseModel):
    prompt: str = Field(min_length=1, max_length=16_000)
    thread_id: str = "mobile-session"
    force_stub: bool = False


class AgentResponse(BaseModel):
    response: str
    mode: str
    tool_calls: int


class TeamAgentResponse(BaseModel):
    response: str
    mode: str
    tool_calls: int
    qa_passed: bool
    phases: list[str]
    plan_json: str


class DelegateBody(BaseModel):
    target: str = "grok"
    prompt: str = Field(min_length=1, max_length=16_000)
    plan_json: str | None = None


class GrokSendBody(BaseModel):
    prompt: str = Field(min_length=1, max_length=16_000)
    thread_id: str = "pwa-grok"
    routine_id: str | None = None
    plan_json: str | None = None
    architect_first: bool = True


@app.get("/health")
def health():
    return {
        "ok": True,
        "repo_root": str(settings.repo_root),
        "grok_configured": settings.grok_configured,
    }


@app.get("/grok/status")
def grok_status():
    routines = load_routines()
    return {
        "configured": settings.grok_configured,
        "channel_id": settings.grok_channel_id or None,
        "routines": routines,
        "last_event": last_event(),
    }


@app.get("/grok/routines")
def grok_routines():
    return {"routines": load_routines()}


@app.post("/grok/send")
async def grok_send(body: GrokSendBody):
    plan_json = body.plan_json
    if body.architect_first and not plan_json:
        team = run_team(body.prompt, force_stub=True)
        plan_json = team.plan_json
    result = await send_to_grok_channel(
        body.prompt,
        routine_id=body.routine_id,
        client_ref=body.thread_id,
        plan_json=plan_json,
    )
    code = 200 if result.ok or result.dry_run else 502
    if result.dry_run:
        code = 202
    return JSONResponse(
        status_code=code,
        content={
            "ok": result.ok,
            "dry_run": result.dry_run,
            "request_id": result.request_id,
            "detail": result.detail,
            "status_code": result.status_code,
        },
    )


@app.post("/grok/routine/{routine_id}")
async def grok_trigger_routine(routine_id: str, body: GrokSendBody):
    routines = load_routines()
    label = routines.get(routine_id, routine_id)
    prompt = body.prompt or f"Run saved routine: {label}"
    body.routine_id = routine_id
    body.prompt = prompt
    return await grok_send(body)


@app.post("/grok/webhook")
async def grok_webhook(
    request: Request,
    x_grok_signature: str | None = Header(default=None, alias="X-Grok-Signature"),
):
    raw = await request.body()
    if not verify_grok_webhook_signature(raw, x_grok_signature):
        return JSONResponse(status_code=401, content={"error": "invalid signature"})
    try:
        body = await request.json()
    except Exception:
        return JSONResponse(status_code=400, content={"error": "invalid json"})
    event = parse_inbound_event(body if isinstance(body, dict) else {})
    append_event(event)
    return {"ok": True}


@app.get("/tools")
def list_tools():
    return {"tools": mcp_style_tool_catalog()}


@app.post("/agent/team", response_model=TeamAgentResponse)
async def run_agent_team(data: AgentQuery):
    with trace_span("team_run", {"thread_id": data.thread_id}):
        result = run_team(data.prompt, force_stub=data.force_stub)
    return TeamAgentResponse(
        response=result.final_text,
        mode=result.mode,
        tool_calls=result.tool_calls,
        qa_passed=result.qa_passed,
        phases=result.phases,
        plan_json=result.plan_json,
    )


@app.post("/delegate")
async def delegate_outer_loop(body: DelegateBody):
    plan = None
    if body.plan_json:
        plan = ImplementationPlan.model_validate_json(body.plan_json)
    req = DelegateRequest(target=body.target, user_prompt=body.prompt, plan=plan)  # type: ignore[arg-type]
    if body.target not in ("grok", "grok_build", "cursor_cloud", "claude_code"):
        return {"error": "invalid target"}
    return build_handoff(req).model_dump()


@app.post("/agent", response_model=AgentResponse)
async def run_agent(data: AgentQuery):
    with trace_span("agent_run", {"thread_id": data.thread_id}):
        if data.force_stub or (
            not settings.openai_api_key and not settings.anthropic_api_key
        ):
            result = run_agent_stub(data.prompt)
        else:
            result = run_agent_llm(data.prompt, thread_id=data.thread_id)
    return AgentResponse(
        response=result.final_text,
        mode=result.mode,
        tool_calls=result.tool_calls,
    )


if STATIC.is_dir():
    app.mount("/assets", StaticFiles(directory=STATIC), name="assets")

    @app.get("/")
    def pwa_root():
        return FileResponse(STATIC / "index.html")

    @app.get("/manifest.json")
    def manifest():
        return FileResponse(STATIC / "manifest.json")

    @app.get("/sw.js")
    def service_worker():
        return FileResponse(STATIC / "sw.js")
