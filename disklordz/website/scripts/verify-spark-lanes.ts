/**
 * SparkLE lane registry contract checks (SOP-014). Run: npm run verify:spark-lanes
 */
import {
  SPARK_LANE_REGISTRY,
  SPARK_PRESET_ALIASES,
  resolveSparkPresetId,
} from "../src/lib/spark-le/lane-registry";
import {
  coalesceSparkPreset,
  resolveSparkPresetIdFromPrompt,
} from "../src/lib/spark-le/lane-resolve";

function fail(msg: string): never {
  console.error(`verify-spark-lanes: ${msg}`);
  process.exit(1);
}

const presetIds = new Set<string>();
const laneIds = new Set<string>();

for (const lane of SPARK_LANE_REGISTRY) {
  if (presetIds.has(lane.presetId)) fail(`duplicate presetId ${lane.presetId}`);
  if (laneIds.has(lane.laneId)) fail(`duplicate laneId ${lane.laneId}`);
  presetIds.add(lane.presetId);
  laneIds.add(lane.laneId);

  const { drum } = lane;
  if (drum.kickPitch < 28 || drum.kickPitch > 72) fail(`${lane.presetId} kickPitch out of range`);
  if (drum.kickDecay < 0.1 || drum.kickDecay > 0.7) fail(`${lane.presetId} kickDecay out of range`);
}

for (const [alias, target] of Object.entries(SPARK_PRESET_ALIASES)) {
  const resolved = resolveSparkPresetId(alias);
  if (resolved !== target) fail(`alias ${alias} resolve mismatch`);
  if (!presetIds.has(target)) fail(`alias ${alias} points to missing ${target}`);
}

const inferenceCases: [string, string][] = [
  ["fat pat screwed trunk", "lane-fat-pat"],
  ["90s snoop dogg g funk pocket", "lane-snoop-90s"],
  ["bryson tiller trap soul night", "lane-bryson-tiller"],
  ["metro boomin dark roll", "lane-metro-boomin"],
  ["dj screw chop slow", "lane-dj-screw"],
  ["majestic casual chill hip hop", "lane-majestic-casual"],
  ["crunk music southern club", "lane-crunk"],
  ["lofi jazz trap dusty beat", "lane-lofi-jazz-trap"],
  ["classic nola bounce triggerman", "lane-nola-bounce-classic"],
  ["flint michigan street rap", "lane-flint-michigan"],
  ["smooth jazz rap smoky cruise", "lane-smooth-jazz-rap"],
];

for (const [prompt, expected] of inferenceCases) {
  const got = resolveSparkPresetIdFromPrompt(prompt);
  if (got !== expected) {
    fail(`inference "${prompt}" expected ${expected}, got ${got ?? "undefined"}`);
  }
}

const coalesced = coalesceSparkPreset("lane-sza sparse alt rnb", "mpc-ready-808");
if (coalesced !== "lane-sza") {
  fail(`coalesce expected lane-sza, got ${coalesced}`);
}

console.log(
  `verify-spark-lanes: OK (${SPARK_LANE_REGISTRY.length} lanes, ${Object.keys(SPARK_PRESET_ALIASES).length} aliases)`,
);
