export const SR = 44100;

export function adsr(t, a, d, s, r, gate) {
  if (t < a) return a > 0 ? t / a : 1;
  if (t < a + d) return 1 - (1 - s) * ((t - a) / d);
  if (t < gate) return s;
  if (t < gate + r) return r > 0 ? s * (1 - (t - gate) / r) : 0;
  return 0;
}

export function noteHz(note, octave = 1) {
  const names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  const i = names.indexOf(note);
  const midi = (octave + 1) * 12 + (i >= 0 ? i : 5);
  return 440 * Math.pow(2, (midi - 69) / 12);
}

export function mulberry32(seed) {
  let a = seed >>> 0;
  return () => {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = Math.imul(a ^ (a >>> 15), 1 | a);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

const HB7 = [-0.037935, 0, 0.28729, 0.5, 0.28729, 0, -0.037935];

function upsampleHold2x(mono) {
  const up = new Float32Array(mono.length * 2);
  for (let i = 0; i < mono.length; i++) {
    up[i * 2] = mono[i];
    up[i * 2 + 1] = mono[i];
  }
  return up;
}

function halfBandDecimate2x(up2x) {
  const n = Math.floor(up2x.length / 2);
  const out = new Float32Array(n);
  const mid = (HB7.length - 1) / 2;
  for (let i = 0; i < n; i++) {
    const center = i * 2;
    let s = 0;
    for (let k = 0; k < HB7.length; k++) {
      const idx = center - mid + k;
      if (idx >= 0 && idx < up2x.length) s += up2x[idx] * HB7[k];
    }
    out[i] = s;
  }
  return out;
}

export function applyNonlinearOversample2x(mono, sampleFn) {
  const up = upsampleHold2x(mono);
  for (let i = 0; i < up.length; i++) up[i] = sampleFn(up[i]);
  return halfBandDecimate2x(up);
}

/** 2× OS: hold upsample → nonlinear → half-band decimate. */
export function applySaturationBuffer(mono, drive = 1.2) {
  return applyNonlinearOversample2x(mono, (x) => softClipFl(triode(x, drive)));
}

export function triode(x, drive = 1.2) {
  const d = x * drive;
  return d >= 0 ? Math.tanh(d * 1.1) : Math.tanh(d * 0.85) * 0.95;
}

export function softClipFl(x, threshold = 0.9) {
  const t = threshold;
  if (x > t) return t + (x - t) / (1 + (x - t) * 8);
  if (x < -t) return -t + (x + t) / (1 + (-x - t) * 8);
  return x;
}

export function applyDrive(sample, drive = 1, distType = "triode") {
  const d = 1 + (drive ?? 0);
  const x = sample * d;
  switch (distType) {
    case "tape":
      return Math.tanh(x * 0.85) * 0.92 + x * 0.04;
    case "tube":
      return softClipFl(triode(sample, d), 0.92);
    case "foldback":
      return Math.sin(x * Math.PI * 0.55);
    case "spinz":
      return softClipFl(x, 0.82);
    case "fl_clip":
      return softClipFl(x, 0.9);
    default:
      return softClipFl(triode(sample, d), 0.92);
  }
}

/** Chamberlin SVF low-pass (12 dB). */
export function svfLowPass(input, state, cutoff, q = 0.707) {
  const g = Math.tan((Math.PI * Math.max(80, Math.min(cutoff, 16000))) / SR);
  const k = 1 / Math.max(0.1, q);
  const a1 = 1 / (1 + g * (g + k));
  const a2 = g * a1;
  const a3 = g * a2;
  const v3 = input - state.ic2eq;
  const v1 = a1 * state.ic1eq + a2 * v3;
  const v2 = state.ic2eq + a2 * state.ic1eq + a3 * v3;
  state.ic1eq = 2 * v1 - state.ic1eq;
  state.ic2eq = 2 * v2 - state.ic2eq;
  return v2;
}

export function decimate(signal, lofiSr, bits) {
  const step = SR / lofiSr;
  const levels = Math.pow(2, bits - 1);
  const out = new Float32Array(signal.length);
  for (let i = 0; i < signal.length; i++) {
    const idx = Math.min(signal.length - 1, Math.floor(Math.floor(i / step) * step));
    out[i] = Math.round(signal[idx] * levels) / levels;
  }
  return out;
}

export function onePoleLP(x, prev, cutoff) {
  const a = Math.exp((-2 * Math.PI * cutoff) / SR);
  return (1 - a) * x + a * prev;
}

export function biquadLP(input, state, cutoff, q = 0.707) {
  const w0 = (2 * Math.PI * cutoff) / SR;
  const alpha = Math.sin(w0) / (2 * q);
  const b0 = (1 - Math.cos(w0)) / 2;
  const b1 = 1 - Math.cos(w0);
  const b2 = (1 - Math.cos(w0)) / 2;
  const a0 = 1 + alpha;
  const a1 = -2 * Math.cos(w0);
  const a2 = 1 - alpha;
  let { x1, x2, y1, y2 } = state;
  const y0 = (b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2) / a0;
  state.x2 = x1; state.x1 = input; state.y2 = y1; state.y1 = y0;
  return y0;
}

/** Band-limited white noise buffer (deterministic seed). */
export function fillBandLimitedNoise(out, cutoff = 9000, seed = 0x7a3f0001) {
  const rng = mulberry32(seed);
  const st = { x1: 0, x2: 0, y1: 0, y2: 0 };
  for (let i = 0; i < out.length; i++) {
    out[i] = biquadLP(rng() * 2 - 1, st, cutoff, 0.65);
  }
}

export function fillPinkNoise(out, seed = 0xbeef0001) {
  const rng = mulberry32(seed);
  let b0 = 0;
  let b1 = 0;
  let b2 = 0;
  for (let i = 0; i < out.length; i++) {
    const white = rng() * 2 - 1;
    b0 = 0.99886 * b0 + white * 0.0555179;
    b1 = 0.99332 * b1 + white * 0.0750759;
    b2 = 0.969 * b2 + white * 0.153852;
    out[i] = (b0 + b1 + b2 + white * 0.5362) * 0.22;
  }
}

export function normalizePeak(mono, db = -0.3) {
  let peak = 0;
  for (let i = 0; i < mono.length; i++) peak = Math.max(peak, Math.abs(mono[i]));
  if (peak < 1e-9) return mono;
  const g = Math.pow(10, db / 20) / peak;
  const out = new Float32Array(mono.length);
  for (let i = 0; i < mono.length; i++) out[i] = mono[i] * g;
  return out;
}

/** Studio transient shaper: ±dB boost/cut in the first `windowSec` (default 15 ms). */
export function attackTransientGain(t, attackDb, windowSec = 0.015) {
  if (!attackDb || t >= windowSec) return 1;
  const env = Math.exp(-t / (windowSec * 0.28));
  return Math.pow(10, (attackDb * env) / 20);
}

export function sustainBodyGain(t, sustainDb, onsetSec = 0.015) {
  if (!sustainDb || t < onsetSec) return 1;
  return Math.pow(10, sustainDb / 20);
}

/** Phase 3/4 parallel trunk bus: FL soft-clip + envelope-aware smash (no digital overs). */
export function applyTrunkMasterBus(left, right, opts = {}) {
  const n = Math.min(left.length, right.length);
  const ceil = Math.pow(10, (opts.ceilingDb ?? -0.3) / 20);
  const drive = 1 + (opts.drive ?? 0.15);
  const trunk = opts.trunkAmount ?? 0.38;
  const L = new Float32Array(n);
  const R = new Float32Array(n);
  let env = 0;
  const att = Math.exp(-1 / (SR * 0.0015));
  const rel = Math.exp(-1 / (SR * 0.06));
  for (let i = 0; i < n; i++) {
    const mid = (left[i] + right[i]) * 0.5;
    const pk = Math.abs(mid);
    env = pk > env ? att * env + (1 - att) * pk : rel * env + (1 - rel) * pk;
    const smash = Math.tanh(mid * drive * (1.2 + env * 3.5));
    const dryL = left[i] * drive;
    const dryR = right[i] * drive;
    const wetL = dryL * (1 - trunk) + smash * trunk * 1.15;
    const wetR = dryR * (1 - trunk) + smash * trunk * 1.12;
    L[i] = softClipFl(wetL, ceil);
    R[i] = softClipFl(wetR, ceil);
  }
  return { left: L, right: R };
}

export function encodeStereoWav24(left, right) {
  const n = Math.min(left.length, right.length);
  const dataSize = n * 6;
  const buf = new ArrayBuffer(44 + dataSize);
  const v = new DataView(buf);
  const w = (o, s) => { for (let i = 0; i < s.length; i++) v.setUint8(o + i, s.charCodeAt(i)); };
  w(0, "RIFF");
  v.setUint32(4, 36 + dataSize, true);
  w(8, "WAVE");
  w(12, "fmt ");
  v.setUint32(16, 16, true);
  v.setUint16(20, 1, true);
  v.setUint16(22, 2, true);
  v.setUint32(24, SR, true);
  v.setUint32(28, SR * 6, true);
  v.setUint16(32, 6, true);
  v.setUint16(34, 24, true);
  w(36, "data");
  v.setUint32(40, dataSize, true);
  let o = 44;
  for (let i = 0; i < n; i++) {
    for (const s of [left[i], right[i]]) {
      const val = Math.round(Math.max(-1, Math.min(1, s)) * 8388607);
      v.setUint8(o++, val & 255);
      v.setUint8(o++, (val >> 8) & 255);
      v.setUint8(o++, (val >> 16) & 255);
    }
  }
  return buf;
}

export function encodeMonoWav24(mono) {
  return encodeStereoWav24(mono, mono);
}

function writeWavHeader(view, channels, sampleRate, bits, dataSize) {
  const w = (o, s) => { for (let i = 0; i < s.length; i++) view.setUint8(o + i, s.charCodeAt(i)); };
  const blockAlign = channels * (bits / 8);
  w(0, "RIFF");
  view.setUint32(4, 36 + dataSize, true);
  w(8, "WAVE");
  w(12, "fmt ");
  view.setUint32(16, 16, true);
  view.setUint16(20, 1, true);
  view.setUint16(22, channels, true);
  view.setUint32(24, sampleRate, true);
  view.setUint32(28, sampleRate * blockAlign, true);
  view.setUint16(32, blockAlign, true);
  view.setUint16(34, bits, true);
  w(36, "data");
  view.setUint32(40, dataSize, true);
}

export function encodeMonoWav(mono, bits = 24) {
  const n = mono.length;
  if (bits === 16) {
    const dataSize = n * 2;
    const buf = new ArrayBuffer(44 + dataSize);
    const v = new DataView(buf);
    writeWavHeader(v, 1, SR, 16, dataSize);
    let o = 44;
    for (let i = 0; i < n; i++) {
      const val = Math.round(Math.max(-1, Math.min(1, mono[i])) * 32767);
      v.setInt16(o, val, true);
      o += 2;
    }
    return buf;
  }
  if (bits === 32) {
    const dataSize = n * 4;
    const buf = new ArrayBuffer(44 + dataSize);
    const v = new DataView(buf);
    writeWavHeader(v, 1, SR, 32, dataSize);
    let o = 44;
    for (let i = 0; i < n; i++) {
      v.setFloat32(o, mono[i], true);
      o += 4;
    }
    return buf;
  }
  return encodeMonoWav24(mono);
}

export function encodeStereoWav(left, right, bits = 24) {
  const n = Math.min(left.length, right.length);
  if (bits === 24) return encodeStereoWav24(left, right);
  if (bits === 16) {
    const dataSize = n * 4;
    const buf = new ArrayBuffer(44 + dataSize);
    const v = new DataView(buf);
    writeWavHeader(v, 2, SR, 16, dataSize);
    let o = 44;
    for (let i = 0; i < n; i++) {
      for (const s of [left[i], right[i]]) {
        v.setInt16(o, Math.round(Math.max(-1, Math.min(1, s)) * 32767), true);
        o += 2;
      }
    }
    return buf;
  }
  if (bits === 32) {
    const dataSize = n * 8;
    const buf = new ArrayBuffer(44 + dataSize);
    const v = new DataView(buf);
    writeWavHeader(v, 2, SR, 32, dataSize);
    let o = 44;
    for (let i = 0; i < n; i++) {
      v.setFloat32(o, left[i], true);
      v.setFloat32(o + 4, right[i], true);
      o += 8;
    }
    return buf;
  }
  return encodeStereoWav24(left, right);
}

export function downloadBuffer(buf, name) {
  const blob = new Blob([buf], { type: "audio/wav" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  URL.revokeObjectURL(url);
}
