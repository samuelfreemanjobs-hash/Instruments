"""16-slot phonk bank layout for LIVEN Lofi-12 (one bank = slots 1–16)."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class SlotSpec:
    index: int  # 1–16 on the device UI
    key: str
    label: str
    drum_mode: bool = True


PHONK_BANK_A: tuple[SlotSpec, ...] = (
    SlotSpec(1, "kick_808", "808 kick", True),
    SlotSpec(2, "kick_dist", "Distorted 808", True),
    SlotSpec(3, "kick_sub", "Sub tail", True),
    SlotSpec(4, "snare_memphis", "Memphis snare", True),
    SlotSpec(5, "snare_rim", "Rim shot", True),
    SlotSpec(6, "clap", "Layer clap", True),
    SlotSpec(7, "hat_closed", "Closed hat", True),
    SlotSpec(8, "hat_open", "Open hat", True),
    SlotSpec(9, "hat_pedal", "Pedal / choke hat", True),
    SlotSpec(10, "cowbell_low", "Cowbell low", True),
    SlotSpec(11, "cowbell_high", "Cowbell high", True),
    SlotSpec(12, "tom_low", "Low tom", True),
    SlotSpec(13, "tom_hi", "High tom", True),
    SlotSpec(14, "perc_snap", "Perc snap", True),
    SlotSpec(15, "fx_impact", "Impact", False),
    SlotSpec(16, "fx_riser", "Riser / noise", False),
)


def filename_for_slot(spec: SlotSpec) -> str:
    return f"{spec.index:02d}_{spec.key}.wav"
