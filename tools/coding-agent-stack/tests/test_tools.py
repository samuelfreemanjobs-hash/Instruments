from coding_agent_stack.sandbox import LocalSandbox
from coding_agent_stack.tools import dispatch_tool, tool_read_file


def test_sandbox_python():
    sb = LocalSandbox()
    r = sb.run_python("print(2 + 2)")
    assert r.exit_code == 0
    assert "4" in r.stdout


def test_read_architecture():
    text = tool_read_file("ARCHITECTURE.md", max_bytes=2000)
    assert "Instruments" in text or "repository" in text.lower()


def test_dispatch_unknown():
    assert "unknown tool" in dispatch_tool("nope", {})
