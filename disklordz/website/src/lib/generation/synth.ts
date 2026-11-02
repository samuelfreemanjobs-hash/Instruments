import type { DrumParams } from "@/lib/generation/prompt-params";

const SAMPLE_RATE = 44100;

function sine(freq: number, t: number): number {
  return Math.sin(2 * Math.PI * freq * t);
}

function noise(t: number, seed: number): number {
  return Math.sin(t * 9973 + seed * 0.17) * Math.sin(t * 12347 + seed);
}

function kick(pitch: number, decay: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.45);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / decay);
    const f = pitch * (1 + 4 * Math.exp(-t * 30));
    out[i] = sine(f, t) * env;
  }
  return out;
}

function snare(body: number, snap: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.25);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / 0.12);
    const tone = sine(180, t) * body;
    const nse = noise(t, seed) * snap * Math.exp(-t / 0.04);
    out[i] = (tone + nse) * env;
  }
  return out;
}

function hat(decay: number, bright: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.08);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / decay);
    out[i] = noise(t, seed) * bright * env + sine(8000, t) * 0.05 * env;
  }
  return out;
}

function rim(metal: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.06);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / 0.02);
    out[i] = (sine(420, t) + sine(840, t) * 0.5) * metal * env;
  }
  return out;
}

function clap(wide: number, seed: number): Float32Array {
  const n = Math.floor(SAMPLE_RATE * 0.18);
  const out = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / 0.09);
    let burst = 0;
    for (const off of [0, 0.012, 0.024]) {
      if (t >= off) burst += noise(t - off, seed) * Math.exp(-(t - off) / 0.02);
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
      return kick(params.kickPitch, params.kickDecay);
    case "snare":
      return snare(params.snareBody, params.snareSnap, params.seed);
    case "hat_closed":
      return hat(params.hatDecay, params.hatBright, params.seed + 1);
    case "hat_open":
      return hat(params.hatDecay * 2.2, params.hatBright * 1.1, params.seed + 2);
    case "rim":
      return rim(params.rimMetal);
    case "clap":
      return clap(params.clapWide, params.seed + 3);
    default:
      return kick(45, 0.28);
  }
}
