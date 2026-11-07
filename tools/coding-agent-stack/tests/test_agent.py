from coding_agent_stack.graph import run_agent_stub


def test_stub_agent_harness_keyword():
    result = run_agent_stub("run agent tests")
    assert result.mode == "stub"
    assert result.tool_calls >= 1
    assert "harness" in result.final_text.lower() or "pytest" in result.final_text.lower()
