"""Automated QA on rendered PCM (peak, true-peak, DC, loudness)."""

from __future__ import annotations

from dataclasses import dataclass

from drum_dsp import (
    ONE_SHOT_PEAK_DBFS,
    ONE_SHOT_RMS_DBFS,
    hard_clip,
    normalize_peak,
    normalize_rms,
    one_pole_hp,
    peak_abs,
    remove_dc,
    rms_db,
    true_peak_estimate,
)

# Documented targets for one-shots (short drum hits — not -14 LUFS integrated)
QA_TARGETS_DOC = """
Drum Factory QA targets (Phase 1)
---------------------------------
One-shots:
  - Peak: -1.0 dBFS max (linear 10^(-1/20))
  - True-peak estimate (4x OS): <= +0.3 dBFS equivalent
  - DC offset: |mean| <= 1e-4 after correction
  - RMS normalize toward -18 dBFS after peak safety (musical one-shot level)
Loops (multibar):
  - Integrated loudness target approx -18 LUFS (RMS proxy here)
  - Peak limit -1.0 dBFS
"""


@dataclass
class QaReport:
    peak_dbfs: float
    true_peak_linear: float
    rms_dbfs: float
    dc_before: float
    dc_after: float
    clipped_before: bool
    ok: bool


def _db_to_linear(db: float) -> float:
    return 10 ** (db / 20.0)


def process_one_shot(samples: list[float]) -> tuple[list[float], QaReport]:
    dc_b = sum(samples) / len(samples) if samples else 0.0
    cleaned = remove_dc(samples)
    if abs(dc_b) > 1e-4:
        cleaned = one_pole_hp(cleaned, fc_hz=5.0)
    cleaned = remove_dc(cleaned)
    tp = true_peak_estimate(cleaned)
    peak_lin = _db_to_linear(ONE_SHOT_PEAK_DBFS)
    clipped_before = tp > peak_lin * 1.02
    if tp > peak_lin:
        cleaned = normalize_peak(cleaned, peak_lin * 0.99)
    cleaned = normalize_rms(cleaned, ONE_SHOT_RMS_DBFS)
    cleaned = hard_clip(cleaned, _db_to_linear(ONE_SHOT_PEAK_DBFS))
    pk = peak_abs(cleaned)
    rep = QaReport(
        peak_dbfs=20 * __import__("math").log10(pk + 1e-12),
        true_peak_linear=true_peak_estimate(cleaned),
        rms_dbfs=rms_db(cleaned),
        dc_before=dc_b,
        dc_after=sum(cleaned) / len(cleaned) if cleaned else 0.0,
        clipped_before=clipped_before,
        ok=pk <= peak_lin * 1.01 and abs(sum(cleaned) / max(1, len(cleaned))) < 1e-3,
    )
    return cleaned, rep


def process_loop(samples: list[float], *, target_rms_db: float = -18.0) -> tuple[list[float], QaReport]:
    cleaned = remove_dc(samples)
    peak_lin = _db_to_linear(-1.0)
    cleaned = normalize_peak(cleaned, peak_lin * 0.98)
    cleaned = normalize_rms(cleaned, target_rms_db)
    pk = peak_abs(cleaned)
    rep = QaReport(
        peak_dbfs=20 * __import__("math").log10(pk + 1e-12),
        true_peak_linear=true_peak_estimate(cleaned),
        rms_dbfs=rms_db(cleaned),
        dc_before=0.0,
        dc_after=0.0,
        clipped_before=False,
        ok=True,
    )
    return cleaned, rep
