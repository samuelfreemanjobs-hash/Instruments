import type { GenerationEngine, GenerationSpec } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import {
  SAMPLE_RATE,
  applyBitcrushBlock,
  mulberry32,
  onePoleHighpass,
  onePoleLowpass,
  softClip,
} from "@/lib/generation/dsp-core";
import { analyzeQuality, ensureAudible } from "@/lib/generation/quality-gate";

export type ProcessedAudio = {
  mono: Float32Array;
  /** Interleaved stereo when width > 0 */
  stereoInterleaved: Float32Array | null;
};

function targetRmsNormalize(samples: Float32Array, targetRms = 0.12): Float32Array {
  let sumSq = 0;
  for (const s of samples) sumSq += s * s;
  const rms = Math.sqrt(sumSq / samples.length) || 0.0001;
  const gain = targetRms / rms;
  const out = new Float32Array(samples.length);
  for (let i = 0; i < samples.length; i++) {
    out[i] = softClip(samples[i] * gain, 1.35);
  }
  return out;
}

function applyTapeWobble(samples: Float32Array, amount: number, seed: number): Float32Array {
  if (amount <= 0) return samples;
  const out = new Float32Array(samples.length);
  const rng = mulberry32(seed);
  let lp = 0;
  for (let i = 0; i < samples.length; i++) {
    const mod = 1 + (rng() - 0.5) * amount * 0.04;
    const srcIdx = Math.min(samples.length - 1, Math.floor(i * mod));
    lp = onePoleLowpass(lp, samples[srcIdx], 12000);
    out[i] = lp;
  }
  return out;
}

function widenToStereo(mono: Float32Array, width: number, seed: number): Float32Array {
  const interleaved = new Float32Array(mono.length * 2);
  const delaySamples = Math.floor(SAMPLE_RATE * 0.0008 * (1 + width));
  const rng = mulberry32(seed + 99);
  for (let i = 0; i < mono.length; i++) {
    const l = mono[i];
    const rSrc = i >= delaySamples ? mono[i - delaySamples] : mono[i];
    const phase = (rng() - 0.5) * width * 0.08;
    const r = rSrc * (1 - width * 0.15) + phase;
    interleaved[i * 2] = l;
    interleaved[i * 2 + 1] = r;
  }
  return interleaved;
}

/** Master bus: engine chain + normalize + optional stereo (WO-SAAS-017). */
export function masterSample(
  raw: Float32Array,
  params: DrumParams,
  spec: GenerationSpec,
  engine: GenerationEngine,
  grit: number,
): ProcessedAudio {
  let s = raw;

  if (engine === "creative") {
    s = applyBitcrushBlock(s, 10, 0.15 + grit * 0.25);
    s = applyTapeWobble(s, 0.4 + spec.wildness * 0.5, params.seed);
  } else {
    s = applyBitcrushBlock(s, 14, grit * 0.12);
  }

  if (grit > 0.35) {
    s = applyBitcrushBlock(s, 8, grit * 0.4);
  }

  let prevIn = 0;
  let prevOut = 0;
  const filtered = new Float32Array(s.length);
  const hpCut = engine === "studio" ? 28 : 40;
  for (let i = 0; i < s.length; i++) {
    const input = s[i];
    prevOut = onePoleHighpass(prevIn, prevOut, input, hpCut);
    filtered[i] = prevOut;
    prevIn = input;
  }
  s = filtered;

  s = targetRmsNormalize(s, engine === "studio" ? 0.11 : 0.13);
  s = ensureAudible(s);

  const report = analyzeQuality(s);
  if (!report.pass && report.reasons.includes("clipped")) {
    s = targetRmsNormalize(s, 0.08);
  }

  const width = spec.stereo;
  const stereoInterleaved = width > 0.08 ? widenToStereo(s, width, params.seed) : null;

  return { mono: s, stereoInterleaved };
}
