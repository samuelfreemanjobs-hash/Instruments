import { createHash } from "crypto";

import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { wildnessJitterScale } from "@/lib/generation/generation-spec";
import { getPreset } from "@/lib/presets";

export type DrumParams = {
  kickPitch: number;
  kickDecay: number;
  snareBody: number;
  snareSnap: number;
  hatDecay: number;
  hatBright: number;
  rimMetal: number;
  clapWide: number;
  seed: number;
};

const PRESET_BASE: Record<string, Partial<DrumParams>> = {
  "boulevard-86": {
    kickPitch: 52,
    kickDecay: 0.22,
    snareBody: 0.35,
    snareSnap: 0.25,
    hatDecay: 0.025,
    hatBright: 0.35,
    rimMetal: 0.45,
    clapWide: 0.35,
  },
  "midnight-circuit": {
    kickPitch: 38,
    kickDecay: 0.35,
    snareBody: 0.2,
    snareSnap: 0.55,
    hatDecay: 0.018,
    hatBright: 0.55,
    rimMetal: 0.65,
    clapWide: 0.5,
  },
  "disklordz-screw": {
    kickPitch: 34,
    kickDecay: 0.55,
    snareBody: 0.55,
    snareSnap: 0.15,
    hatDecay: 0.04,
    hatBright: 0.2,
    rimMetal: 0.3,
    clapWide: 0.25,
  },
  "terminal-mirage": {
    kickPitch: 58,
    kickDecay: 0.18,
    snareBody: 0.3,
    snareSnap: 0.35,
    hatDecay: 0.015,
    hatBright: 0.45,
    rimMetal: 0.55,
    clapWide: 0.4,
  },
  "mpc-ready-808": {
    kickPitch: 45,
    kickDecay: 0.28,
    snareBody: 0.25,
    snareSnap: 0.4,
    hatDecay: 0.022,
    hatBright: 0.45,
    rimMetal: 0.5,
    clapWide: 0.42,
  },
  "lane-jeezy": {
    kickPitch: 39,
    kickDecay: 0.38,
    snareBody: 0.42,
    snareSnap: 0.38,
    hatDecay: 0.028,
    hatBright: 0.32,
    rimMetal: 0.4,
    clapWide: 0.38,
  },
  "lane-gucci-mane": {
    kickPitch: 42,
    kickDecay: 0.3,
    snareBody: 0.28,
    snareSnap: 0.48,
    hatDecay: 0.02,
    hatBright: 0.48,
    rimMetal: 0.55,
    clapWide: 0.52,
  },
  "lane-mike-will": {
    kickPitch: 44,
    kickDecay: 0.24,
    snareBody: 0.22,
    snareSnap: 0.52,
    hatDecay: 0.016,
    hatBright: 0.5,
    rimMetal: 0.45,
    clapWide: 0.44,
  },
  "lane-drumma-boy": {
    kickPitch: 46,
    kickDecay: 0.32,
    snareBody: 0.38,
    snareSnap: 0.5,
    hatDecay: 0.024,
    hatBright: 0.42,
    rimMetal: 0.62,
    clapWide: 0.48,
  },
  "lane-zaytoven": {
    kickPitch: 48,
    kickDecay: 0.26,
    snareBody: 0.26,
    snareSnap: 0.42,
    hatDecay: 0.012,
    hatBright: 0.46,
    rimMetal: 0.48,
    clapWide: 0.4,
  },
  "lane-ti": {
    kickPitch: 47,
    kickDecay: 0.29,
    snareBody: 0.32,
    snareSnap: 0.44,
    hatDecay: 0.02,
    hatBright: 0.52,
    rimMetal: 0.58,
    clapWide: 0.5,
  },
  "lane-dj-doomp": {
    kickPitch: 35,
    kickDecay: 0.48,
    snareBody: 0.45,
    snareSnap: 0.58,
    hatDecay: 0.035,
    hatBright: 0.22,
    rimMetal: 0.38,
    clapWide: 0.32,
  },
};

function hashSeed(prompt: string, presetId: string): number {
  const hex = createHash("sha256").update(`${presetId}::${prompt}`).digest("hex");
  return parseInt(hex.slice(0, 8), 16);
}

function clamp(n: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, n));
}

export type VariationContext = {
  variationIndex: number;
  batchId: string;
};

/** Map natural-language prompt tokens to parameter nudges (deterministic + seeded jitter). */
export function resolveDrumParams(
  prompt: string,
  presetId: string,
  spec?: GenerationSpec,
  variation?: VariationContext,
): DrumParams {
  const base = PRESET_BASE[presetId] ?? PRESET_BASE["mpc-ready-808"];
  let seedInput = spec
    ? `${presetId}::${prompt}::${spec.bpm}::${spec.key}::${spec.wildness}::${spec.engine}`
    : prompt;
  if (variation) {
    seedInput += `::batch=${variation.batchId}::v=${variation.variationIndex}`;
  }
  const seed = hashSeed(seedInput, presetId);
  const jitterScale = spec ? wildnessJitterScale(spec) : 1;
  const jitter = (i: number) =>
    (((seed >> (i * 4)) & 0xf) / 0xf - 0.5) * jitterScale;

  const p = prompt.toLowerCase();
  let kickPitch = base.kickPitch ?? 45;
  let kickDecay = base.kickDecay ?? 0.28;
  let snareBody = base.snareBody ?? 0.3;
  let snareSnap = base.snareSnap ?? 0.35;
  let hatBright = base.hatBright ?? 0.4;
  let clapWide = base.clapWide ?? 0.4;
  let hatDecay = base.hatDecay ?? 0.025;
  let rimMetal = base.rimMetal ?? 0.5;

  if (/\b(808|sub|deep|low)\b/.test(p)) kickPitch -= 6;
  if (/\b(tight|punch|knock)\b/.test(p)) kickDecay *= 0.85;
  if (/\b(screw|slow|tape|dusty|lo-?fi)\b/.test(p)) {
    kickDecay *= 1.25;
    snareSnap *= 0.7;
    hatBright *= 0.75;
  }
  if (/\b(dirty|grit|memphis|phonk|distort)\b/.test(p)) snareSnap *= 1.2;
  if (/\b(jeezy|snowman|anthem|march)\b/.test(p)) {
    kickDecay *= 1.1;
    snareBody *= 1.08;
  }
  if (/\b(gucci|bounce|quirky|atl)\b/.test(p)) clapWide *= 1.1;
  if (/\b(mike will|mike will made it|minimal hat|hard trap)\b/.test(p)) {
    hatDecay *= 0.9;
    snareSnap *= 1.08;
  }
  if (/\b(drumma|anthem|crack snare|orchestral)\b/.test(p)) {
    snareSnap *= 1.12;
    rimMetal *= 1.1;
  }
  if (/\b(zaytoven|flutter|shaker|atl trap)\b/.test(p)) {
    hatDecay *= 0.85;
    hatBright *= 1.05;
  }
  if (/\b(t\.?i\.?|king of the south|swagger)\b/.test(p)) {
    hatBright *= 1.08;
    clapWide *= 1.05;
  }
  if (/\b(doomp|doom|memphis crunch|strip)\b/.test(p)) {
    kickDecay *= 1.15;
    hatBright *= 0.8;
    snareSnap *= 1.1;
  }
  if (/\b(clean|cyber|digital|crisp)\b/.test(p)) {
    snareSnap *= 0.9;
    hatBright *= 1.1;
  }
  if (/\b(bright|open|air)\b/.test(p)) hatBright *= 1.15;

  const preset = getPreset(presetId);
  if (preset?.artistLane === "DL002") snareSnap *= 1.05;
  if (preset?.artistLane === "DL006") kickDecay *= 1.1;
  if (preset?.artistLane === "DL-JEEZY") kickDecay *= 1.05;
  if (preset?.artistLane === "DL-GUCCI") clapWide *= 1.06;
  if (preset?.artistLane === "DL-MIKEWILL") hatDecay *= 0.92;
  if (preset?.artistLane === "DL-DRUMMA") snareSnap *= 1.04;
  if (preset?.artistLane === "DL-ZAYTOVEN") hatDecay *= 0.88;
  if (preset?.artistLane === "DL-DOOMP") {
    kickDecay *= 1.08;
    hatBright *= 0.9;
  }

  if (spec) {
    const bpmFactor = spec.bpm / 120;
    kickDecay *= clamp(1 / Math.sqrt(bpmFactor), 0.85, 1.2);
    if (spec.stereo < 0.4) {
      clapWide = (base.clapWide ?? 0.4) * 0.85;
      hatBright *= 0.92;
    } else if (spec.stereo > 0.65) {
      clapWide = clamp((base.clapWide ?? 0.4) * 1.15, 0.15, 0.65);
    }
    if (/\bminor\b/i.test(spec.key)) hatBright *= 0.98;
  }

  return {
    kickPitch: clamp(kickPitch + jitter(0) * 4, 28, 72),
    kickDecay: clamp(kickDecay * (1 + jitter(1) * 0.08), 0.12, 0.7),
    snareBody: clamp(snareBody * (1 + jitter(2) * 0.1), 0.1, 0.7),
    snareSnap: clamp(snareSnap * (1 + jitter(3) * 0.1), 0.08, 0.75),
    hatDecay: clamp(hatDecay * (1 + jitter(4) * 0.1), 0.01, 0.1),
    hatBright: clamp(hatBright * (1 + jitter(5) * 0.1), 0.1, 0.75),
    rimMetal: clamp(rimMetal * (1 + jitter(6) * 0.1), 0.2, 0.85),
    clapWide: clamp(clapWide * (1 + jitter(7) * 0.1), 0.15, 0.65),
    seed,
  };
}
