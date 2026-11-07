from coding_agent_stack.team_graph import run_team


def test_team_pipeline_stub():
    r = run_team("run agent tests", force_stub=True)
    assert r.phases[0] == "orchestrator"
    assert "architect" in r.phases
    assert r.tool_calls >= 1
