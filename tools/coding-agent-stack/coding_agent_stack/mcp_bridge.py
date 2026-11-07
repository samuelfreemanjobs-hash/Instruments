"""MCP integration notes and JSON tool export for external clients.

Desktop Cursor already runs MCP servers (Slack, Zapier, Supabase). This HTTP
service mirrors a subset of tools for mobile/PWA clients via REST + the same
schemas MCP would expose.
"""

from __future__ import annotations

from typing import Any

from coding_agent_stack.tools import TOOL_REGISTRY


def mcp_style_tool_catalog() -> list[dict[str, Any]]:
    """Serializable catalog for a future MCP adapter process."""
    return [
        {
            "name": name,
            "description": desc,
            "inputSchema": schema,
        }
        for name, (desc, _fn, schema) in TOOL_REGISTRY.items()
    ]
