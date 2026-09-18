"""
Marketing Director — local API orchestrator (importable module).

Used by marketing_director_live.py and test_harness.py (mocked/smoke/full).
Not imported by business_agent.py or GitHub Actions except mocked harness.
"""

from __future__ import annotations

import json
import logging
import time
import uuid
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Callable, Optional

try:
    from anthropic.types import Message
except ImportError:  # CI mocked harness uses MagicMock clients
    Message = Any  # type: ignore[misc, assignment]

logger = logging.getLogger(__name__)

COMPANY = "Instruments + Disklordz"
BRAND_MEMORY_PATH = Path(__file__).resolve().parent / "brand_memory" / "instruments_disklordz.json"


@dataclass
class ModelConfig:
    director: str = "claude-opus-4-6"
    creative: str = "claude-opus-4-6"
    research: str = "claude-sonnet-4-6"
    copy: str = "claude-sonnet-4-6"
    media: str = "claude-sonnet-4-6"
    analytics: str = "claude-sonnet-4-6"
    compliance: str = "claude-haiku-4-5-20251001"


@dataclass
class LoopBudget:
    director: int = 12
    research: int = 8
    creative: int = 6
    copy: int = 5
    media: int = 5
    analytics: int = 6
    compliance: int = 4


@dataclass
class CostThresholds:
    budget_commit_usd: float = 25_000
    estimated_token_cost_usd: float = 50
    repeated_calls_to_same_specialist: int = 3


SPECIALIST_TOOL_SCHEMAS: list[dict[str, Any]] = [
    {
        "name": "research_agent",
        "description": "Market research, competitive analysis, audience insights.",
        "input_schema": {
            "type": "object",
            "properties": {
                "question": {"type": "string"},
                "scope": {"type": "string"},
                "depth": {"type": "string", "enum": ["quick", "standard", "deep"]},
                "deadline_minutes": {"type": "integer"},
            },
            "required": ["question", "depth"],
        },
    },
    {
        "name": "creative_agent",
        "description": "Creative concepts and brand expression.",
        "input_schema": {
            "type": "object",
            "properties": {
                "brief": {"type": "string"},
                "audience": {"type": "string"},
                "channels": {"type": "array", "items": {"type": "string"}},
            },
            "required": ["brief", "audience", "channels"],
        },
    },
    {
        "name": "copy_agent",
        "description": "Ad copy, email, social, landing variants.",
        "input_schema": {
            "type": "object",
            "properties": {
                "brief": {"type": "string"},
                "channel": {"type": "string"},
                "variants": {"type": "integer", "default": 3},
            },
            "required": ["brief", "channel"],
        },
    },
    {
        "name": "media_agent",
        "description": "Media mix and budget allocation.",
        "input_schema": {
            "type": "object",
            "properties": {
                "budget_usd": {"type": "number"},
                "audience": {"type": "string"},
                "flight_days": {"type": "integer"},
                "kpi_priority": {
                    "type": "string",
                    "enum": ["reach", "engagement", "conversion", "retention"],
                },
            },
            "required": ["budget_usd", "audience", "flight_days", "kpi_priority"],
        },
    },
    {
        "name": "analytics_agent",
        "description": "Performance analysis and experiment design.",
        "input_schema": {
            "type": "object",
            "properties": {
                "question": {"type": "string"},
                "analysis_type": {
                    "type": "string",
                    "enum": ["descriptive", "diagnostic", "predictive", "experiment_design"],
                },
            },
            "required": ["question", "analysis_type"],
        },
    },
    {
        "name": "compliance_agent",
        "description": "Legal/regulatory review — required before external publish.",
        "input_schema": {
            "type": "object",
            "properties": {
                "content": {"type": "string"},
                "content_type": {"type": "string"},
            },
            "required": ["content", "content_type"],
        },
    },
    {
        "name": "brand_memory_read",
        "description": "Retrieve brand guidelines and prohibited claims.",
        "input_schema": {
            "type": "object",
            "properties": {"topic": {"type": "string"}},
            "required": ["topic"],
        },
    },
    {
        "name": "request_human_review",
        "description": "Escalate to human reviewer.",
        "input_schema": {
            "type": "object",
            "properties": {
                "reason": {"type": "string"},
                "urgency": {"type": "string", "enum": ["routine", "elevated", "urgent"]},
                "context": {"type": "string"},
            },
            "required": ["reason", "urgency", "context"],
        },
    },
]

DIRECTOR_SYSTEM_PROMPT = f"""\
You are the Marketing Director for {COMPANY}. Plan, delegate to specialists (tools), \
synthesize deliverables. Never skip compliance for external-facing content. Escalate \
when budget exceeds $25,000, compliance is HIGH/CRITICAL, or same specialist is stuck. \
If compliance blocks (MEDIUM+), do not echo prohibited claims in the final deliverable — \
state the block and revisions. Check brand_memory before campaigns.
"""


@dataclass
class SpecialistResult:
    status: str
    output: dict[str, Any] = field(default_factory=dict)
    notes: str = ""
    tokens_used: int = 0
    loops_used: int = 0


class BaseSpecialist:
    SYSTEM_PROMPT: str = ""
    TOOLS: list[dict[str, Any]] = []

    def __init__(
        self,
        client: Any,
        model: str,
        max_loops: int,
        tool_executor: Optional[Callable[[str, dict], Any]] = None,
        trace_id: str = "",
    ):
        self.client = client
        self.model = model
        self.max_loops = max_loops
        self.tool_executor = tool_executor or (
            lambda name, inp: {"error": f"no executor for {name}"}
        )
        self.trace_id = trace_id

    def _build_user_message(self, input_data: dict) -> str:
        return json.dumps(input_data, indent=2)

    def _extract_output(self, response: Message) -> dict[str, Any]:
        text = "".join(
            block.text for block in response.content if getattr(block, "type", "") == "text"
        )
        return {"text": text}

    def run(self, input_data: dict) -> SpecialistResult:
        messages: list[dict[str, Any]] = [
            {"role": "user", "content": self._build_user_message(input_data)}
        ]
        total_tokens = 0
        for loop_n in range(self.max_loops):
            kwargs: dict[str, Any] = dict(
                model=self.model,
                max_tokens=2048,
                system=self.SYSTEM_PROMPT,
                messages=messages,
            )
            if self.TOOLS:
                kwargs["tools"] = self.TOOLS
            response = self.client.messages.create(**kwargs)
            total_tokens += response.usage.input_tokens + response.usage.output_tokens
            if response.stop_reason == "end_turn":
                return SpecialistResult(
                    status="ok",
                    output=self._extract_output(response),
                    tokens_used=total_tokens,
                    loops_used=loop_n + 1,
                )
            if response.stop_reason == "tool_use":
                messages.append({"role": "assistant", "content": response.content})
                tool_results = []
                for block in response.content:
                    if getattr(block, "type", "") == "tool_use":
                        try:
                            result = self.tool_executor(block.name, block.input)
                        except Exception as e:
                            logger.exception("tool execution failed")
                            result = {"error": str(e)}
                        tool_results.append(
                            {
                                "type": "tool_result",
                                "tool_use_id": block.id,
                                "content": json.dumps(result),
                            }
                        )
                messages.append({"role": "user", "content": tool_results})
                continue
            break
        return SpecialistResult(
            status="incomplete",
            notes=f"loop budget {self.max_loops} exhausted",
            tokens_used=total_tokens,
            loops_used=self.max_loops,
        )


class ResearchAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"Research specialist for {COMPANY}. Return JSON findings."


class CreativeAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"Creative specialist for {COMPANY}. Return JSON concepts."


class CopyAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"Copy specialist for {COMPANY}. Return JSON with variants."


class MediaAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"Media planner for {COMPANY}. Return JSON channel mix."


class AnalyticsAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"Analytics specialist for {COMPANY}. Return JSON findings."


class ComplianceAgent(BaseSpecialist):
    SYSTEM_PROMPT = f"""\
Compliance for {COMPANY}. Return JSON: severity, issues[], required_disclaimers[], ready_to_publish.
"""

    def _extract_output(self, response: Message) -> dict[str, Any]:
        text = "".join(b.text for b in response.content if getattr(b, "type", "") == "text")
        try:
            verdict = json.loads(text)
        except json.JSONDecodeError:
            return {
                "severity": "HIGH",
                "issues": [],
                "required_disclaimers": [],
                "ready_to_publish": False,
                "_parse_error": True,
            }
        sev = verdict.get("severity", "HIGH")
        if sev in {"MEDIUM", "HIGH", "CRITICAL"}:
            verdict["ready_to_publish"] = False
        return verdict


class MarketingDirector:
    def __init__(
        self,
        client: Any,
        models: Optional[ModelConfig] = None,
        budgets: Optional[LoopBudget] = None,
        thresholds: Optional[CostThresholds] = None,
        brand_memory_loader: Optional[Callable[[str], dict]] = None,
        human_review_handler: Optional[Callable[[dict], dict]] = None,
    ):
        self.client = client
        self.models = models or ModelConfig()
        self.budgets = budgets or LoopBudget()
        self.thresholds = thresholds or CostThresholds()
        self.brand_memory_loader = brand_memory_loader or self._default_brand_memory
        self.human_review_handler = human_review_handler or self._default_human_review
        self.trace_id = str(uuid.uuid4())
        te = self._specialist_tool_executor
        tid = self.trace_id
        self.specialists: dict[str, BaseSpecialist] = {
            "research_agent": ResearchAgent(
                client, self.models.research, self.budgets.research, te, tid
            ),
            "creative_agent": CreativeAgent(
                client, self.models.creative, self.budgets.creative, te, tid
            ),
            "copy_agent": CopyAgent(client, self.models.copy, self.budgets.copy, te, tid),
            "media_agent": MediaAgent(client, self.models.media, self.budgets.media, te, tid),
            "analytics_agent": AnalyticsAgent(
                client, self.models.analytics, self.budgets.analytics, te, tid
            ),
            "compliance_agent": ComplianceAgent(
                client, self.models.compliance, self.budgets.compliance, te, tid
            ),
        }
        self._call_counts: dict[str, int] = {}

    def handle_request(self, user_request: str) -> dict[str, Any]:
        start = time.time()
        total_tokens = 0
        messages: list[dict[str, Any]] = [{"role": "user", "content": user_request}]
        escalated = False
        specialist_call_counts: dict[str, int] = {}

        for _loop_n in range(self.budgets.director):
            response = self.client.messages.create(
                model=self.models.director,
                max_tokens=4096,
                system=DIRECTOR_SYSTEM_PROMPT,
                tools=SPECIALIST_TOOL_SCHEMAS,
                messages=messages,
            )
            total_tokens += response.usage.input_tokens + response.usage.output_tokens
            if response.stop_reason == "end_turn":
                deliverable = "".join(
                    b.text for b in response.content if getattr(b, "type", "") == "text"
                )
                return {
                    "status": "ok",
                    "trace_id": self.trace_id,
                    "deliverable": deliverable,
                    "tokens_used": total_tokens,
                    "wall_time_sec": round(time.time() - start, 2),
                    "escalated": escalated,
                    "specialist_call_counts": dict(specialist_call_counts),
                }
            if response.stop_reason == "tool_use":
                messages.append({"role": "assistant", "content": response.content})
                tool_results = []
                for block in response.content:
                    if getattr(block, "type", "") != "tool_use":
                        continue
                    self._call_counts[block.name] = self._call_counts.get(block.name, 0) + 1
                    if block.name.endswith("_agent"):
                        specialist_call_counts[block.name] = (
                            specialist_call_counts.get(block.name, 0) + 1
                        )
                    if (
                        self._call_counts[block.name]
                        > self.thresholds.repeated_calls_to_same_specialist
                    ):
                        result = self._force_escalate(block.name)
                        escalated = True
                    else:
                        result = self._dispatch(block.name, block.input)
                        if block.name == "request_human_review":
                            escalated = True
                    tool_results.append(
                        {
                            "type": "tool_result",
                            "tool_use_id": block.id,
                            "content": json.dumps(result),
                        }
                    )
                messages.append({"role": "user", "content": tool_results})
                continue
            break
        return {
            "status": "incomplete",
            "trace_id": self.trace_id,
            "tokens_used": total_tokens,
            "wall_time_sec": round(time.time() - start, 2),
            "escalated": escalated,
        }

    def _dispatch(self, tool_name: str, tool_input: dict) -> dict:
        if tool_name in self.specialists:
            result = self.specialists[tool_name].run(tool_input)
            return {
                "status": result.status,
                "output": result.output,
                "notes": result.notes,
                "loops_used": result.loops_used,
            }
        if tool_name == "brand_memory_read":
            return self.brand_memory_loader(tool_input.get("topic", ""))
        if tool_name == "request_human_review":
            return self.human_review_handler(tool_input)
        return {"error": f"unknown tool: {tool_name}"}

    def _specialist_tool_executor(self, tool_name: str, tool_input: dict) -> dict:
        if tool_name in ("web_search", "internal_db_query", "analytics_query"):
            return {"_stub": True, "tool": tool_name}
        return {"error": f"no executor for {tool_name}"}

    def _force_escalate(self, tool_name: str) -> dict:
        return self.human_review_handler(
            {
                "reason": f"Repeated calls to {tool_name} ({self._call_counts[tool_name]} times)",
                "urgency": "elevated",
                "context": "Director loop cap",
            }
        )

    def _default_brand_memory(self, topic: str) -> dict:
        if BRAND_MEMORY_PATH.is_file():
            data = json.loads(BRAND_MEMORY_PATH.read_text(encoding="utf-8"))
            return {"topic": topic, "brand_memory": data}
        return {"topic": topic, "_stub": True}

    def _default_human_review(self, request: dict) -> dict:
        logger.warning("HUMAN REVIEW: %s", request)
        return {"status": "queued", "review_id": str(uuid.uuid4()), "_stub": True}
