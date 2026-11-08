"""Build Disklordz drum kit manifests from local WAV folders (Phonk Drum Studio exports)."""

from __future__ import annotations

import hashlib
import json
import re
import uuid
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, Iterable, List, Mapping, Optional, Sequence

import numpy as np

try:
    import soundfile as sf
except ImportError:  # pragma: no cover
    sf = None

SAMPLE_RATE = 44100

# Keep slot names aligned with website/src/lib/generation/synth.ts SAMPLE_NAMES
KIT_SLOTS: Sequence[str] = (
    "kick",
    "snare",
    "hat_closed",
    "hat_open",
    "rim",
    "clap",
)

PRESET_ARTIST_LANE: Dict[str, str] = {
    "boulevard-86": "DL001",
    "midnight-circuit": "DL002",
    "disklordz-screw": "DL006",
    "terminal-mirage": "DL004",
    "mpc-ready-808": "SUPPLY",
}

SLOT_FILE_ALIASES: Dict[str, Sequence[str]] = {
    "kick": ("kick", "kick_808", "bd", "808", "kick_vae"),
    "snare": ("snare", "sd", "snr", "snare_vae"),
    "hat_closed": ("hat_closed", "hihat_closed", "ch", "closed_hat", "hat-closed"),
    "hat_open": ("hat_open", "hihat_open", "oh", "open_hat", "hat-open"),
    "rim": ("rim", "rimshot", "rs", "rim_vae"),
    "clap": ("clap", "cp", "handclap"),
}

PROVENANCE = "factory_studio_v1"


@dataclass
class ResolvedSlot:
    name: str
    source_path: Path


def _sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _load_map_file(map_path: Optional[Path]) -> Dict[str, str]:
    if map_path is None or not map_path.is_file():
        return {}
    data = json.loads(map_path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("studio map JSON must be an object of slot → filename")
    return {str(k): str(v) for k, v in data.items()}


def _index_wavs(samples_dir: Path) -> Dict[str, Path]:
    index: Dict[str, Path] = {}
    for path in sorted(samples_dir.rglob("*")):
        if not path.is_file():
            continue
        if path.suffix.lower() not in {".wav", ".aif", ".aiff"}:
            continue
        index[path.stem.lower()] = path
        index[path.name.lower()] = path
    return index


def resolve_slot_sources(
    samples_dir: Path,
    *,
    map_path: Optional[Path] = None,
    explicit_map: Optional[Mapping[str, str]] = None,
) -> List[ResolvedSlot]:
    """Map kit slots to WAV paths under samples_dir."""
    if not samples_dir.is_dir():
        raise FileNotFoundError(f"Samples directory not found: {samples_dir}")

    file_map = _load_map_file(map_path)
    if explicit_map:
        file_map.update({str(k): str(v) for k, v in explicit_map.items()})

    wav_index = _index_wavs(samples_dir)
    resolved: List[ResolvedSlot] = []

    for slot in KIT_SLOTS:
        path: Optional[Path] = None
        if slot in file_map:
            candidate = samples_dir / file_map[slot]
            if candidate.is_file():
                path = candidate
        if path is None:
            for alias in SLOT_FILE_ALIASES.get(slot, (slot,)):
                if alias in wav_index:
                    path = wav_index[alias]
                    break
        if path is None:
            pattern = re.compile(rf"(^|[_-]){re.escape(slot)}([_-]|$)", re.I)
            for key, candidate in wav_index.items():
                if pattern.search(key):
                    path = candidate
                    break
        if path is not None:
            resolved.append(ResolvedSlot(name=slot, source_path=path))

    return resolved


def _to_mono_float(data: np.ndarray) -> np.ndarray:
    if data.ndim == 1:
        return data.astype(np.float32, copy=False)
    return data.mean(axis=1).astype(np.float32)


def normalize_wav_for_kit(src: Path, dst: Path, target_sr: int = SAMPLE_RATE) -> None:
    """Write 44.1 kHz 16-bit PCM mono WAV for factory kits."""
    if sf is None:
        raise ImportError("soundfile is required to build studio kits")

    data, sr = sf.read(str(src), always_2d=True, dtype="float32")
    mono = _to_mono_float(data)
    if sr != target_sr and mono.size > 0:
        duration = mono.shape[0] / float(sr)
        target_len = max(1, int(round(duration * target_sr)))
        x_old = np.linspace(0.0, duration, num=mono.shape[0], endpoint=False)
        x_new = np.linspace(0.0, duration, num=target_len, endpoint=False)
        mono = np.interp(x_new, x_old, mono).astype(np.float32)

    peak = float(np.max(np.abs(mono))) if mono.size else 0.0
    if peak > 1e-8:
        mono = (mono * (0.89 / peak)).astype(np.float32)

    dst.parent.mkdir(parents=True, exist_ok=True)
    sf.write(dst, mono, target_sr, subtype="PCM_16")


def default_generation_spec(preset_id: str) -> Dict[str, Any]:
    bpm = 140 if preset_id == "midnight-circuit" else 96
    if preset_id == "disklordz-screw":
        bpm = 72
    return {
        "mode": "one_shot",
        "engine": "studio",
        "key": "F#min",
        "bpm": bpm,
        "bars": 2,
        "length": "short",
        "stereo": 0,
        "wildness": 0.35,
    }


def build_kit_from_samples(
    *,
    preset_id: str,
    prompt: str,
    samples_dir: Path,
    out_dir: Path,
    map_path: Optional[Path] = None,
    stub_root: Optional[Path] = None,
    vae_checkpoint: Optional[str] = None,
) -> Path:
    """
    Copy/normalize resolved samples into out_dir and write manifest.json.

    Missing slots are optionally filled from stub_root/<preset>/<slot>.wav (website public samples).
    """
    artist_lane = PRESET_ARTIST_LANE.get(preset_id, "SUPPLY")
    kit_id = str(uuid.uuid4())
    out_dir.mkdir(parents=True, exist_ok=True)

    resolved = {r.name: r for r in resolve_slot_sources(samples_dir, map_path=map_path)}
    manifest_samples: List[Dict[str, Any]] = []

    for slot in KIT_SLOTS:
        dst = out_dir / f"{slot}.wav"
        src = resolved.get(slot)
        if src is None and stub_root:
            stub = stub_root / preset_id / f"{slot}.wav"
            if stub.is_file():
                src_path = stub
                source_kind = "stub_factory_v0"
            else:
                src_path = None
                source_kind = None
        else:
            src_path = src.source_path if src else None
            source_kind = "phonk_studio_vae" if src else None

        if src_path is None:
            raise ValueError(
                f"No WAV found for slot '{slot}' in {samples_dir} "
                f"(and no stub under {stub_root})"
            )

        normalize_wav_for_kit(src_path, dst)
        sha = _sha256_file(dst)
        rel = dst.name
        source_id = (
            f"phonk-studio-vae://{preset_id}/{slot}?sha={sha[:16]}"
            if source_kind == "phonk_studio_vae"
            else f"factory-stub://{preset_id}/{slot}"
        )
        manifest_samples.append(
            {
                "name": slot,
                "filename": rel,
                "url": f"/api/samples/{kit_id}/{rel}",
                "sha256": sha,
                "provenance": PROVENANCE if source_kind == "phonk_studio_vae" else "stub_factory_v0",
                "sourceId": source_id,
            }
        )

    manifest: Dict[str, Any] = {
        "format": "DISKLORDZ_DRUM_KIT_MANIFEST",
        "version": 1,
        "kitId": kit_id,
        "createdAt": datetime.now(timezone.utc).isoformat(),
        "prompt": prompt.strip(),
        "presetId": preset_id,
        "artistLane": artist_lane,
        "sampleRate": SAMPLE_RATE,
        "samples": manifest_samples,
        "license": "personal_and_commercial_v0_preview",
        "storageBackend": "local",
        "generationSpec": default_generation_spec(preset_id),
        "factoryParams": {
            "phonkStudio": True,
            "vaeCheckpoint": vae_checkpoint or "",
            "samplesDir": str(samples_dir.resolve()),
        },
    }
    manifest_path = out_dir / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return manifest_path
