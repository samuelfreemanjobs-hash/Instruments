/**
 * Export a SparkLE-ready folder from an example manifest (8 one-shots + loops).
 *
 * Usage:
 *   npx tsx scripts/export-spark-sprint-kit.ts \
 *     --manifest ../../docs/disklordz/spark-le/examples/manifest-smooth-jazz-rap.json \
 *     --out ../../docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap \
 *     --prompt "smooth jazz rap smoky cruise" \
 *     --seed 86031103
 */
import { mkdirSync, readFileSync, writeFileSync } from "fs";
import { dirname, join } from "path";

import { renderSampleForEngine } from "../src/lib/generation/engine-render";
import { defaultGenerationSpec, type GenerationSpec } from "../src/lib/generation/generation-spec";
import { renderDrumLoop } from "../src/lib/generation/loop-render";
import { resolveDrumParams } from "../src/lib/generation/prompt-params";
import type { DrumParams } from "../src/lib/generation/prompt-params";
import { encodeWav } from "../src/lib/generation/wav";
import {
  evaluateLoopGridContract,
  evaluateSampleContracts,
} from "../src/lib/spark-le/audio-contracts";

const SAMPLE_RATE = 44100;

type LoopProfile = "trap" | "screw" | "smooth-jazz" | "southern-trunk";

type SparkManifest = {
  preset_id: string;
  bpm: number;
  dna_template_id?: string | null;
  spark_le: {
    pads: Record<
      string,
      { role: string; file: string; target_lufs?: number }
    >;
  };
  loops?: { id: string; file: string; bars: number }[];
};

function parseArgs(): {
  manifestPath: string;
  outDir: string;
  prompt: string;
  seed: number;
} {
  const argv = process.argv.slice(2);
  let manifestPath = "";
  let outDir = "";
  let prompt = "smooth jazz rap";
  let seed = 42;
  for (let i = 0; i < argv.length; i++) {
    if (argv[i] === "--manifest") manifestPath = argv[++i] ?? "";
    else if (argv[i] === "--out") outDir = argv[++i] ?? "";
    else if (argv[i] === "--prompt") prompt = argv[++i] ?? prompt;
    else if (argv[i] === "--seed") seed = parseInt(argv[++i] ?? "42", 10);
  }
  if (!manifestPath || !outDir) {
    console.error(
      "Usage: export-spark-sprint-kit.ts --manifest <path> --out <dir> [--prompt] [--seed]",
    );
    process.exit(1);
  }
  return {
    manifestPath: join(process.cwd(), manifestPath),
    outDir: join(process.cwd(), outDir),
    prompt,
    seed,
  };
}

function normalizeExportPeak(pcm: Float32Array): Float32Array {
  let peak = 0;
  for (const s of pcm) peak = Math.max(peak, Math.abs(s));
  const scale = peak > 0 ? 0.89 / peak : 1;
  const out = new Float32Array(pcm.length);
  for (let i = 0; i < pcm.length; i++) out[i] = pcm[i] * scale;
  return out;
}

function mixAt(buf: Float32Array, offset: number, sample: Float32Array, gain = 1): void {
  for (let i = 0; i < sample.length; i++) {
    const idx = offset + i;
    if (idx >= buf.length) break;
    buf[idx] += sample[i] * gain;
  }
}

function renderPadPcm(role: string, params: DrumParams, engine: "studio"): Float32Array {
  switch (role) {
    case "808_sub":
      return renderSampleForEngine("kick", params, engine);
    case "808_knock": {
      const knockParams = {
        ...params,
        kickDecay: params.kickDecay * 0.28,
        kickPitch: params.kickPitch * 1.12,
      };
      return renderSampleForEngine("kick", knockParams, engine);
    }
    case "snare_body":
      return renderSampleForEngine("snare", params, engine);
    case "snare_snap_clap":
      return renderSampleForEngine("clap", params, engine);
    case "hat_closed":
      return renderSampleForEngine("hat_closed", params, engine);
    case "hat_open":
      return renderSampleForEngine("hat_open", params, engine);
    case "rim_perc":
      return renderSampleForEngine("rim", params, engine);
    case "cowbell_fx":
      return renderSampleForEngine("rim", { ...params, rimMetal: params.rimMetal * 0.75 }, engine);
    default:
      return renderSampleForEngine("rim", params, engine);
  }
}

/** smooth-jazz-8ths: 8th hats, soft backbeat, no 32nd rolls (SOP sprint session 3). */
function renderSmoothJazzLoop(
  params: DrumParams,
  bpm: number,
  bars: number,
  engine: "studio",
  includeKick: boolean,
): Float32Array {
  const beats = bars * 4;
  const seconds = (60 / bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);

  const kick = renderSampleForEngine("kick", params, engine);
  const snare = renderSampleForEngine("snare", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);
  const rim = renderSampleForEngine("rim", params, engine);

  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  const eighthSamples = Math.floor(beatSamples / 2);

  for (let beat = 0; beat < beats; beat++) {
    const beatOffset = Math.floor(beat * beatSamples);
    if (includeKick && beat % 4 === 0) {
      mixAt(out, beatOffset, kick, 0.72);
    }
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, beatOffset, snare, 0.62);
      mixAt(out, beatOffset, rim, 0.28);
    }
    for (let eighth = 0; eighth < 2; eighth++) {
      const off = beatOffset + eighth * eighthSamples;
      const ghost = eighth === 1 ? 0.22 : 0.38;
      mixAt(out, off, hat, ghost);
    }
  }

  return normalizeExportPeak(out);
}

function loopProfile(manifest: SparkManifest): LoopProfile {
  if (manifest.dna_template_id === "smooth-jazz-8ths") return "smooth-jazz";
  if (
    manifest.dna_template_id === "southern-trunk-lazy" ||
    manifest.preset_id === "lane-fat-pat"
  ) {
    return "southern-trunk";
  }
  if (manifest.preset_id === "lane-dj-screw" || manifest.bpm <= 80) return "screw";
  return "trap";
}

/** Trap / gangsta: 4/4 with optional kick (kickless tops export). */
function renderTrapStyleLoop(
  params: DrumParams,
  bpm: number,
  bars: number,
  engine: "studio",
  includeKick: boolean,
): Float32Array {
  const spec: GenerationSpec = {
    mode: "loop",
    engine,
    key: "C",
    bpm,
    bars,
    length: "short",
    stereo: 0,
    wildness: 0,
  };
  if (includeKick) {
    return renderDrumLoop(params, spec, engine);
  }
  const beats = bars * 4;
  const seconds = (60 / bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);
  const snare = renderSampleForEngine("snare", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);
  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  for (let beat = 0; beat < beats; beat++) {
    const offset = Math.floor(beat * beatSamples);
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, offset, snare, 0.75);
    }
    mixAt(out, offset, hat, 0.35);
    if (beat % 2 === 1) {
      mixAt(out, offset + Math.floor(beatSamples / 2), hat, 0.25);
    }
  }
  return normalizeExportPeak(out);
}

/** Screw / slowed: sparse hats, trunk kick on 1 & 3. */
function renderScrewStyleLoop(
  params: DrumParams,
  bpm: number,
  bars: number,
  engine: "studio",
  includeKick: boolean,
): Float32Array {
  const beats = bars * 4;
  const seconds = (60 / bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);
  const kick = renderSampleForEngine("kick", params, engine);
  const snare = renderSampleForEngine("snare", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);
  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  for (let beat = 0; beat < beats; beat++) {
    const offset = Math.floor(beat * beatSamples);
    if (includeKick && (beat % 4 === 0 || beat % 4 === 2)) {
      mixAt(out, offset, kick, 0.88);
    }
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, offset, snare, 0.65);
    }
    if (beat % 2 === 0) {
      mixAt(out, offset, hat, 0.22);
    }
  }
  return normalizeExportPeak(out);
}

/** Fat Pat / Trunk Sermon — lazy 8ths, rim ride, kick 1 & 3. */
function renderSouthernTrunkLoop(
  params: DrumParams,
  bpm: number,
  bars: number,
  engine: "studio",
  includeKick: boolean,
): Float32Array {
  const beats = bars * 4;
  const seconds = (60 / bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);

  const kick = renderSampleForEngine("kick", params, engine);
  const snare = renderSampleForEngine("snare", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);
  const rim = renderSampleForEngine("rim", params, engine);

  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  const eighthSamples = Math.floor(beatSamples / 2);

  for (let beat = 0; beat < beats; beat++) {
    const beatOffset = Math.floor(beat * beatSamples);
    if (includeKick && (beat % 4 === 0 || beat % 4 === 2)) {
      mixAt(out, beatOffset, kick, 0.82);
    }
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, beatOffset, snare, 0.78);
      mixAt(out, beatOffset, rim, 0.42);
    }
    for (let eighth = 0; eighth < 2; eighth++) {
      if (beat % 2 === 0 && eighth === 1) continue;
      const off = beatOffset + eighth * eighthSamples;
      mixAt(out, off, hat, eighth === 0 ? 0.28 : 0.18);
    }
  }

  return normalizeExportPeak(out);
}

function renderRimRideLoop(
  params: DrumParams,
  bpm: number,
  bars: number,
  engine: "studio",
): Float32Array {
  const beats = bars * 4;
  const seconds = (60 / bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);
  const rim = renderSampleForEngine("rim", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);
  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  for (let beat = 0; beat < beats; beat++) {
    const beatOffset = Math.floor(beat * beatSamples);
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, beatOffset, rim, 0.72);
    }
    if (beat % 4 === 0) {
      mixAt(out, beatOffset, hat, 0.12);
    }
  }
  return normalizeExportPeak(out);
}

function renderLoopForManifest(
  manifest: SparkManifest,
  params: DrumParams,
  loopId: string,
  bars: number,
  engine: "studio",
): Float32Array {
  const includeKick = loopId !== "tops_kickless";
  const profile = loopProfile(manifest);
  const bpm = manifest.bpm;
  if (loopId === "rim_ride") {
    return renderRimRideLoop(params, bpm, bars, engine);
  }
  switch (profile) {
    case "smooth-jazz":
      return renderSmoothJazzLoop(params, bpm, bars, engine, includeKick);
    case "screw":
      return renderScrewStyleLoop(params, bpm, bars, engine, includeKick);
    case "southern-trunk":
      return renderSouthernTrunkLoop(params, bpm, bars, engine, includeKick);
    default:
      return renderTrapStyleLoop(params, bpm, bars, engine, includeKick);
  }
}

function writeWav(outDir: string, relFile: string, pcm: Float32Array): void {
  const full = join(outDir, relFile);
  mkdirSync(dirname(full), { recursive: true });
  writeFileSync(full, encodeWav(pcm));
}

const { manifestPath, outDir, prompt, seed } = parseArgs();
const raw = readFileSync(manifestPath, "utf8");
const manifest = JSON.parse(raw) as SparkManifest;
const presetId = manifest.preset_id;
const spec = defaultGenerationSpec(presetId);
spec.mode = "one_shot";
spec.bpm = manifest.bpm;
const params = resolveDrumParams(prompt, presetId, spec, { seed });

mkdirSync(outDir, { recursive: true });

for (const [, pad] of Object.entries(manifest.spark_le.pads)) {
  const pcm = normalizeExportPeak(renderPadPcm(pad.role, params, "studio"));
  const contractPad = padRoleToContractPad(pad.role);
  if (contractPad) {
    for (const v of evaluateSampleContracts(presetId, contractPad, pcm)) {
      console.error(`export-spark-sprint-kit: contract fail ${v.contractId}: ${v.message}`);
      process.exit(1);
    }
  }
  writeWav(outDir, pad.file, pcm);
}

for (const loop of manifest.loops ?? []) {
  const pcm = renderLoopForManifest(manifest, params, loop.id, loop.bars, "studio");
  for (const v of evaluateLoopGridContract(presetId, pcm)) {
    console.error(
      `export-spark-sprint-kit: loop contract fail (${loop.id}): ${v.message} metric=${v.metric} bound=${v.bound}`,
    );
    process.exit(1);
  }
  writeWav(outDir, loop.file, pcm);
}

const exportManifest = {
  ...manifest,
  generator_version: "export-spark-sprint-kit-v1",
  batch_id: `sprint-export-${seed}`,
  factory: { seed, prompt, preset_id: presetId, engine: "studio" },
};
writeFileSync(join(outDir, "spark-le-manifest.json"), JSON.stringify(exportManifest, null, 2));

console.log(`export-spark-sprint-kit: OK → ${outDir} (preset=${presetId}, bpm=${manifest.bpm}, seed=${seed})`);

function padRoleToContractPad(role: string): "kick" | "snare" | "hat_closed" | null {
  if (role === "808_sub") return "kick";
  if (role === "snare_body") return "snare";
  if (role === "hat_closed" || role === "hat_open") return "hat_closed";
  return null;
}
