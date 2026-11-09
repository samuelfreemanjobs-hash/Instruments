from __future__ import annotations

from typing import Any


def validate_graph(graph: dict[str, Any]) -> tuple[bool, list[str]]:
    errors: list[str] = []
    nodes = {n["id"]: n for n in graph.get("nodes", []) if "id" in n}
    edges = graph.get("edges", [])
    if not nodes:
        errors.append("no nodes defined")
        return False, errors
    if "output" not in [n.get("type") for n in nodes.values()]:
        errors.append("missing output node type")
    for edge in edges:
        src, dst = edge.get("from"), edge.get("to")
        if src not in nodes:
            errors.append(f"unknown edge source: {src}")
        if dst not in nodes:
            errors.append(f"unknown edge dest: {dst}")
    # Simple cycle check (DFS)
    adj: dict[str, list[str]] = {nid: [] for nid in nodes}
    for edge in edges:
        if edge.get("from") in adj and edge.get("to"):
            adj[edge["from"]].append(edge["to"])
    visited: set[str] = set()
    stack: set[str] = set()

    def dfs(n: str) -> bool:
        visited.add(n)
        stack.add(n)
        for m in adj.get(n, []):
            if m not in visited:
                if dfs(m):
                    return True
            elif m in stack:
                return True
        stack.remove(n)
        return False

    for nid in nodes:
        if nid not in visited and dfs(nid):
            errors.append("cycle detected in signal graph")
            break
    return len(errors) == 0, errors
