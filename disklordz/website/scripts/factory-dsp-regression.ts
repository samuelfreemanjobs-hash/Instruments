/**
 * Offline Factory DSP regression (WO-SAAS-018). No HTTP server required.
 * Run: npm run factory:dsp-regression
 */
import { readFileSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

import { gritFromParams, renderSampleForEngine } from "../src/lib/generation/engine-render";
import { defaultGenerationSpec } from "../src/lib/generation/generation-spec";
import { renderDrumLoop } from "../src/lib/generation/loop-render";
import { masterSample } from "../src/lib/generation/post-process";
import { resolveDrumParams } from "../src/lib/generation/prompt-params";
import { SAMPLE_NAMES } from "../src/lib/generation/synth";
import { analyzeQuality } from "../src/lib/generation/quality-gate";

const __dir = dirname(fileURLToPath(import.meta.url));
const baseline = JSON.parse(
  readFileSync(join(__dir, "factory-dsp-baseline.json"), "utf8"),
) as Record<
  string,
  { minFrames: number; maxFrames: number; minPeak: number; minRms: number }
>;

function stats(samples: Float32Array) {
  let peak = 0;
  let sumSq = 0;
  for (const s of samples) {
    peak = Math.max(peak, Math.abs(s));
    sumSq += s * s;
  }
  return { peak, rms: Math.sqrt(sumSq / samples.length), frames: samples.length };
}

function assertSample(name: string, mono: Float32Array) {
  const b = baseline[name];
  if (!b) throw new Error(`missing baseline for ${name}`);
  const s = stats(mono);
  const q = analyzeQuality(mono);
  if (s.frames < b.minFrames || s.frames > b.maxFrames) {
    throw new Error(`${name}: frames ${s.frames} outside [${b.minFrames}, ${b.maxFrames}]`);
  }
  if (s.peak < b.minPeak) throw new Error(`${name}: peak ${s.peak} < ${b.minPeak}`);
  if (s.rms < b.minRms) throw new Error(`${name}: rms ${s.rms} < ${b.minRms}`);
  if (!q.pass && q.reasons.includes("mostly_silent")) {
    throw new Error(`${name}: quality gate mostly_silent`);
  }
}

const presetId = "boulevard-86";
const prompt = "memphis phonk 808 trap regression";
const spec = { ...defaultGenerationSpec(presetId), mode: "one_shot" as const, engine: "creative" as const };
const params = resolveDrumParams(prompt, presetId, spec);
const grit = gritFromParams(params, prompt);

for (const name of SAMPLE_NAMES) {
  const raw = renderSampleForEngine(name, params, spec.engine);
  const { mono } = masterSample(raw, params, spec, spec.engine, grit);
  assertSample(name, mono);
}

const loopSpec = { ...spec, mode: "loop" as const, bpm: 140, bars: 4, wildness: 0.65 };
const loopParams = resolveDrumParams(prompt, presetId, loopSpec);
const loopRaw = renderDrumLoop(loopParams, loopSpec, loopSpec.engine);
const loopGrit = gritFromParams(loopParams, prompt);
const { mono: loopMono } = masterSample(loopRaw, loopParams, loopSpec, loopSpec.engine, loopGrit);
if (loopMono.length < 44100 * 4) {
  throw new Error(`loop too short: ${loopMono.length} samples`);
}
const loopStats = stats(loopMono);
if (loopStats.peak < 0.04) throw new Error(`loop peak too low: ${loopStats.peak}`);

type GoldenLane = { presetId: string; prompt: string; engine: "creative" | "studio" };
const golden = JSON.parse(
  readFileSync(join(__dir, "factory-golden-lanes.json"), "utf8"),
) as GoldenLane[];

for (const lane of golden) {
  const laneSpec = {
    ...defaultGenerationSpec(lane.presetId),
    mode: "one_shot" as const,
    engine: lane.engine,
  };
  const laneParams = resolveDrumParams(lane.prompt, lane.presetId, laneSpec);
  const laneGrit = gritFromParams(laneParams, lane.prompt);
  const kickRaw = renderSampleForEngine("kick", laneParams, lane.engine);
  const { mono: kickMono } = masterSample(kickRaw, laneParams, laneSpec, lane.engine, laneGrit);
  const kickStats = stats(kickMono);
  if (kickStats.peak < 0.06) {
    throw new Error(`golden kick too quiet: ${lane.presetId} peak=${kickStats.peak}`);
  }
}

console.log("factory:dsp-regression OK", {
  presetId,
  samples: SAMPLE_NAMES.length,
  loopFrames: loopMono.length,
  loopPeak: loopStats.peak,
  goldenLanes: golden.length,
});
