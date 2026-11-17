"""Original Memphis / phonk oneshot synthesizers for kit factory pipelines."""

from .phonk_kick import generate_memphis_phonk_kick, write_wav_24bit_mono
from .phonk_snare import generate_memphis_phonk_snare

__all__ = [
    "generate_memphis_phonk_kick",
    "generate_memphis_phonk_snare",
    "write_wav_24bit_mono",
]
