import type { DrumParams } from "@/lib/generation/prompt-params";
import {
  SAMPLE_RATE,
  expEnv,
  glidePitchHz,
  mulberry32,
  noiseSample,
  onePoleHighpass,
  onePoleLowpass,
  sine,
  softClip,
} from "@/lib/generation/dsp-core";

function kick808(pitch: number, decay: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.62);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  const startHz = pitch * 3.2;
  const endHz = pitch;
  let clickLp = 0;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, decay);
    const pitchHz = glidePitchHz(t, startHz, endHz, 0.042);
    const body = sine(pitchHz, t) * env;
    const sub =
      sine(pitchHz * 0.5, t) * expEnv(t, decay * 1.4) * (pitch < 42 ? 0.62 : 0.48);
    const clickRaw = i < 140 ? noiseSample(rng) : 0;
    clickLp = onePoleLowpass(clickLp, clickRaw, 2800);
    const click = clickLp * expEnv(t, 0.0022) * 0.42;
    out[i] = softClip((body + sub + click) * 1.02, 1.28);
  }
  return out;
}

function snare(body: number, snap: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.38);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let lp = 0;
  let hpIn = 0;
  let hpOut = 0;
  const toneHz = 185 + body * 55;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const ampEnv = expEnv(t, 0.13);
    const toneEnv = expEnv(t, 0.05);
    const tone =
      sine(toneHz, t) * body * toneEnv +
      sine(toneHz * 2.05, t) * body * 0.22 * toneEnv;
    const raw = noiseSample(rng);
    lp = onePoleLowpass(lp, raw, 3400);
    hpOut = onePoleHighpass(hpIn, hpOut, lp, 520);
    hpIn = lp;
    const snapEnv = expEnv(t, 0.026);
    const nse = hpOut * snap * snapEnv * 1.15;
    out[i] = softClip((tone + nse) * ampEnv, 1.18);
  }
  return out;
}

function metallicPartial(t: number, freq: number, phase: number): number {
  return softClip(sine(freq, t, phase) * 2.4, 1.35);
}

function hat(decay: number, bright: number, seed: number, open: boolean): Float32Array {
  const dur = open ? 0.28 : 0.1;
  const n = Math.floor(SAMPLE_RATE * dur);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let hpIn = 0;
  let hpOut = 0;
  const scale = 0.92 + bright * 0.14;
  const freqs = [3177, 4821, 6532, 8911, 11200].map((f) => f * scale);
  const cutoff = 4200 + bright * 7500;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, decay * (open ? 2.4 : 1));
    const raw = noiseSample(rng);
    hpOut = onePoleHighpass(hpIn, hpOut, raw, cutoff);
    hpIn = raw;
    let metal = 0;
    for (let p = 0; p < freqs.length; p++) {
      metal += metallicPartial(t, freqs[p], seed * 0.001 + p) * 0.09;
    }
    const mix = open ? 0.55 : 0.72;
    const s = hpOut * bright * mix + metal * bright * (open ? 0.35 : 0.22);
    out[i] = s * env;
  }
  return out;
}

function rim(metal: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.085);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let lp = 0;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, 0.022);
    lp = onePoleLowpass(lp, noiseSample(rng), 2400);
    const tone =
      sine(420, t) * 0.55 +
      sine(840, t) * 0.45 +
      sine(1260, t, seed * 0.01) * 0.18;
    out[i] = softClip((tone + lp * 0.35) * metal * env, 1.12);
  }
  return out;
}

function clap(wide: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.22);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let lp = 0;
  const offsets = [0, 0.007, 0.014, 0.021, 0.029];
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, 0.085);
    let burst = 0;
    for (const off of offsets) {
      if (t >= off) {
        burst += noiseSample(rng) * expEnv(t - off, 0.012);
      }
    }
    lp = onePoleLowpass(lp, burst, 1800 + wide * 2200);
    out[i] = lp * wide * env * 1.05;
  }
  return out;
}

export type SampleName =
  | "kick"
  | "snare"
  | "hat_closed"
  | "hat_open"
  | "rim"
  | "clap";

export const SAMPLE_NAMES: SampleName[] = [
  "kick",
  "snare",
  "hat_closed",
  "hat_open",
  "rim",
  "clap",
];

export function renderSample(name: SampleName, params: DrumParams): Float32Array {
  switch (name) {
    case "kick":
      return kick808(params.kickPitch, params.kickDecay, params.seed);
    case "snare":
      return snare(params.snareBody, params.snareSnap, params.seed);
    case "hat_closed":
      return hat(params.hatDecay, params.hatBright, params.seed + 1, false);
    case "hat_open":
      return hat(params.hatDecay * 1.8, params.hatBright * 1.05, params.seed + 2, true);
    case "rim":
      return rim(params.rimMetal, params.seed + 3);
    case "clap":
      return clap(params.clapWide, params.seed + 4);
    default:
      return kick808(45, 0.28, params.seed);
  }
}
