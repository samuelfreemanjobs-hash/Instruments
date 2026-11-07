from __future__ import annotations

import json
from dataclasses import dataclass, field
from typing import Any, Literal, TypedDict

from coding_agent_stack.config import get_settings
from coding_agent_stack.tools import TOOL_REGISTRY, dispatch_tool, openai_tools_spec, stub_plan_for_prompt


class MessageDict(TypedDict, total=False):
    role: Literal["system", "user", "assistant", "tool"]
    content: str
    tool_call_id: str
    name: str


class AgentState(TypedDict, total=False):
    messages: list[MessageDict]
    turns: int


SYSTEM_PROMPT = """You are a remote coding agent for an Instruments/JUCE monorepo.
Use tools to read the repo, run allowlisted shell, execute Python in a sandbox, and run VST harness compiles.
After code changes (made elsewhere), prefer harness_vst_compile and read vst-testing-ops/error_log.txt via read_file.
Be concise; cite file paths. Do not invent APIs."""


@dataclass
class AgentRunResult:
    messages: list[MessageDict] = field(default_factory=list)
    final_text: str = ""
    mode: str = "stub"
    tool_calls: int = 0


def _invoke_openai(messages: list[dict[str, Any]]) -> Any:
    from langchain_openai import ChatOpenAI

    settings = get_settings()
    kwargs: dict[str, Any] = {
        "model": settings.agent_model,
        "temperature": 0,
    }
    if settings.openai_base_url:
        kwargs["base_url"] = settings.openai_base_url
    model = ChatOpenAI(api_key=settings.openai_api_key or None, **kwargs)
    return model.bind_tools(openai_tools_spec()).invoke(messages)


def _messages_to_lc(messages: list[MessageDict]) -> list[Any]:
    from langchain_core.messages import AIMessage, HumanMessage, SystemMessage, ToolMessage

    out: list[Any] = []
    for m in messages:
        role = m.get("role", "user")
        content = m.get("content", "")
        if role == "system":
            out.append(SystemMessage(content=content))
        elif role == "user":
            out.append(HumanMessage(content=content))
        elif role == "assistant":
            out.append(AIMessage(content=content))
        elif role == "tool":
            out.append(
                ToolMessage(content=content, tool_call_id=m.get("tool_call_id", ""))
            )
    return out


def run_agent_llm(prompt: str, thread_id: str = "default") -> AgentRunResult:
    settings = get_settings()
    if not settings.openai_api_key and not settings.anthropic_api_key:
        return run_agent_stub(prompt)

    messages: list[MessageDict] = [
        {"role": "system", "content": SYSTEM_PROMPT},
        {"role": "user", "content": prompt},
    ]
    tool_calls = 0
    max_turns = settings.agent_max_turns

    for _ in range(max_turns):
        lc_msgs = _messages_to_lc(messages)
        if settings.openai_api_key:
            ai = _invoke_openai(lc_msgs)
        else:
            return run_agent_stub(prompt)

        content = ai.content if isinstance(ai.content, str) else str(ai.content)
        raw_tools = getattr(ai, "tool_calls", None) or []
        if not raw_tools:
            return AgentRunResult(
                messages=messages + [{"role": "assistant", "content": content}],
                final_text=content,
                mode="llm",
                tool_calls=tool_calls,
            )

        messages.append({"role": "assistant", "content": content or ""})
        for tc in raw_tools:
            name = tc.get("name") or tc.get("type")
            args = tc.get("args") or {}
            tid = tc.get("id", name)
            if isinstance(args, str):
                args = json.loads(args) if args.startswith("{") else {"code": args}
            result = dispatch_tool(name, args)
            tool_calls += 1
            messages.append(
                {
                    "role": "tool",
                    "content": result,
                    "tool_call_id": tid,
                    "name": name,
                }
            )

    return AgentRunResult(
        messages=messages,
        final_text="Stopped: max turns reached.",
        mode="llm",
        tool_calls=tool_calls,
    )


def run_agent_stub(prompt: str) -> AgentRunResult:
    """Deterministic ReAct-shaped loop without external LLM (free / CI)."""
    messages: list[MessageDict] = [
        {"role": "system", "content": SYSTEM_PROMPT},
        {"role": "user", "content": prompt},
    ]
    plan = stub_plan_for_prompt(prompt)
    summaries: list[str] = []
    tool_calls = 0
    for name, args in plan:
        result = dispatch_tool(name, args)
        tool_calls += 1
        messages.append({"role": "assistant", "content": f"Calling tool {name}"})
        messages.append(
            {
                "role": "tool",
                "content": result,
                "tool_call_id": name,
                "name": name,
            }
        )
        summaries.append(f"**{name}**\n```\n{result[:4000]}\n```")

    final = (
        "Stub agent finished (set OPENAI_API_KEY for full reasoning).\n\n"
        + "\n\n".join(summaries)
    )
    messages.append({"role": "assistant", "content": final})
    return AgentRunResult(
        messages=messages, final_text=final, mode="stub", tool_calls=tool_calls
    )


def try_compile_langgraph():
    """Optional: export a LangGraph StateGraph when langgraph is installed."""
    try:
        from langgraph.graph import END, START, StateGraph
        from typing import Annotated
        from langgraph.graph.message import add_messages
    except ImportError:
        return None

    class GState(TypedDict):
        messages: Annotated[list, add_messages]

    def agent_node(state: GState):
        last = state["messages"][-1]["content"]
        return {"messages": run_agent_stub(last).messages}

    g = StateGraph(GState)
    g.add_node("agent", agent_node)
    g.add_edge(START, "agent")
    g.add_edge("agent", END)
    return g.compile()
