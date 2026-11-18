/**
 * Soft-clip cassette saturation curves (Tascam-ish overload).
 * amount ≈ drive strength (typical 10–60).
 */

/**
 * @param {number} [amount=35]
 * @param {number} [nSamples=44100]
 * @returns {Float32Array}
 */
export function createCassetteCurve(amount = 35, nSamples = 44100) {
  const k = typeof amount === 'number' && Number.isFinite(amount) ? amount : 35;
  const n = Math.max(256, Math.floor(nSamples));
  const curve = new Float32Array(n);
  const deg = Math.PI / 180;
  for (let i = 0; i < n; i += 1) {
    const x = (i * 2) / n - 1;
    // Asymmetric soft-clip (Gemini Memphis FX handoff)
    curve[i] = ((3 + k) * x * 20 * deg) / (Math.PI + k * Math.abs(x));
  }
  return curve;
}

/** Map UI 0..1 drive to curve k parameter. */
export function driveToCurveAmount(drive01) {
  const d = Math.max(0, Math.min(1, drive01));
  return 8 + d * 52; // 8..60
}
