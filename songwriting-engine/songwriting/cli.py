"""Command-line entry point for project generation and MIDI validation."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import replace
from datetime import datetime, timezone
from importlib.metadata import version
from pathlib import Path

import mido

from .generator import generate_project, load_config
from .midi import export_midi, inspect_midi, load_project, save_project, validate_midi
from .mpc import export_mpc500
from .prompt import config_from_prompt


def _slug(value: str) -> str:
    return re.sub(r"[^A-Za-z0-9_-]+", "_", value).strip("_") or "song"


def _generate_from_config(config: dict, output_dir: Path, formats: list[int], mpc500_dir: Path | None = None) -> int:
    created_at = datetime.now(timezone.utc).isoformat(timespec="seconds")
    project = generate_project(config, created_at=created_at)
    output_dir.mkdir(parents=True, exist_ok=True)
    stem = _slug(project.title)
    project_path = save_path = output_dir / f"{stem}.json"
    reports = {}
    for midi_type in formats:
        midi_path = output_dir / f"{stem}_Type{midi_type}.mid"
        export_midi(project, midi_path, midi_type)
        report = validate_midi(midi_path, project, midi_type)
        reports[str(midi_type)] = report
        if not report["valid"]:
            save_project(replace(project, export=replace(project.export, validation_status="invalid")), project_path)
            raise ValueError(f"MIDI validation failed for {midi_path}: {report['failures']}")
    project = replace(project, export=replace(project.export, validation_status="valid"))
    save_project(project, project_path)
    report_path = output_dir / f"{stem}_validation.json"
    report_path.write_text(json.dumps(reports, indent=2) + "\n", encoding="utf-8")
    mpc500 = export_mpc500(project, mpc500_dir) if mpc500_dir else None
    print(json.dumps({
        "valid": True,
        "project": str(save_path.resolve()),
        "midi": [str((output_dir / f"{stem}_Type{midi_type}.mid").resolve()) for midi_type in formats],
        "report": str(report_path.resolve()),
        "bars": project.total_bars,
        "title": project.title,
        "tempo_bpm": project.tempo_bpm,
        "key": project.key,
        "scale": project.scale,
        "seed": project.seed,
        "style": project.style,
        "sections": [
            {"kind": section.kind, "start_bar": section.start_bar, "bars": section.bars}
            for section in project.sections
        ],
        "tracks": [{"name": track.name, "notes": len(track.notes)} for track in project.tracks],
        "id": project.id,
        "warnings": list(project.export.warnings),
        "mpc500": None if mpc500 is None else {"dir": str(mpc500_dir.resolve()), "code": mpc500["code"], "files": len(mpc500["files"])},
    }, indent=2))
    return 0


def _generate(args: argparse.Namespace) -> int:
    return _generate_from_config(load_config(args.config), args.output_dir, args.formats, args.mpc500_dir)


def _generate_prompt(args: argparse.Namespace) -> int:
    config = config_from_prompt(args.prompt, args.seed)
    return _generate_from_config(config, args.output_dir, args.formats, args.mpc500_dir)


def _export_mpc500(args: argparse.Namespace) -> int:
    manifest = export_mpc500(load_project(args.project), args.out, args.code)
    print(json.dumps({"valid": True, "dir": str(args.out.resolve()), "code": manifest["code"], "files": [f["file"] for f in manifest["files"]]}, indent=2))
    return 0


def _validate_midi(args: argparse.Namespace) -> int:
    report = inspect_midi(args.path)
    report["valid"] = report["unmatched_note_ons"] == 0 and report["unmatched_note_offs"] == 0
    if args.project:
        project = load_project(args.project)
        report = validate_midi(args.path, project, args.type)
    print(json.dumps(report, indent=2))
    return 0 if report["valid"] else 1


def _check(_: argparse.Namespace) -> int:
    print(json.dumps({"mido": version("mido"), "ready": True}))
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="songwriter", description="Generate and validate editable MIDI compositions")
    commands = parser.add_subparsers(dest="command", required=True)
    generate = commands.add_parser("generate", help="generate project JSON and MIDI files")
    generate.add_argument("--config", required=True, type=Path)
    generate.add_argument("--output-dir", default=Path("artifacts"), type=Path)
    generate.add_argument("--formats", nargs="+", type=int, choices=(0, 1), default=(0, 1))
    generate.add_argument("--mpc500-dir", type=Path, help="also write an MPC500 sequence package here")
    generate.set_defaults(handler=_generate)
    prompt_generate = commands.add_parser("generate-prompt", help="generate a composition from a natural-language brief")
    prompt_generate.add_argument("--prompt", required=True)
    prompt_generate.add_argument("--seed", type=int, default=48127)
    prompt_generate.add_argument("--output-dir", default=Path("artifacts"), type=Path)
    prompt_generate.add_argument("--formats", nargs="+", type=int, choices=(0, 1), default=(0, 1))
    prompt_generate.add_argument("--mpc500-dir", type=Path, help="also write an MPC500 sequence package here")
    prompt_generate.set_defaults(handler=_generate_prompt)
    mpc500 = commands.add_parser("export-mpc500", help="write MPC500 sequences (96 PPQ, section loops, per-track files) from a project")
    mpc500.add_argument("--project", required=True, type=Path)
    mpc500.add_argument("--out", required=True, type=Path)
    mpc500.add_argument("--code", help="file name prefix, up to 8 letters/digits")
    mpc500.set_defaults(handler=_export_mpc500)
    validate = commands.add_parser("validate-midi", help="inspect MIDI events and note pairing")
    validate.add_argument("path", type=Path)
    validate.add_argument("--project", type=Path)
    validate.add_argument("--type", type=int, choices=(0, 1), default=1)
    validate.set_defaults(handler=_validate_midi)
    commands.add_parser("check", help="check the MIDI library installation").set_defaults(handler=_check)
    args = parser.parse_args(argv)
    try:
        return args.handler(args)
    except (OSError, ValueError, KeyError) as error:
        print(json.dumps({"valid": False, "error": str(error)}), file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
