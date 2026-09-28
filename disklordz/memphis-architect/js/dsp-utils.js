/**
 * Shared DSP utilities — MEMPHIS-660 / DR-660 phonk architect
 */

export const SAMPLE_RATE = 44100;

export function noteToHz(noteName, octave = 1) {
  const names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  const idx = names.indexOf(noteName);
  if (idx < 0) return 43.65;
  const midi = (octave + 1) * 12 + idx;
  return 440 * Math.pow(2, (midi - 69) / 12);
}

export function linToDb(lin) {
  return 20 * Math.log10(Math.max(lin, 1e-12));
}

export function applyPitchEnv(t, rootHz, modSt, decaySec) {
  const pitchEnv = Math.exp(-t / Math.max(decaySec, 0.001));
  return rootHz * Math.pow(2, (modSt * pitchEnv) / 12);
}

export function integrateFreqToPhase(freq, sr) {
  const phase = new Float32Array(freq.length);
  let p = 0;
  for (let i = 0; i < freq.length; i++) {
    p += (2 * Math.PI * freq[i]) / sr;
    phase[i] = p;
  }
  return phase;
}

/** ADSR envelope 0..1 */
export function adsrEnvelope(t, { attack, decay, sustain, release }, totalDuration) {
  const a = Math.max(attack, 0.0001);
  const d = Math.max(decay, 0.0001);
  const r = Math.max(release, 0.0001);
  const s = Math.min(Math.max(sustain, 0), 1);
  const out = new Float32Array(t.length);
  for (let i = 0; i < t.length; i++) {
    const time = t[i];
    if (time < a) {
      out[i] = time / a;
    } else if (time < a + d) {
      out[i] = 1 - (1 - s) * ((time - a) / d);
    } else if (time < totalDuration - r) {
      out[i] = s;
    } else {
      const relT = time - (totalDuration - r);
      out[i] = s * (1 - relT / r);
    }
  }
  return out;
}

export function sp1200Decimate(signal, sr, lofiSr, bitDepth) {
  const step = sr / lofiSr;
  const out = new Float32Array(signal.length);
  const levels = Math.pow(2, bitDepth - 1);
  for (let i = 0; i < signal.length; i++) {
    const idx = Math.min(signal.length - 1, Math.floor(i / step) * Math.floor(step));
    const q = Math.round(signal[idx] * levels) / levels;
    out[i] = q;
  }
  return out;
}

export function onePoleLowpass(signal, sr, cutoffHz) {
  const alpha = Math.exp((-2 * Math.PI * cutoffHz) / sr);
  const out = new Float32Array(signal.length);
  out[0] = signal[0];
  for (let i = 1; i < signal.length; i++) {
    out[i] = (1 - alpha) * signal[i] + alpha * out[i - 1];
  }
  return out;
}

export function onePoleHighpass(signal, sr, cutoffHz) {
  const alpha = Math.exp((-2 * Math.PI * cutoffHz) / sr);
  const out = new Float32Array(signal.length);
  let prevIn = 0;
  let prevOut = 0;
  for (let i = 0; i < signal.length; i++) {
    const x = signal[i];
    out[i] = alpha * (prevOut + x - prevIn);
    prevIn = x;
    prevOut = out[i];
  }
  return out;
}

export function biquadBandpass(signal, sr, centerHz, q = 1.2) {
  const w0 = (2 * Math.PI * centerHz) / sr;
  const alpha = Math.sin(w0) / (2 * q);
  const b0 = alpha;
  const b1 = 0;
  const b2 = -alpha;
  const a0 = 1 + alpha;
  const a1 = -2 * Math.cos(w0);
  const a2 = 1 - alpha;
  const out = new Float32Array(signal.length);
  let x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  for (let i = 0; i < signal.length; i++) {
    const x0 = signal[i];
    const y0 = (b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2) / a0;
    out[i] = y0;
    x2 = x1; x1 = x0; y2 = y1; y1 = y0;
  }
  return out;
}

export function softClip(signal, driveDb) {
  const drive = Math.pow(10, driveDb / 20);
  const out = new Float32Array(signal.length);
  for (let i = 0; i < signal.length; i++) {
    out[i] = Math.tanh(signal[i] * drive);
  }
  return out;
}

export function normalizePeak(signal, targetDb = -0.3) {
  let peak = 0;
  for (let i = 0; i < signal.length; i++) {
    peak = Math.max(peak, Math.abs(signal[i]));
  }
  if (peak < 1e-9) return signal;
  const gain = Math.pow(10, targetDb / 20) / peak;
  const out = new Float32Array(signal.length);
  for (let i = 0; i < signal.length; i++) out[i] = signal[i] * gain;
  return out;
}

export const FILTER_PRESETS = {
  cassette: { id: "cassette", label: "Cassette tape (12 kHz LP)", apply: (s, sr) => onePoleLowpass(s, sr, 12000) },
  fat_lp: { id: "fat_lp", label: "Fat lowpass (90 Hz)", apply: (s, sr) => onePoleLowpass(s, sr, 9000) },
  telephone: { id: "telephone", label: "Telephone bandpass (2.4 kHz)", apply: (s, sr) => biquadBandpass(s, sr, 2400, 1.5) },
  clicky_hp: { id: "clicky_hp", label: "Clicky highpass (180 Hz)", apply: (s, sr) => onePoleHighpass(s, sr, 180) },
  dr660: { id: "dr660", label: "DR-660 punch (1.8 kHz BP)", apply: (s, sr) => biquadBandpass(s, sr, 1800, 0.9) },
  none: { id: "none", label: "Bypass", apply: (s) => s },
};

export function applyFilterPreset(signal, sr, presetId, cutoffMod = 1) {
  const preset = FILTER_PRESETS[presetId] ?? FILTER_PRESETS.cassette;
  if (presetId === "none") return preset.apply(signal, sr);
  return preset.apply(signal, sr, cutoffMod);
}

/** Filter envelope scales effective cutoff multiplier 0.2..1.2 */
export function applyFilterWithEnv(signal, sr, presetId, filterEnv) {
  const out = new Float32Array(signal.length);
  const chunk = 64;
  for (let start = 0; start < signal.length; start += chunk) {
    const end = Math.min(signal.length, start + chunk);
    const slice = signal.subarray(start, end);
    const envMid = filterEnv[Math.min(filterEnv.length - 1, start + chunk / 2)] ?? 1;
    const mod = 0.25 + envMid * 0.95;
    const filtered = applyFilterPreset(slice, sr, presetId, mod);
    out.set(filtered, start);
  }
  return out;
}

export function encodeWav24Mono(samples, sr = SAMPLE_RATE) {
  const numSamples = samples.length;
  const dataSize = numSamples * 3;
  const buffer = new ArrayBuffer(44 + dataSize);
  const view = new DataView(buffer);
  const writeStr = (off, str) => {
    for (let i = 0; i < str.length; i++) view.setUint8(off + i, str.charCodeAt(i));
  };
  writeStr(0, "RIFF");
  view.setUint32(4, 36 + dataSize, true);
  writeStr(8, "WAVE");
  writeStr(12, "fmt ");
  view.setUint32(16, 16, true);
  view.setUint16(20, 1, true);
  view.setUint16(22, 1, true);
  view.setUint32(24, sr, true);
  view.setUint32(28, sr * 3, true);
  view.setUint16(32, 3, true);
  view.setUint16(34, 24, true);
  writeStr(36, "data");
  view.setUint32(40, dataSize, true);
  let offset = 44;
  for (let i = 0; i < numSamples; i++) {
    let v = Math.max(-1, Math.min(1, samples[i]));
    let intVal = Math.round(v * 8388607);
    view.setUint8(offset, intVal & 0xff);
    view.setUint8(offset + 1, (intVal >> 8) & 0xff);
    view.setUint8(offset + 2, (intVal >> 16) & 0xff);
    offset += 3;
  }
  return buffer;
}

export function downloadBlob(buffer, filename) {
  const blob = new Blob([buffer], { type: "audio/wav" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  a.click();
  URL.revokeObjectURL(url);
}

export function whiteNoise(n) {
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) out[i] = Math.random() * 2 - 1;
  return out;
}
