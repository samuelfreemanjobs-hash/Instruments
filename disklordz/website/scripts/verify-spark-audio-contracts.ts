/**
 * SparkLE Phase 2 audio + manifest contracts. Run: npm run verify:spark-audio-contracts
 */
import { readFileSync } from "fs";
import { join } from "path";

import { renderSampleForEngine } from "../src/lib/generation/engine-render";
import { defaultGenerationSpec } from "../src/lib/generation/generation-spec";
import { renderDrumLoop } from "../src/lib/generation/loop-render";
import { resolveDrumParams } from "../src/lib/generation/prompt-params";
import {
  evaluateLoopGridContract,
  evaluateManifestContract,
  evaluateSampleContracts,
  type ContractViolation,
  type SparkManifestShape,
} from "../src/lib/spark-le/audio-contracts";

const FIXTURE_PRESETS = [
  { presetId: "lane-jeezy", prompt: "hard gangsta trap knock" },
  { presetId: "lane-trap-or-die", prompt: "trap or die street" },
  { presetId: "lane-dj-screw", prompt: "chop screw slow" },
  { presetId: "lane-smooth-jazz-rap", prompt: "smooth jazz rap" },
  { presetId: "lane-fat-pat", prompt: "fat pat syrupy trunk" },
  { presetId: "midnight-circuit", prompt: "drift phonk" },
] as const;

const MANIFEST_PATHS = [
  "../../docs/disklordz/templates/TEMPLATE-spark-le-manifest.json",
  "../../docs/disklordz/spark-le/examples/manifest-smooth-jazz-rap.json",
  "../../docs/disklordz/spark-le/examples/manifest-gangsta-trap.json",
  "../../docs/disklordz/spark-le/examples/manifest-dj-screw.json",
  "../../docs/disklordz/spark-le/examples/manifest-trunk-sermon.json",
];

/** Match encodeWav peak scaling (SOP-014 NO-CLIP on exports). */
function normalizeExportPeak(pcm: Float32Array): Float32Array {
  let peak = 0;
  for (const s of pcm) peak = Math.max(peak, Math.abs(s));
  const scale = peak > 0 ? 0.89 / peak : 1;
  const out = new Float32Array(pcm.length);
  for (let i = 0; i < pcm.length; i++) out[i] = pcm[i] * scale;
  return out;
}

function fail(v: ContractViolation): never {
  const pad = v.pad ? ` pad=${v.pad}` : "";
  console.error(
    `verify-spark-audio-contracts: [${v.contractId}] ${v.presetId}${pad}: ${v.message} (metric=${v.metric}, bound=${v.bound})`,
  );
  process.exit(1);
}

let violationCount = 0;

for (const { presetId, prompt } of FIXTURE_PRESETS) {
  const spec = defaultGenerationSpec(presetId);
  spec.mode = "one_shot";
  const params = resolveDrumParams(prompt, presetId, spec);

  for (const pad of ["kick", "snare", "hat_closed"] as const) {
    const name = pad === "hat_closed" ? "hat_closed" : pad;
    const pcm = normalizeExportPeak(renderSampleForEngine(name, params, "studio"));
    for (const v of evaluateSampleContracts(presetId, pad, pcm)) {
      fail(v);
    }
  }

  spec.mode = "loop";
  spec.bars = 2;
  const loop = normalizeExportPeak(renderDrumLoop(params, spec, "studio"));
  for (const v of evaluateLoopGridContract(presetId, loop)) {
    fail(v);
  }
}

for (const rel of MANIFEST_PATHS) {
  const full = join(process.cwd(), rel);
  let raw: string;
  try {
    raw = readFileSync(full, "utf8");
  } catch {
    console.error(`verify-spark-audio-contracts: missing manifest ${rel}`);
    process.exit(1);
  }
  const manifest = JSON.parse(raw) as SparkManifestShape;
  for (const v of evaluateManifestContract(rel, manifest)) {
    fail(v);
  }
}

console.log(
  `verify-spark-audio-contracts: OK (${FIXTURE_PRESETS.length} lane fixtures, ${MANIFEST_PATHS.length} manifests, ${violationCount} violations)`,
);
