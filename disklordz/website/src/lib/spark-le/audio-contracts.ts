/**
 * SparkLE Phase 2 audio contract metrics (SOP-014).
 * Pure PCM analysis — no external DSP deps.
 */

export type ContractId =
  | "808-SUB"
  | "KNOCK"
  | "SNARE-STACK"
  | "HAT-CLARITY"
  | "LOOP-GRID"
  | "NO-CLIP"
  | "SPARK-MANIFEST";

export type ContractViolation = {
  contractId: ContractId;
  presetId: string;
  pad?: string;
  message: string;
  metric: number;
  bound: number;
};

const SAMPLE_RATE = 44100;

function rms(samples: Float32Array): number {
  if (samples.length === 0) return 0;
  let sum = 0;
  for (const s of samples) sum += s * s;
  return Math.sqrt(sum / samples.length);
}

function truePeak(samples: Float32Array): number {
  let peak = 0;
  for (const s of samples) peak = Math.max(peak, Math.abs(s));
  return peak;
}

/** One-pole low-pass RMS (approx band energy). */
function bandRms(samples: Float32Array, cutoffHz: number): number {
  const rc = 1 / (2 * Math.PI * cutoffHz);
  const alpha = 1 / (SAMPLE_RATE * rc + 1);
  let y = 0;
  let sum = 0;
  for (const s of samples) {
    y = y + alpha * (s - y);
    sum += y * y;
  }
  return Math.sqrt(sum / samples.length);
}

function highBandRms(samples: Float32Array): number {
  const low = bandRms(samples, 6000);
  const total = rms(samples);
  return Math.max(0, total - low);
}

/** Transient peak in first `ms` vs overall RMS (KNOCK). */
function knockRatio(samples: Float32Array, ms = 40): number {
  const n = Math.min(samples.length, Math.floor((SAMPLE_RATE * ms) / 1000));
  if (n === 0) return 0;
  let peak = 0;
  for (let i = 0; i < n; i++) peak = Math.max(peak, Math.abs(samples[i]));
  const body = rms(samples);
  return body > 0 ? peak / body : 0;
}

/** Rough onset count for LOOP-GRID (4/4 loop). */
export function countOnsets(samples: Float32Array, threshold = 0.04): number {
  let count = 0;
  let prev = 0;
  for (let i = 1; i < samples.length; i++) {
    const e = samples[i] * samples[i];
    if (e > threshold * threshold && prev <= threshold * threshold) count++;
    prev = Math.sqrt(e);
  }
  return count;
}

export type LaneContractThresholds = {
  subLowRatioMin: number;
  knockRatioMin: number;
  snareBodyMin: number;
  hatClarityMin: number;
  loopOnsetsMin: number;
  loopOnsetsMax: number;
  truePeakMax: number;
};

export const DEFAULT_THRESHOLDS: LaneContractThresholds = {
  subLowRatioMin: 0.35,
  knockRatioMin: 1.5,
  snareBodyMin: 0.02,
  hatClarityMin: 0.08,
  loopOnsetsMin: 4,
  loopOnsetsMax: 128,
  truePeakMax: 0.92,
};

/** Lane-specific contract profiles (SOP-014). */
export const LANE_CONTRACT_PROFILES: Record<string, Partial<LaneContractThresholds>> = {
  "lane-jeezy": { subLowRatioMin: 0.32, hatClarityMin: 0.07 },
  "lane-dj-screw": {
    subLowRatioMin: 0.38,
    knockRatioMin: 1.2,
    hatClarityMin: 0.03,
    loopOnsetsMin: 0,
    loopOnsetsMax: 200,
  },
  "lane-smooth-jazz-rap": { subLowRatioMin: 0.28, hatClarityMin: 0.04, loopOnsetsMax: 80 },
  "midnight-circuit": { subLowRatioMin: 0.32, hatClarityMin: 0.08 },
  "lane-trap-or-die": { subLowRatioMin: 0.32, hatClarityMin: 0.07 },
};

export function thresholdsForPreset(presetId: string): LaneContractThresholds {
  return { ...DEFAULT_THRESHOLDS, ...LANE_CONTRACT_PROFILES[presetId] };
}

export function evaluateSampleContracts(
  presetId: string,
  pad: string,
  samples: Float32Array,
): ContractViolation[] {
  const t = thresholdsForPreset(presetId);
  const out: ContractViolation[] = [];
  const peak = truePeak(samples);
  if (peak > t.truePeakMax) {
    out.push({
      contractId: "NO-CLIP",
      presetId,
      pad,
      message: "true peak above ceiling",
      metric: peak,
      bound: t.truePeakMax,
    });
  }

  if (pad === "kick") {
    const low = bandRms(samples, 120);
    const total = rms(samples);
    const ratio = total > 0 ? low / total : 0;
    if (ratio < t.subLowRatioMin) {
      out.push({
        contractId: "808-SUB",
        presetId,
        pad,
        message: "low-band energy ratio too low",
        metric: ratio,
        bound: t.subLowRatioMin,
      });
    }
    const kr = knockRatio(samples);
    if (kr < t.knockRatioMin) {
      out.push({
        contractId: "KNOCK",
        presetId,
        pad,
        message: "knock transient too weak",
        metric: kr,
        bound: t.knockRatioMin,
      });
    }
  }

  if (pad === "snare") {
    const body = rms(samples);
    if (body < t.snareBodyMin) {
      out.push({
        contractId: "SNARE-STACK",
        presetId,
        pad,
        message: "snare body RMS too low",
        metric: body,
        bound: t.snareBodyMin,
      });
    }
  }

  if (pad === "hat_closed") {
    const hi = highBandRms(samples);
    const total = rms(samples);
    const clarity = total > 0 ? hi / total : 0;
    if (clarity < t.hatClarityMin) {
      out.push({
        contractId: "HAT-CLARITY",
        presetId,
        pad,
        message: "hat high-band clarity too low",
        metric: clarity,
        bound: t.hatClarityMin,
      });
    }
  }

  return out;
}

export function evaluateLoopGridContract(
  presetId: string,
  loop: Float32Array,
): ContractViolation[] {
  const t = thresholdsForPreset(presetId);
  const onsets = countOnsets(loop, 0.08);
  const out: ContractViolation[] = [];
  if (onsets < t.loopOnsetsMin || onsets > t.loopOnsetsMax) {
    out.push({
      contractId: "LOOP-GRID",
      presetId,
      pad: "loop",
      message: "onset count out of range for 4/4 loop",
      metric: onsets,
      bound: t.loopOnsetsMin,
    });
  }
  return out;
}

export type SparkManifestShape = {
  schema_version?: string;
  generator_version?: string;
  lane?: string;
  family?: string;
  bpm?: number;
  spark_le?: { pad_map_version?: string; pads?: Record<string, unknown> };
};

export function evaluateManifestContract(path: string, manifest: SparkManifestShape): ContractViolation[] {
  const out: ContractViolation[] = [];
  const presetId = path;

  if (!manifest.schema_version) {
    out.push({
      contractId: "SPARK-MANIFEST",
      presetId,
      message: "missing schema_version",
      metric: 0,
      bound: 1,
    });
  }
  if (!manifest.lane || !manifest.family) {
    out.push({
      contractId: "SPARK-MANIFEST",
      presetId,
      message: "missing lane or family",
      metric: 0,
      bound: 1,
    });
  }
  const pads = manifest.spark_le?.pads;
  const padCount = pads ? Object.keys(pads).length : 0;
  if (padCount !== 8) {
    out.push({
      contractId: "SPARK-MANIFEST",
      presetId,
      message: "expected 8 spark_le pads",
      metric: padCount,
      bound: 8,
    });
  }
  return out;
}
