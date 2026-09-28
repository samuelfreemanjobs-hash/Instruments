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

/** 2× linear upsample → nonlinear → average down (anti-aliased saturation). */
export function applySaturationBuffer(mono, drive = 1.2) {
  const out = new Float32Array(mono.length);
  for (let i = 0; i < mono.length; i++) {
    const a = mono[i];
    const b = i + 1 < mono.length ? mono[i + 1] : a;
    const s0 = softClipFl(triode(a, drive));
    const s1 = softClipFl(triode((a + b) * 0.5, drive));
    out[i] = (s0 + s1) * 0.5;
  }
  return out;
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

export function normalizePeak(mono, db = -0.3) {
  let peak = 0;
  for (let i = 0; i < mono.length; i++) peak = Math.max(peak, Math.abs(mono[i]));
  if (peak < 1e-9) return mono;
  const g = Math.pow(10, db / 20) / peak;
  const out = new Float32Array(mono.length);
  for (let i = 0; i < mono.length; i++) out[i] = mono[i] * g;
  return out;
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

export function downloadBuffer(buf, name) {
  const blob = new Blob([buf], { type: "audio/wav" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  URL.revokeObjectURL(url);
}
