"""Profile reference WAV files into spectral_references."""

from __future__ import annotations

from pathlib import Path

import numpy as np
from scipy.io import wavfile

from disklordz import audit
from disklordz.db import connect, init_db, insert_reference


def load_mono(path: Path) -> tuple[np.ndarray, int]:
    sr, data = wavfile.read(path)
    if data.ndim > 1:
        data = data.mean(axis=1)
    if data.dtype == np.int16:
        audio = data.astype(np.float64) / 32768.0
    elif data.dtype == np.int32:
        audio = data.astype(np.float64) / 2147483648.0
    else:
        audio = data.astype(np.float64)
    return audio, int(sr)


def profile_file(path: Path) -> dict[str, float]:
    audio, sr = load_mono(path)
    return audit.fingerprint(audio, sr)


def profile_directory(db_path: Path, folder: Path) -> int:
    init_db(db_path)
    count = 0
    with connect(db_path) as conn:
        for wav in sorted(folder.glob("**/*.wav")):
            metrics = profile_file(wav)
            insert_reference(conn, wav.stem, str(wav.resolve()), metrics)
            count += 1
        conn.commit()
    return count
