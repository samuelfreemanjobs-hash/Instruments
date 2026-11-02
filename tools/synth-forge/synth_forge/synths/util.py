"""Binary helpers shared by hardware adapters."""

from __future__ import annotations

import struct

from synth_forge.models import SynthParameters

PARAM_FLOATS: tuple[str, ...] = (
    "osc1_wave",
    "osc2_wave",
    "filter_cutoff",
    "filter_resonance",
    "amp_attack",
    "amp_decay",
    "amp_sustain",
    "amp_release",
    "lfo_rate",
    "lfo_depth",
    "delay_feedback",
    "master_volume",
    "detune",
    "pitch_glide",
)


def crc16_ccitt(data: bytes, seed: int = 0xFFFF) -> int:
    crc = seed
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def params_to_payload(params: SynthParameters) -> bytes:
    values = [float(getattr(params, name)) for name in PARAM_FLOATS]
    category = params.category.encode("utf-8", errors="replace")[:32].ljust(32, b"\x00")
    return struct.pack(f"<{len(values)}f", *values) + category


def payload_to_params(payload: bytes, category_default: str = "generic") -> SynthParameters:
    float_size = 4 * len(PARAM_FLOATS)
    if len(payload) < float_size:
        raise ValueError("payload too short for parameter block")
    values = struct.unpack(f"<{len(PARAM_FLOATS)}f", payload[:float_size])
    category = category_default
    if len(payload) >= float_size + 32:
        raw = payload[float_size : float_size + 32].split(b"\x00", 1)[0]
        if raw:
            category = raw.decode("utf-8", errors="replace")
    data = dict(zip(PARAM_FLOATS, values))
    data["category"] = category
    return SynthParameters.model_validate(data)


def embed_params_block(blob: bytearray, offset: int, params: SynthParameters) -> None:
    block = params_to_payload(params)
    end = offset + len(block)
    if end > len(blob):
        raise ValueError("parameter block exceeds blob size")
    blob[offset:end] = block
