/** Reject or flag unusable factory output (SAS-style gates, lightweight). */

export type QualityReport = {
  pass: boolean;
  peak: number;
  rms: number;
  silentRatio: number;
  clippedRatio: number;
  reasons: string[];
};

const CLIP_THRESHOLD = 0.98;
const MIN_PEAK = 0.02;
const MAX_SILENT_RATIO = 0.85;
const MAX_CLIPPED_RATIO = 0.02;

export function analyzeQuality(samples: Float32Array): QualityReport {
  const reasons: string[] = [];
  let peak = 0;
  let sumSq = 0;
  let silent = 0;
  let clipped = 0;
  const silenceThresh = 0.0005;

  for (let i = 0; i < samples.length; i++) {
    const a = Math.abs(samples[i]);
    peak = Math.max(peak, a);
    sumSq += samples[i] * samples[i];
    if (a < silenceThresh) silent++;
    if (a >= CLIP_THRESHOLD) clipped++;
  }

  const silentRatio = samples.length ? silent / samples.length : 1;
  const clippedRatio = samples.length ? clipped / samples.length : 0;
  const rms = samples.length ? Math.sqrt(sumSq / samples.length) : 0;

  if (peak < MIN_PEAK) reasons.push("too_quiet");
  if (silentRatio > MAX_SILENT_RATIO) reasons.push("mostly_silent");
  if (clippedRatio > MAX_CLIPPED_RATIO) reasons.push("clipped");

  return {
    pass: reasons.length === 0,
    peak,
    rms,
    silentRatio,
    clippedRatio,
    reasons,
  };
}

/** If gate fails, apply emergency gain (should be rare after master). */
export function ensureAudible(samples: Float32Array): Float32Array {
  const report = analyzeQuality(samples);
  if (report.pass) return samples;
  const out = new Float32Array(samples);
  const gain = report.peak > 0 ? 0.5 / report.peak : 10;
  for (let i = 0; i < out.length; i++) out[i] *= gain;
  return out;
}
