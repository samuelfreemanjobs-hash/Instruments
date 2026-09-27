/** WO-SAAS-017: seeded procedural DSP primitives (factory v2). */

export const SAMPLE_RATE = 44100;

export function mulberry32(seed: number): () => number {
  let t = seed >>> 0;
  return () => {
    t += 0x6d2b79f5;
    let r = Math.imul(t ^ (t >>> 15), 1 | t);
    r ^= r + Math.imul(r ^ (r >>> 7), 61 | r);
    return ((r ^ (r >>> 14)) >>> 0) / 4294967296;
  };
}

export function onePoleLowpass(prev: number, input: number, cutoffHz: number): number {
  const dt = 1 / SAMPLE_RATE;
  const rc = 1 / (2 * Math.PI * cutoffHz);
  const alpha = dt / (rc + dt);
  return prev + alpha * (input - prev);
}

export function onePoleHighpass(prevIn: number, prevOut: number, input: number, cutoffHz: number): number {
  const dt = 1 / SAMPLE_RATE;
  const rc = 1 / (2 * Math.PI * cutoffHz);
  const alpha = rc / (rc + dt);
  const out = alpha * (prevOut + input - prevIn);
  return out;
}

export function softClip(x: number, drive: number): number {
  return Math.tanh(x * drive) / Math.tanh(drive);
}

export function bitCrushSample(x: number, bits: number): number {
  const levels = Math.max(2, Math.pow(2, bits));
  return Math.round(x * levels) / levels;
}

/** White-ish noise from seeded PRNG. */
export function noiseSample(rng: () => number): number {
  return rng() * 2 - 1;
}

export function sine(freq: number, t: number, phase = 0): number {
  return Math.sin(2 * Math.PI * freq * t + phase);
}

export function expEnv(t: number, decay: number): number {
  return Math.exp(-t / decay);
}

export function applyBitcrushBlock(
  samples: Float32Array,
  bits: number,
  mix: number,
): Float32Array {
  const out = new Float32Array(samples.length);
  for (let i = 0; i < samples.length; i++) {
    const crushed = bitCrushSample(samples[i], bits);
    out[i] = samples[i] * (1 - mix) + crushed * mix;
  }
  return out;
}
