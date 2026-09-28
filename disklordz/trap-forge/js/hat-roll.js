import { SR } from "./dsp-core.js";

/** Spacing between roll triggers at `bpm` (FL-style note divisions). */
export function rollSpacingSec(bpm, divisionKey) {
  const beat = 60 / bpm;
  const map = {
    8: beat / 2,
    16: beat / 4,
    32: beat / 8,
    64: beat / 16,
    "12T": beat / 3,
    "24T": beat / 6,
  };
  return map[divisionKey] ?? beat / 4;
}

export function velocityForRamp(i, count, ramp) {
  if (count <= 1) return 1;
  const t = i / (count - 1);
  if (ramp === "crescendo") return 0.35 + t * 0.65;
  if (ramp === "decrescendo") return 1 - t * 0.55;
  if (ramp === "jitter") return 0.55 + Math.random() * 0.45;
  return 0.85;
}

/**
 * Phase 5 hat ratchet: micro-timing jitter, per-hit envelope retrigger,
 * velocity ramp, and pitch drift on dispersion.
 */
export function renderHatRoll(synthHatClosed, baseParams, opts) {
  const {
    bpm = 140,
    division = "16",
    count = 8,
    ramp = "crescendo",
    pitchDrift = 0.006,
    timingJitter = 0.018,
    pitchSlide = 0,
  } = opts;

  const spacing = rollSpacingSec(bpm, String(division));
  const hits = [];

  for (let i = 0; i < count; i++) {
    const vel = velocityForRamp(i, count, ramp);
    const jitter = (Math.random() - 0.5) * timingJitter;
    const timeSec = Math.max(0, i * spacing + jitter);
    const drift =
      (Math.random() - 0.5) * pitchDrift +
      (pitchSlide ? (i / Math.max(1, count - 1)) * pitchSlide : 0);
    const p = {
      ...baseParams,
      dispersion: (baseParams.dispersion ?? 0.01) + drift,
      aA: 0.0004,
      aD: (baseParams.aD ?? 0.022) * (0.85 + Math.random() * 0.2),
    };
    const hit = synthHatClosed(p);
    hits.push({ left: hit.left, right: hit.right, offset: Math.floor(timeSec * SR), vel });
  }

  let total = 0;
  hits.forEach((h) => {
    total = Math.max(total, h.offset + h.left.length);
  });
  total += Math.floor(SR * 0.04);

  const left = new Float32Array(total);
  const right = new Float32Array(total);
  hits.forEach((h) => {
    for (let i = 0; i < h.left.length; i++) {
      const idx = h.offset + i;
      if (idx >= total) break;
      left[idx] += h.left[i] * h.vel;
      right[idx] += h.right[i] * h.vel;
    }
  });

  const mono = new Float32Array(total);
  for (let i = 0; i < total; i++) mono[i] = (left[i] + right[i]) * 0.5;

  return { left, right, mono, durationSec: total / SR };
}
