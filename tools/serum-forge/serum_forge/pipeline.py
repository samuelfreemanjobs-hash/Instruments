"""Five-stage autonomous pipeline (symbolic + optional binary/render)."""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path

from serum_forge.compiler import CompilerNotConfiguredError, compile_serum_preset, write_intermediate_json
from serum_forge.guardrails import clamp_patch
from serum_forge.models import PatchArchetype, SerumSymbolPatch
from serum_forge.semantic import intent_to_patch


@dataclass
class PipelineResult:
    patch: SerumSymbolPatch
    intermediate_json: Path | None = None
    serum_preset: Path | None = None
    render_wav: Path | None = None
    evaluation: dict = field(default_factory=dict)
    warnings: list[str] = field(default_factory=list)


def run_pipeline(
    prompt: str,
    *,
    archetype: PatchArchetype = PatchArchetype.pad_ambient,
    out_dir: Path,
    compile_binary: bool = False,
    render: bool = False,
) -> PipelineResult:
    out_dir.mkdir(parents=True, exist_ok=True)
    warnings: list[str] = []

    # 1 Intent ingestion
    patch = intent_to_patch(prompt, archetype=archetype)

    # 2 Symbolic assembly + guardrails
    patch = clamp_patch(patch)

    # 3 Binary compilation
    json_path = out_dir / f"{patch.name.replace(' ', '_')}.serum-forge.json"
    write_intermediate_json(patch, json_path)
    preset_path = None
    if compile_binary:
        try:
            preset_path = out_dir / f"{patch.name.replace(' ', '_')}.SerumPreset"
            compile_serum_preset(patch, preset_path)
        except CompilerNotConfiguredError as exc:
            warnings.append(str(exc))

    # 4 Headless render (optional external)
    wav_path = None
    if render:
        try:
            from serum_forge.render import render_preset_headless

            if preset_path and preset_path.is_file():
                wav_path = render_preset_headless(preset_path, out_dir / "preview.wav")
        except Exception as exc:  # noqa: BLE001 — surface to caller via warnings
            warnings.append(f"render skipped: {exc}")

    # 5 Evaluation placeholder (CLAP / perceptual loss wired when deps present)
    evaluation = {"status": "symbolic_only", "prompt": prompt[:200]}

    return PipelineResult(
        patch=patch,
        intermediate_json=json_path,
        serum_preset=preset_path,
        render_wav=wav_path,
        evaluation=evaluation,
        warnings=warnings,
    )
