"""Spike: emit DspGraph stub from validated graph (behind approve_architecture)."""

from __future__ import annotations

from typing import Any


def codegen_stub(graph: dict[str, Any], plugin_name: str) -> str:
    nodes = graph.get("nodes", [])
    lines = [
        "#pragma once",
        f"// Auto-generated spike for {plugin_name} — hand-wire in Source/Dsp/",
        "namespace factory {",
        "struct DspGraph {",
        "    void prepare(double sampleRate, int blockSize) { (void)sampleRate; (void)blockSize; }",
        "    void process(float* const* channels, int numChannels, int numSamples) {",
        "        (void)channels; (void)numChannels; (void)numSamples;",
    ]
    for n in nodes:
        lines.append(f"        // node {n.get('id')}: {n.get('type')}")
    lines.extend(["    }", "};", "}"])
    return "\n".join(lines) + "\n"
