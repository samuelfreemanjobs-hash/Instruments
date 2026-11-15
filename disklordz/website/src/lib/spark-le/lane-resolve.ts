import {
  resolveSparkPresetId,
  SPARK_LANE_REGISTRY,
  SPARK_PRESET_ALIASES,
  type SparkLaneDef,
} from "@/lib/spark-le/lane-registry";

/** Presets that defer drum character to prompt-matched lanes. */
export const SPARK_GENERIC_PRESET_IDS = new Set([
  "mpc-ready-808",
  "terminal-mirage",
  "boulevard-86",
]);

const TAG_STOPWORDS = new Set([
  "archetype",
  "trap",
  "rap",
  "type",
  "beats",
  "beat",
  "drums",
  "808",
  "phonk",
  "houston",
  "memphis",
  "atlanta",
  "west coast",
  "g funk",
  "rnb",
]);

function normalizePrompt(prompt: string): string {
  return prompt.toLowerCase().replace(/\s+/g, " ").trim();
}

function escapeRe(s: string): string {
  return s.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

function phraseMatches(prompt: string, phrase: string): boolean {
  const t = phrase.toLowerCase().trim();
  if (!t) return false;
  if (t.includes(" ")) return prompt.includes(t);
  const re = new RegExp(`\\b${escapeRe(t)}\\b`, "i");
  return re.test(prompt);
}

function tagMatches(prompt: string, tag: string): boolean {
  const t = tag.toLowerCase().trim();
  if (t.length < 3 || TAG_STOPWORDS.has(t)) return false;
  return phraseMatches(prompt, t);
}

function scoreLane(prompt: string, lane: SparkLaneDef): number {
  let best = 0;
  for (const tag of lane.tags) {
    if (!tagMatches(prompt, tag)) continue;
    const len = tag.toLowerCase().trim().length;
    best = Math.max(best, len);
  }
  const slug = lane.presetId.replace(/^lane-/, "").replace(/-/g, " ");
  if (phraseMatches(prompt, slug)) {
    best = Math.max(best, Math.max(12, slug.length));
  }
  const laneKey = lane.laneId.toLowerCase().replace(/-/g, " ");
  if (phraseMatches(prompt, laneKey)) {
    best = Math.max(best, Math.max(14, laneKey.length));
  }
  return best;
}

/**
 * Pick the best lane preset from natural-language prompt (longest tag win).
 * Returns undefined when no lane tag matches confidently.
 */
export function resolveSparkPresetIdFromPrompt(prompt: string): string | undefined {
  const p = normalizePrompt(prompt);
  if (!p) return undefined;

  for (const [alias, target] of Object.entries(SPARK_PRESET_ALIASES)) {
    const key = alias.replace(/^lane-/, "").replace(/-/g, " ");
    if (p.includes(alias) || p.includes(key)) {
      return resolveSparkPresetId(target);
    }
  }

  let winner: SparkLaneDef | undefined;
  let bestScore = 0;
  for (const lane of SPARK_LANE_REGISTRY) {
    const score = scoreLane(p, lane);
    if (score > bestScore) {
      bestScore = score;
      winner = lane;
    }
  }

  if (!winner || bestScore < 4) return undefined;
  return winner.presetId;
}

/** Use explicit lane preset, or infer from prompt when preset is generic/neutral. */
export function coalesceSparkPreset(prompt: string, presetId: string): string {
  const resolved = resolveSparkPresetId(presetId);
  if (!SPARK_GENERIC_PRESET_IDS.has(resolved)) return resolved;
  const inferred = resolveSparkPresetIdFromPrompt(prompt);
  return inferred ?? resolved;
}
