/**
 * SparkLE elite lane registry — single source for preset ids, manifest lane IDs, and drum bases.
 * Sync note: disklordz/sound-factory/scripts/generate_kit.py should mirror PRESET_BASE keys.
 */

export type SparkLaneFamily =
  | "core"
  | "memphis"
  | "atlanta"
  | "south-texas"
  | "chicago"
  | "hardware"
  | "regional-trap"
  | "rage";

export type SparkLaneDrumBase = {
  kickPitch: number;
  kickDecay: number;
  snareBody: number;
  snareSnap: number;
  hatDecay: number;
  hatBright: number;
  rimMetal: number;
  clapWide: number;
};

export type SparkLaneDef = {
  /** SaaS preset id */
  presetId: string;
  /** Manifest / SOP lane id (uppercase) */
  laneId: string;
  label: string;
  artistLane: string;
  family: SparkLaneFamily;
  bpmHint: string;
  description: string;
  tags: string[];
  drum: SparkLaneDrumBase;
  /** Multipliers applied after prompt tokens when this preset is selected */
  nudge?: Partial<Record<keyof SparkLaneDrumBase, number>>;
};

const d = (
  presetId: string,
  laneId: string,
  label: string,
  artistLane: string,
  family: SparkLaneFamily,
  bpmHint: string,
  description: string,
  tags: string[],
  drum: SparkLaneDrumBase,
  nudge?: SparkLaneDef["nudge"],
): SparkLaneDef => ({
  presetId,
  laneId,
  label,
  artistLane,
  family,
  bpmHint,
  description,
  tags,
  drum,
  nudge,
});

/** Style archetypes — original procedural drums; not affiliated with named artists. */
export const SPARK_LANE_REGISTRY: SparkLaneDef[] = [
  d(
    "lane-jeezy",
    "JEEZY",
    "Jeezy lane",
    "DL-JEEZY",
    "atlanta",
    "68–76 / 136–152",
    "Trap-or-die 808 weight, dark knock, anthem snare.",
    ["jeezy", "trap", "808", "archetype"],
    { kickPitch: 39, kickDecay: 0.38, snareBody: 0.42, snareSnap: 0.38, hatDecay: 0.028, hatBright: 0.32, rimMetal: 0.4, clapWide: 0.38 },
    { kickDecay: 1.05 },
  ),
  d(
    "lane-gucci-mane",
    "GUCCI-MANE",
    "Gucci Mane lane",
    "DL-GUCCI",
    "atlanta",
    "70–78 / 140–156",
    "Bouncy ATL sub, wide clap, quirky perc.",
    ["gucci", "atl", "bounce", "archetype"],
    { kickPitch: 42, kickDecay: 0.3, snareBody: 0.28, snareSnap: 0.48, hatDecay: 0.02, hatBright: 0.48, rimMetal: 0.55, clapWide: 0.52 },
    { clapWide: 1.06 },
  ),
  d(
    "lane-mike-will",
    "MIKE-WILL",
    "Mike Will Made It lane",
    "DL-MIKEWILL",
    "atlanta",
    "130–160",
    "Hard modern trap — tight knock, minimal hats.",
    ["mike will", "trap", "hard", "archetype"],
    { kickPitch: 44, kickDecay: 0.24, snareBody: 0.22, snareSnap: 0.52, hatDecay: 0.016, hatBright: 0.5, rimMetal: 0.45, clapWide: 0.44 },
    { hatDecay: 0.92 },
  ),
  d(
    "lane-drumma-boy",
    "DRUMMA-BOY",
    "Drumma Boy lane",
    "DL-DRUMMA",
    "south-texas",
    "72–88 / 144–176",
    "Southern anthem snare crack, rim, FX hit slot.",
    ["drumma boy", "south", "anthem", "archetype"],
    { kickPitch: 46, kickDecay: 0.32, snareBody: 0.38, snareSnap: 0.5, hatDecay: 0.024, hatBright: 0.42, rimMetal: 0.62, clapWide: 0.48 },
    { snareSnap: 1.04 },
  ),
  d(
    "lane-zaytoven",
    "ZAYTOVEN",
    "Zaytoven lane",
    "DL-ZAYTOVEN",
    "atlanta",
    "130–155",
    "ATL pocket — flutter hats, lighter knock.",
    ["zaytoven", "atl", "hats", "archetype"],
    { kickPitch: 48, kickDecay: 0.26, snareBody: 0.26, snareSnap: 0.42, hatDecay: 0.012, hatBright: 0.46, rimMetal: 0.48, clapWide: 0.4 },
    { hatDecay: 0.88 },
  ),
  d(
    "lane-d-rich",
    "D-RICH",
    "D-Rich lane",
    "DL-DRICH",
    "atlanta",
    "130–155",
    "ATL trunk bounce — knock-heavy, club weight (Zaytoven-adjacent).",
    ["d-rich", "d rich", "atl", "trunk", "archetype"],
    { kickPitch: 45, kickDecay: 0.3, snareBody: 0.3, snareSnap: 0.46, hatDecay: 0.018, hatBright: 0.44, rimMetal: 0.5, clapWide: 0.46 },
    { kickDecay: 1.03, snareSnap: 1.03 },
  ),
  d(
    "lane-ti",
    "TI",
    "T.I. lane",
    "DL-TI",
    "south-texas",
    "80–100 / 160–200",
    "Swagger bounce, clean clap stack, crisp hats.",
    ["ti", "south", "bounce", "archetype"],
    { kickPitch: 47, kickDecay: 0.29, snareBody: 0.32, snareSnap: 0.44, hatDecay: 0.02, hatBright: 0.52, rimMetal: 0.58, clapWide: 0.5 },
  ),
  d(
    "lane-dj-toomp",
    "DJ-TOOMP",
    "DJ Toomp lane",
    "DL-TOOMP",
    "atlanta",
    "65–80 / 130–160",
    "Trunk-rattling ATL 808 — thick sub, hard knock, street anthem.",
    ["dj toomp", "toomp", "atl", "trunk", "archetype"],
    { kickPitch: 37, kickDecay: 0.42, snareBody: 0.36, snareSnap: 0.46, hatDecay: 0.026, hatBright: 0.36, rimMetal: 0.44, clapWide: 0.42 },
    { kickDecay: 1.06, hatBright: 0.92 },
  ),
  d(
    "lane-dj-paul",
    "DJ-PAUL",
    "DJ Paul lane",
    "DL-DJPAUL",
    "memphis",
    "65–90 / 130–180",
    "Memphis horror-crunch — lo-fi snap, dark hats, cowbell-ready.",
    ["dj paul", "memphis", "crunch", "archetype"],
    { kickPitch: 33, kickDecay: 0.5, snareBody: 0.48, snareSnap: 0.62, hatDecay: 0.038, hatBright: 0.2, rimMetal: 0.42, clapWide: 0.3 },
    { snareSnap: 1.05, hatBright: 0.88 },
  ),
  d(
    "lane-three-six-mafia",
    "THREE-SIX-MAFIA",
    "Three 6 Mafia lane",
    "DL-THREE6",
    "memphis",
    "65–95 / 130–190",
    "Hypnotic Memphis — crushed snare, dusty tops, perc on 8.",
    ["three 6", "36 mafia", "memphis", "archetype"],
    { kickPitch: 32, kickDecay: 0.52, snareBody: 0.5, snareSnap: 0.64, hatDecay: 0.04, hatBright: 0.18, rimMetal: 0.4, clapWide: 0.28 },
    { snareSnap: 1.06 },
  ),
  d(
    "lane-juicy-j",
    "JUICY-J",
    "Juicy J lane",
    "DL-JUICYJ",
    "memphis",
    "130–160",
    "Dark Memphis trap bounce — heavy 808, triplet hat space.",
    ["juicy j", "memphis", "trap", "archetype"],
    { kickPitch: 40, kickDecay: 0.36, snareBody: 0.34, snareSnap: 0.54, hatDecay: 0.022, hatBright: 0.38, rimMetal: 0.48, clapWide: 0.4 },
  ),
  d(
    "lane-shawty-pimp",
    "SHAWTY-PIMP",
    "Shawty Pimp lane",
    "DL-SHAWTYPIMP",
    "memphis",
    "60–85",
    "Underground Memphis slow strip — long decay, minimal tops.",
    ["shawty pimp", "memphis", "slow", "archetype"],
    { kickPitch: 34, kickDecay: 0.58, snareBody: 0.52, snareSnap: 0.48, hatDecay: 0.042, hatBright: 0.16, rimMetal: 0.35, clapWide: 0.26 },
    { kickDecay: 1.1 },
  ),
  d(
    "lane-shawty-redd",
    "SHAWTY-REDD",
    "Shawty Redd lane",
    "DL-SHAWTYREDD",
    "atlanta",
    "70–88 / 140–176",
    "Dirty south loop culture — thick kick, clap stack, swing.",
    ["shawty redd", "dirty south", "loops", "archetype"],
    { kickPitch: 44, kickDecay: 0.34, snareBody: 0.4, snareSnap: 0.47, hatDecay: 0.024, hatBright: 0.4, rimMetal: 0.52, clapWide: 0.5 },
    { snareBody: 1.05, clapWide: 1.04 },
  ),
  d(
    "lane-young-chop",
    "YOUNG-CHOP",
    "Young Chop lane",
    "DL-YOUNGCHOP",
    "chicago",
    "130–150",
    "Early drill pressure — tight 808, violent snare snap, fast hats.",
    ["young chop", "chicago", "drill", "archetype"],
    { kickPitch: 41, kickDecay: 0.27, snareBody: 0.24, snareSnap: 0.58, hatDecay: 0.014, hatBright: 0.48, rimMetal: 0.46, clapWide: 0.38 },
    { snareSnap: 1.05, hatDecay: 0.9 },
  ),
  d(
    "lane-est-g",
    "EST-G",
    "EST G lane",
    "DL-ESTG",
    "memphis",
    "130–155",
    "Hard Memphis street trap — dark sub, aggressive snap.",
    ["est g", "est gee", "memphis", "street", "archetype"],
    { kickPitch: 38, kickDecay: 0.34, snareBody: 0.3, snareSnap: 0.56, hatDecay: 0.02, hatBright: 0.34, rimMetal: 0.44, clapWide: 0.4 },
  ),
  d(
    "lane-young-dolph",
    "YOUNG-DOLPH",
    "Young Dolph lane",
    "DL-DOLPH",
    "memphis",
    "130–160",
    "Memphis hard bounce — confident knock, paper-tag energy on 8.",
    ["young dolph", "dolph", "memphis", "bounce", "archetype"],
    { kickPitch: 40, kickDecay: 0.32, snareBody: 0.32, snareSnap: 0.5, hatDecay: 0.019, hatBright: 0.42, rimMetal: 0.5, clapWide: 0.44 },
  ),
  d(
    "lane-ugk",
    "UGK",
    "UGK lane",
    "DL-UGK",
    "south-texas",
    "70–90 / 140–180",
    "Houston pocket — trunk kick, rim rides, unhurried hats.",
    ["ugk", "houston", "pocket", "archetype"],
    { kickPitch: 48, kickDecay: 0.36, snareBody: 0.36, snareSnap: 0.4, hatDecay: 0.028, hatBright: 0.38, rimMetal: 0.64, clapWide: 0.44 },
    { rimMetal: 1.05 },
  ),
  d(
    "lane-pimp-c",
    "PIMP-C",
    "Pimp C lane",
    "DL-PIMPC",
    "south-texas",
    "68–88 / 136–176",
    "Texas trunk philosophy — fat sub, body snare, soul in the pocket.",
    ["pimp c", "ugk", "texas", "trunk", "archetype"],
    { kickPitch: 46, kickDecay: 0.4, snareBody: 0.42, snareSnap: 0.42, hatDecay: 0.03, hatBright: 0.36, rimMetal: 0.6, clapWide: 0.46 },
    { kickDecay: 1.04, snareBody: 1.04 },
  ),
  d(
    "lane-dr-660",
    "DR-660",
    "Roland DR-660 lane",
    "DL-DR660",
    "hardware",
    "90–120 / any",
    "Drum machine punch — short decay, electronic snare, machine hats.",
    ["dr-660", "dr660", "roland", "machine", "archetype"],
    { kickPitch: 50, kickDecay: 0.2, snareBody: 0.28, snareSnap: 0.36, hatDecay: 0.018, hatBright: 0.52, rimMetal: 0.5, clapWide: 0.38 },
    { kickDecay: 0.95 },
  ),
  d(
    "lane-tr-808-clone",
    "TR-808-CLONE",
    "TR-808 clone lane",
    "DL-TR808",
    "hardware",
    "any",
    "Repo-aligned 808 clone voice — canonical sub, rim, cowbell slot.",
    ["tr-808", "808 clone", "analog", "archetype"],
    { kickPitch: 43, kickDecay: 0.32, snareBody: 0.22, snareSnap: 0.38, hatDecay: 0.024, hatBright: 0.44, rimMetal: 0.55, clapWide: 0.4 },
  ),
  d(
    "lane-bay-area-trap",
    "BAY-AREA-TRAP",
    "Bay Area trap lane",
    "DL-BAY",
    "regional-trap",
    "85–110 / 170–220",
    "Hyphy-adjacent trap — slap knock, bright hats, street energy.",
    ["bay area", "hyphy", "sf", "oakland", "archetype"],
    { kickPitch: 43, kickDecay: 0.26, snareBody: 0.26, snareSnap: 0.5, hatDecay: 0.016, hatBright: 0.58, rimMetal: 0.48, clapWide: 0.45 },
    { hatBright: 1.06 },
  ),
  d(
    "lane-detroit-trap",
    "DETROIT-TRAP",
    "Detroit trap / street rap lane",
    "DL-DETROIT",
    "regional-trap",
    "130–160",
    "Industrial street trap — hard snap, bleak hats, pressure.",
    ["detroit", "street rap", "trap", "archetype"],
    { kickPitch: 42, kickDecay: 0.28, snareBody: 0.26, snareSnap: 0.54, hatDecay: 0.015, hatBright: 0.5, rimMetal: 0.47, clapWide: 0.42 },
    { snareSnap: 1.04 },
  ),
  d(
    "lane-rage-trap",
    "RAGE-TRAP",
    "Rage trap lane",
    "DL-RAGE",
    "rage",
    "140–160",
    "Rage energy — short 808, frantic hats, mosh pressure.",
    ["rage", "rage trap", "hyper", "archetype"],
    { kickPitch: 40, kickDecay: 0.22, snareBody: 0.2, snareSnap: 0.48, hatDecay: 0.01, hatBright: 0.54, rimMetal: 0.42, clapWide: 0.4 },
    { hatDecay: 0.88, kickDecay: 0.94 },
  ),
];

export const SPARK_PRESET_ALIASES: Record<string, string> = {
  "lane-dj-doomp": "lane-dj-toomp",
};

export function resolveSparkPresetId(presetId: string): string {
  return SPARK_PRESET_ALIASES[presetId] ?? presetId;
}

export function getSparkLaneByPresetId(presetId: string): SparkLaneDef | undefined {
  const id = resolveSparkPresetId(presetId);
  return SPARK_LANE_REGISTRY.find((l) => l.presetId === id);
}

export function sparkLanePresetBases(): Record<string, SparkLaneDrumBase> {
  const out: Record<string, SparkLaneDrumBase> = {};
  for (const lane of SPARK_LANE_REGISTRY) {
    out[lane.presetId] = lane.drum;
  }
  return out;
}

export function applySparkLaneNudges(
  presetId: string,
  state: SparkLaneDrumBase & { kickPitch: number },
): void {
  const lane = getSparkLaneByPresetId(presetId);
  if (!lane?.nudge) return;
  for (const [key, mult] of Object.entries(lane.nudge) as [keyof SparkLaneDrumBase, number][]) {
    if (key === "kickPitch") state.kickPitch *= mult;
    else state[key] *= mult;
  }
}
