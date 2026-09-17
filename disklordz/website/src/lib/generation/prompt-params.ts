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
};

function hashSeed(prompt: string, presetId: string): number {
  const hex = createHash("sha256").update(`${presetId}::${prompt}`).digest("hex");
  return parseInt(hex.slice(0, 8), 16);
}

function clamp(n: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, n));
}

/** Map natural-language prompt tokens to parameter nudges (deterministic + seeded jitter). */
export function resolveDrumParams(
  prompt: string,
  presetId: string,
  spec?: GenerationSpec,
): DrumParams {
  const base = PRESET_BASE[presetId] ?? PRESET_BASE["mpc-ready-808"];
  const seed = hashSeed(
    spec
      ? `${presetId}::${prompt}::${spec.bpm}::${spec.key}::${spec.wildness}`
      : prompt,
    presetId,
  );
  const jitterScale = spec ? wildnessJitterScale(spec) : 1;
  const jitter = (i: number) =>
    (((seed >> (i * 4)) & 0xf) / 0xf - 0.5) * jitterScale;

  const p = prompt.toLowerCase();
  let kickPitch = base.kickPitch ?? 45;
  let kickDecay = base.kickDecay ?? 0.28;
  let snareSnap = base.snareSnap ?? 0.35;
  let hatBright = base.hatBright ?? 0.4;
  let clapWide = base.clapWide ?? 0.4;

  if (/\b(808|sub|deep|low)\b/.test(p)) kickPitch -= 6;
  if (/\b(tight|punch|knock)\b/.test(p)) kickDecay *= 0.85;
  if (/\b(screw|slow|tape|dusty|lo-?fi)\b/.test(p)) {
    kickDecay *= 1.25;
    snareSnap *= 0.7;
    hatBright *= 0.75;
  }
  if (/\b(dirty|grit|memphis|phonk|distort)\b/.test(p)) snareSnap *= 1.2;
  if (/\b(clean|cyber|digital|crisp)\b/.test(p)) {
    snareSnap *= 0.9;
    hatBright *= 1.1;
  }
  if (/\b(bright|open|air)\b/.test(p)) hatBright *= 1.15;

  const preset = getPreset(presetId);
  if (preset?.artistLane === "DL002") snareSnap *= 1.05;
  if (preset?.artistLane === "DL006") kickDecay *= 1.1;

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
    snareBody: clamp((base.snareBody ?? 0.3) * (1 + jitter(2) * 0.1), 0.1, 0.7),
    snareSnap: clamp(snareSnap * (1 + jitter(3) * 0.1), 0.08, 0.75),
    hatDecay: clamp((base.hatDecay ?? 0.025) * (1 + jitter(4) * 0.1), 0.01, 0.1),
    hatBright: clamp(hatBright * (1 + jitter(5) * 0.1), 0.1, 0.75),
    rimMetal: clamp((base.rimMetal ?? 0.5) * (1 + jitter(6) * 0.1), 0.2, 0.85),
    clapWide: clamp(clapWide * (1 + jitter(7) * 0.1), 0.15, 0.65),
    seed,
  };
}
