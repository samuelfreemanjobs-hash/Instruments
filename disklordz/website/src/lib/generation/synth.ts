import type { DrumParams } from "@/lib/generation/prompt-params";
import {
  SAMPLE_RATE,
  expEnv,
  mulberry32,
  noiseSample,
  onePoleHighpass,
  onePoleLowpass,
  sine,
  softClip,
} from "@/lib/generation/dsp-core";

function kick808(pitch: number, decay: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.55);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, decay);
    const pitchEnv = pitch * (1 + 3.5 * expEnv(t, 0.04));
    const body = sine(pitchEnv, t) * env;
    const click = (i < 80 ? noiseSample(rng) * expEnv(t, 0.003) * 0.35 : 0);
    out[i] = softClip(body + click, 1.2);
  }
  return out;
}

function snare(body: number, snap: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.32);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let lp = 0;
  let hp = 0;
  let hpIn = 0;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, 0.11);
    const tone = sine(185, t) * body * expEnv(t, 0.06);
    const raw = noiseSample(rng);
    lp = onePoleLowpass(lp, raw, 2800);
    hpIn = lp;
    hp = onePoleHighpass(hpIn, hp, hp, 600);
    const nse = hp * snap * expEnv(t, 0.035);
    out[i] = (tone + nse) * env;
  }
  return out;
}

function hat(decay: number, bright: number, seed: number, open: boolean): Float32Array {
  const dur = open ? 0.22 : 0.09;
  const n = Math.floor(SAMPLE_RATE * dur);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  let hp = 0;
  let hpIn = 0;
  const cutoff = 5000 + bright * 6000;
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, decay * (open ? 2.2 : 1));
    hpIn = noiseSample(rng);
    hp = onePoleHighpass(hpIn, hp, hp, cutoff);
    const ring = sine(8000 + bright * 2000, t) * 0.04 * env;
    out[i] = (hp * bright + ring) * env;
  }
  return out;
}

function rim(metal: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.07);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, 0.018);
    out[i] = (sine(420, t) + sine(840, t) * 0.55 + sine(1260, t, seed * 0.01) * 0.2) * metal * env;
  }
  return out;
}

function clap(wide: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.2);
  const out = new Float32Array(n);
  const rng = mulberry32(seed);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = expEnv(t, 0.08);
    let burst = 0;
    for (const off of [0, 0.008, 0.016, 0.024]) {
      if (t >= off) burst += noiseSample(rng) * expEnv(t - off, 0.015);
    }
    out[i] = burst * wide * env;
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
