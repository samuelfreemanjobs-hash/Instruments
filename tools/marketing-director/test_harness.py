#!/usr/bin/env python3
"""
Marketing Director — test harness.

  mocked — no API calls; scripted director + mock specialists
  smoke    — real API on cases marked smoke=True
  full     — all cases + LLM judges (requires ANTHROPIC_API_KEY)

Run from this directory:
  python3 test_harness.py --mode mocked
"""

from __future__ import annotations

import argparse
import json
import logging
import os
import sys
import time
import uuid
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Any, Optional

from eval_cases import EVAL_CASES, EvalCase, build_mock_tool_sequence
from marketing_director import (
    BaseSpecialist,
    LoopBudget,
    MarketingDirector,
    ModelConfig,
    SpecialistResult,
)

logger = logging.getLogger("test_harness")

COMPANY_JUDGE = "Instruments + Disklordz"


@dataclass
class TraceEvent:
    ts: float
    kind: str
    tool: str = ""
    input: dict = field(default_factory=dict)
    output: dict = field(default_factory=dict)
    duration_ms: float = 0
    note: str = ""


class TraceRecorder:
    def __init__(self, trace_id: str = ""):
        self.trace_id = trace_id or str(uuid.uuid4())
        self.events: list[TraceEvent] = []
        self.started_at = time.time()

    def record(self, kind: str, **kwargs) -> None:
        self.events.append(TraceEvent(ts=time.time() - self.started_at, kind=kind, **kwargs))

    def specialists_called(self) -> list[str]:
        return [
            e.tool
            for e in self.events
            if e.kind == "dispatch_start" and e.tool.endswith("_agent")
        ]

    def unique_specialists(self) -> set[str]:
        return set(self.specialists_called())

    def was_escalated(self) -> bool:
        return any(e.kind == "escalation" for e in self.events)

    def call_count(self, tool: str) -> int:
        return sum(1 for e in self.events if e.kind == "dispatch_start" and e.tool == tool)

    def to_jsonl(self) -> str:
        return "\n".join(
            json.dumps({"trace_id": self.trace_id, **asdict(e)}) for e in self.events
        )


class InstrumentedDirector(MarketingDirector):
    def __init__(self, *args, recorder: Optional[TraceRecorder] = None, **kwargs):
        super().__init__(*args, **kwargs)
        self.recorder = recorder or TraceRecorder(trace_id=self.trace_id)

    def _dispatch(self, tool_name: str, tool_input: dict) -> dict:
        start = time.time()
        self.recorder.record("dispatch_start", tool=tool_name, input=tool_input)
        try:
            result = super()._dispatch(tool_name, tool_input)
        except Exception as e:
            self.recorder.record("dispatch_error", tool=tool_name, note=str(e))
            raise
        self.recorder.record(
            "dispatch_end",
            tool=tool_name,
            output={"status": result.get("status", "?")},
            duration_ms=(time.time() - start) * 1000,
        )
        if tool_name == "request_human_review":
            self.recorder.record("escalation", tool=tool_name, input=tool_input)
        return result

    def _force_escalate(self, tool_name: str) -> dict:
        self.recorder.record(
            "escalation",
            tool=tool_name,
            note="loop_cap",
            input={"tool": tool_name, "count": self._call_counts.get(tool_name)},
        )
        return super()._force_escalate(tool_name)


class MockSpecialist(BaseSpecialist):
    def __init__(self, canned_output: dict, status: str = "ok"):
        self.canned_output = canned_output
        self.status = status
        self.calls: list[dict] = []

    def run(self, input_data: dict) -> SpecialistResult:
        self.calls.append(input_data)
        return SpecialistResult(
            status=self.status,
            output=self.canned_output,
            tokens_used=0,
            loops_used=1,
        )


DEFAULT_CANNED_OUTPUTS: dict[str, dict] = {
    "research_agent": {
        "summary": "Mock research summary.",
        "findings": [{"claim": "mock", "source": "mock://", "confidence": "medium"}],
        "gaps": [],
        "recommendations": [],
    },
    "creative_agent": {
        "concepts": [
            {
                "name": "Concept A",
                "core_idea": "mock",
                "visual_direction": "mock",
                "emotional_arc": "mock",
                "channel_adaptations": {},
                "rationale": "mock",
            },
        ],
        "recommended": "Concept A",
        "trade_offs": "mock",
    },
    "copy_agent": {
        "variants": [
            {
                "copy": "Sampleless 909 weight in your DAW",
                "character_count": 32,
                "hypothesis": "workflow",
                "suggested_visual_pair": "",
            },
            {
                "copy": "WAVE-909 waitlist — trap subs without samples",
                "character_count": 42,
                "hypothesis": "benefit",
                "suggested_visual_pair": "",
            },
            {
                "copy": "Open the demo preset menu: Phonk Stack 03",
                "character_count": 38,
                "hypothesis": "specificity",
                "suggested_visual_pair": "",
            },
        ],
        "recommended_variant": 0,
        "notes": "mock",
    },
    "media_agent": {
        "channel_mix": [
            {
                "channel": "email",
                "budget_usd": 5000,
                "percentage": 100,
                "expected_reach": 0,
                "expected_cpa_or_cpc": 0,
                "rationale": "mock",
            }
        ],
        "total_budget_check": "ok",
        "kpi_targets": {},
        "risk_notes": "mock",
    },
    "analytics_agent": {
        "findings": [{"claim": "mock", "evidence": "mock", "confidence": "medium"}],
        "methodology": "mock",
        "recommendations": [],
        "data_gaps": [],
    },
    "compliance_agent": {
        "severity": "NONE",
        "issues": [],
        "required_disclaimers": [],
        "ready_to_publish": True,
    },
}


def install_mocks(director: MarketingDirector, canned: dict[str, dict]) -> None:
    for name in list(director.specialists.keys()):
        output = canned.get(name, {"_mocked": True})
        director.specialists[name] = MockSpecialist(output)


@dataclass
class AssertionResult:
    name: str
    passed: bool
    detail: str = ""


class Assertions:
    @staticmethod
    def specialists_include(recorder: TraceRecorder, required: list[str]) -> AssertionResult:
        called = recorder.unique_specialists()
        missing = set(required) - called
        return AssertionResult(
            name=f"specialists_include({required})",
            passed=not missing,
            detail=f"missing: {missing}" if missing else f"called: {sorted(called)}",
        )

    @staticmethod
    def specialists_exclude(recorder: TraceRecorder, forbidden: list[str]) -> AssertionResult:
        called = recorder.unique_specialists()
        bad = set(forbidden) & called
        return AssertionResult(
            name=f"specialists_exclude({forbidden})",
            passed=not bad,
            detail=f"unexpectedly called: {bad}" if bad else "ok",
        )

    @staticmethod
    def escalated(recorder: TraceRecorder, should: bool = True) -> AssertionResult:
        actual = recorder.was_escalated()
        return AssertionResult(
            name=f"escalated={should}",
            passed=actual == should,
            detail=f"actual: {actual}",
        )

    @staticmethod
    def specialist_called_at_most(recorder: TraceRecorder, tool: str, n: int) -> AssertionResult:
        count = recorder.call_count(tool)
        return AssertionResult(
            name=f"{tool}_called_<={n}",
            passed=count <= n,
            detail=f"called {count} times",
        )

    @staticmethod
    def deliverable_nonempty(result: dict) -> AssertionResult:
        deliverable = result.get("deliverable", "")
        return AssertionResult(
            name="deliverable_nonempty",
            passed=bool(deliverable and len(deliverable.strip()) > 20),
            detail=f"length: {len(deliverable)}",
        )

    @staticmethod
    def tokens_under(result: dict, limit: int) -> AssertionResult:
        used = result.get("tokens_used", 0)
        return AssertionResult(
            name=f"tokens_under({limit})",
            passed=used < limit,
            detail=f"used: {used}",
        )

    @staticmethod
    def compliance_was_called_if_external(
        recorder: TraceRecorder, is_external: bool
    ) -> AssertionResult:
        if not is_external:
            return AssertionResult(name="compliance_if_external", passed=True, detail="n/a")
        called = "compliance_agent" in recorder.unique_specialists()
        return AssertionResult(
            name="compliance_if_external",
            passed=called,
            detail="compliance_agent called" if called else "MISSING for external content",
        )


class LLMJudge:
    JUDGE_PROMPT: str = ""
    JUDGE_MODEL: str = "claude-sonnet-4-6"

    def __init__(self, client):
        self.client = client

    def evaluate(self, content_under_review: str, context: dict | None = None) -> dict:
        prompt = self._build_prompt(content_under_review, context or {})
        response = self.client.messages.create(
            model=self.JUDGE_MODEL,
            max_tokens=1024,
            system=self.JUDGE_PROMPT,
            messages=[{"role": "user", "content": prompt}],
        )
        text = "".join(
            b.text for b in response.content if getattr(b, "type", "") == "text"
        )
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            return {"_parse_error": True, "raw": text, "overall_pass": False}

    def _build_prompt(self, content: str, context: dict) -> str:
        return f"Content to evaluate:\n---\n{content}\n---"


class BrandVoiceJudge(LLMJudge):
    JUDGE_PROMPT = f"""\
You evaluate marketing copy for {COMPANY_JUDGE} using the 5-check rewrite test.
Return ONLY JSON with checks (generic, vocabulary, you_we, specificity, read_aloud), \
score 0-5, overall_pass true if score >= 4, optional rewrite_suggestion.
"""


class FabricationJudge(LLMJudge):
    JUDGE_PROMPT = """\
Audit for fabricated statistics, quotes, or unsourced facts. Return JSON with \
fabricated_claims[], overall_pass (true if no high severity), notes.
"""


class RoutingEfficiencyJudge(LLMJudge):
    JUDGE_PROMPT = """\
Evaluate orchestration efficiency. Return JSON with appropriate_specialists, \
over_engineered, under_engineered, ideal_specialist_set, verdict \
(efficient|wasteful|insufficient), explanation, and overall_pass \
(true when verdict is efficient and not over_engineered).
"""

    def _build_prompt(self, content: str, context: dict) -> str:
        return (
            f"USER REQUEST:\n{context.get('request', '')}\n\n"
            f"SPECIALISTS CALLED:\n{context.get('specialists', [])}\n\n"
            f"FINAL DELIVERABLE:\n{content}\n"
        )

    def evaluate(self, content_under_review: str, context: dict | None = None) -> dict:
        result = super().evaluate(content_under_review, context)
        if "overall_pass" not in result and "verdict" in result:
            result["overall_pass"] = result.get("verdict") == "efficient" and not result.get(
                "over_engineered", True
            )
        return result


def judge_passed(name: str, result: dict) -> bool:
    if result.get("_parse_error"):
        return False
    return bool(result.get("overall_pass", False))


@dataclass
class CaseOutcome:
    case_id: str
    case_name: str
    passed: bool
    assertions: list[AssertionResult]
    judge_results: dict[str, dict]
    tokens_used: int
    wall_time_sec: float
    escalated: bool
    specialists_called: list[str]
    deliverable: str
    trace_path: str = ""


class Runner:
    def __init__(self, mode: str, client=None, traces_dir: str = "./traces"):
        self.mode = mode
        self.client = client
        self.traces_dir = Path(traces_dir)
        self.traces_dir.mkdir(parents=True, exist_ok=True)
        self.run_id = time.strftime("%Y%m%d-%H%M%S")

    def run(self, cases: list[EvalCase]) -> list[CaseOutcome]:
        outcomes: list[CaseOutcome] = []
        for i, case in enumerate(cases):
            logger.info("[%d/%d] %s — %s", i + 1, len(cases), case.id, case.name)
            try:
                outcome = self._run_case(case)
            except Exception as e:
                logger.exception("case %s crashed", case.id)
                outcome = CaseOutcome(
                    case_id=case.id,
                    case_name=case.name,
                    passed=False,
                    assertions=[AssertionResult("crashed", False, str(e))],
                    judge_results={},
                    tokens_used=0,
                    wall_time_sec=0,
                    escalated=False,
                    specialists_called=[],
                    deliverable="",
                )
            outcomes.append(outcome)
            logger.info(
                "  → %s (tokens=%d, time=%.1fs)",
                "PASS" if outcome.passed else "FAIL",
                outcome.tokens_used,
                outcome.wall_time_sec,
            )
        return outcomes

    def _run_case(self, case: EvalCase) -> CaseOutcome:
        from unittest.mock import MagicMock

        recorder = TraceRecorder()
        if self.mode == "mocked":
            client = MagicMock()
            director = InstrumentedDirector(
                client=client,
                recorder=recorder,
                brand_memory_loader=lambda topic: {"topic": topic, "_mocked": True},
                human_review_handler=lambda req: {
                    "_mocked": True,
                    "review_id": "mock",
                    "status": "queued",
                },
            )
            install_mocks(director, DEFAULT_CANNED_OUTPUTS | (case.canned_overrides or {}))
            self._install_director_script(director, case)
        else:
            director = InstrumentedDirector(client=self.client, recorder=recorder)

        result = director.handle_request(case.input_request)
        assertions = self._run_assertions(case, recorder, result)

        judge_results: dict[str, dict] = {}
        if self.mode == "full" and case.judges:
            judge_results = self._run_judges(case, result, recorder)

        trace_path = self.traces_dir / f"{self.run_id}_{case.id}.jsonl"
        trace_path.write_text(recorder.to_jsonl(), encoding="utf-8")

        struct_ok = all(a.passed for a in assertions)
        judges_ok = all(judge_passed(n, j) for n, j in judge_results.items())
        passed = struct_ok and judges_ok

        return CaseOutcome(
            case_id=case.id,
            case_name=case.name,
            passed=passed,
            assertions=assertions,
            judge_results=judge_results,
            tokens_used=result.get("tokens_used", 0),
            wall_time_sec=result.get("wall_time_sec", 0),
            escalated=result.get("escalated", False),
            specialists_called=recorder.specialists_called(),
            deliverable=result.get("deliverable", ""),
            trace_path=str(trace_path),
        )

    def _install_director_script(
        self, director: InstrumentedDirector, case: EvalCase
    ) -> None:
        from unittest.mock import MagicMock

        def make_response(content_blocks, stop_reason):
            r = MagicMock()
            r.content = content_blocks
            r.stop_reason = stop_reason
            r.usage = MagicMock(input_tokens=100, output_tokens=100)
            return r

        def make_tool_use(name: str, input_data: dict | None = None):
            b = MagicMock()
            b.type = "tool_use"
            b.name = name
            b.input = input_data or {"_test": True}
            b.id = f"toolu_{uuid.uuid4().hex[:8]}"
            return b

        def make_text(text: str):
            b = MagicMock()
            b.type = "text"
            b.text = text
            return b

        sequence = build_mock_tool_sequence(case)
        script = []
        for tool in sequence:
            script.append(make_response([make_tool_use(tool)], "tool_use"))
        script.append(
            make_response(
                [make_text(f"Final deliverable for case {case.id}: mocked synthesis.")],
                "end_turn",
            )
        )
        director.client.messages.create.side_effect = script

    def _run_assertions(
        self, case: EvalCase, recorder: TraceRecorder, result: dict
    ) -> list[AssertionResult]:
        out: list[AssertionResult] = []
        if case.expected_specialists is not None:
            out.append(Assertions.specialists_include(recorder, case.expected_specialists))
        if case.forbidden_specialists is not None:
            out.append(Assertions.specialists_exclude(recorder, case.forbidden_specialists))
        if case.should_escalate is not None:
            out.append(Assertions.escalated(recorder, should=case.should_escalate))
        if case.is_external_facing is not None:
            out.append(
                Assertions.compliance_was_called_if_external(recorder, case.is_external_facing)
            )
        if case.max_tokens is not None and self.mode != "mocked":
            out.append(Assertions.tokens_under(result, case.max_tokens))
        out.append(Assertions.deliverable_nonempty(result))
        if case.id != "failure_loop_cap":
            for sp in recorder.unique_specialists():
                out.append(Assertions.specialist_called_at_most(recorder, sp, 3))
        return out

    def _run_judges(
        self, case: EvalCase, result: dict, recorder: TraceRecorder
    ) -> dict[str, dict]:
        deliverable = result.get("deliverable", "")
        ctx = {"request": case.input_request, "specialists": recorder.specialists_called()}
        judges: dict[str, dict] = {}
        if "brand_voice" in case.judges:
            judges["brand_voice"] = BrandVoiceJudge(self.client).evaluate(deliverable)
        if "fabrication" in case.judges:
            judges["fabrication"] = FabricationJudge(self.client).evaluate(deliverable)
        if "routing_efficiency" in case.judges:
            judges["routing_efficiency"] = RoutingEfficiencyJudge(self.client).evaluate(
                deliverable, ctx
            )
        return judges


def render_text_report(outcomes: list[CaseOutcome]) -> str:
    lines = []
    passed = sum(1 for o in outcomes if o.passed)
    total_tokens = sum(o.tokens_used for o in outcomes)
    total_time = sum(o.wall_time_sec for o in outcomes)
    lines.append("=" * 72)
    lines.append(f"EVAL RESULTS: {passed}/{len(outcomes)} passed")
    lines.append(f"Total tokens: {total_tokens:,}   Total time: {total_time:.1f}s")
    lines.append("=" * 72)
    for o in outcomes:
        marker = "PASS" if o.passed else "FAIL"
        lines.append(f"\n{marker} [{o.case_id}] {o.case_name}")
        lines.append(
            f"    tokens={o.tokens_used}  time={o.wall_time_sec:.1f}s  "
            f"escalated={o.escalated}  specialists={o.specialists_called}"
        )
        for a in o.assertions:
            am = "  ok" if a.passed else "  FAIL"
            lines.append(f"    {am} {a.name}: {a.detail}")
        for jname, jres in o.judge_results.items():
            jm = "  ok" if judge_passed(jname, jres) else "  FAIL"
            lines.append(f"    {jm} judge[{jname}]: {json.dumps(jres)[:200]}")
    return "\n".join(lines)


def render_html_report(outcomes: list[CaseOutcome], run_id: str) -> str:
    rows = []
    for o in outcomes:
        color = "#1a7f37" if o.passed else "#cf222e"
        assertion_rows = "".join(
            f"<li>{'ok' if a.passed else 'FAIL'} <b>{a.name}</b>: {a.detail}</li>"
            for a in o.assertions
        )
        judge_rows = "".join(
            f"<li><b>{name}</b>: {json.dumps(r)[:300]}</li>"
            for name, r in o.judge_results.items()
        )
        rows.append(
            f"<tr><td><b>{o.case_id}</b></td><td style='color:{color}'>{'PASS' if o.passed else 'FAIL'}</td>"
            f"<td>{o.tokens_used}</td><td>{o.wall_time_sec:.1f}s</td>"
            f"<td>{', '.join(o.specialists_called)}</td>"
            f"<td><ul>{assertion_rows}</ul></td>"
            f"<td><ul>{judge_rows or '<i>n/a</i>'}</ul></td></tr>"
        )
    passed = sum(1 for o in outcomes if o.passed)
    return f"""<!doctype html><html><head><meta charset='utf-8'><title>Eval {run_id}</title></head>
<body><h1>Marketing Director Eval — {run_id}</h1><p>{passed}/{len(outcomes)} passed</p>
<table border='1' cellpadding='6'><tr><th>Case</th><th>Result</th><th>Tokens</th><th>Time</th>
<th>Specialists</th><th>Assertions</th><th>Judges</th></tr>{''.join(rows)}</table></body></html>"""


def main() -> None:
    parser = argparse.ArgumentParser(description="Marketing Director eval harness")
    parser.add_argument("--mode", choices=["mocked", "smoke", "full"], default="mocked")
    parser.add_argument("--case", help="Run single case by id")
    parser.add_argument("--report", help="Write report (.txt or .html)")
    parser.add_argument("--traces-dir", default="./traces")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
        format="%(asctime)s %(levelname)s %(message)s",
    )

    cases = list(EVAL_CASES)
    if args.case:
        cases = [c for c in cases if c.id == args.case]
        if not cases:
            print(f"No case with id={args.case}", file=sys.stderr)
            sys.exit(2)
    if args.mode == "smoke":
        cases = [c for c in cases if c.smoke]

    client = None
    if args.mode in {"smoke", "full"}:
        try:
            from anthropic import Anthropic
        except ImportError:
            print("pip install anthropic", file=sys.stderr)
            sys.exit(2)
        if not os.environ.get("ANTHROPIC_API_KEY"):
            print("ANTHROPIC_API_KEY not set", file=sys.stderr)
            sys.exit(2)
        client = Anthropic()

    runner = Runner(mode=args.mode, client=client, traces_dir=args.traces_dir)
    outcomes = runner.run(cases)
    print(render_text_report(outcomes))
    if args.report:
        path = Path(args.report)
        path.write_text(
            render_html_report(outcomes, runner.run_id)
            if path.suffix == ".html"
            else render_text_report(outcomes),
            encoding="utf-8",
        )
        print(f"\nReport written to {path}")

    sys.exit(0 if all(o.passed for o in outcomes) else 1)


if __name__ == "__main__":
    main()
