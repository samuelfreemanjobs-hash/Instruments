# Oversampling

Nonlinear processing (saturation, folding) requires oversampling when running at base rate. 2× minimum for mild drive; 4× for aggressive waveshaping.

Downsample with linear-phase or acceptable minimum-phase filter; measure alias foldback in `analyze_wav.py` spectrum mode (V2).
