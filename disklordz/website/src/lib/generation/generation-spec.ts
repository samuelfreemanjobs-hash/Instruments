import { getPreset } from "@/lib/presets";

export type GenerationMode = "one_shot" | "loop" | "sfx";
export type GenerationEngine = "creative" | "studio";
export type LoopLength = "short" | "medium" | "long";

export type GenerationSpec = {
  mode: GenerationMode;
  engine: GenerationEngine;
  key: string;
  bpm: number;
  bars: number;
  length: LoopLength;
  stereo: number;
  wildness: number;
};

const MODES: GenerationMode[] = ["one_shot", "loop", "sfx"];
const ENGINES: GenerationEngine[] = ["creative", "studio"];
const LENGTHS: LoopLength[] = ["short", "medium", "long"];

function clamp(n: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, n));
}

/** Parse preset bpmHint like "118–124" or "any" into a default BPM. */
export function defaultBpmFromPreset(presetId: string): number {
  const preset = getPreset(presetId);
  const hint = preset?.bpmHint ?? "90";
  if (hint === "any") return 96;
  const nums = hint.match(/\d+/g);
  if (!nums?.length) return 96;
  const values = nums.map((n) => parseInt(n, 10));
  if (values.length === 1) return values[0];
  return Math.round((values[0] + values[1]) / 2);
}

export function defaultGenerationSpec(presetId: string): GenerationSpec {
  const preset = getPreset(presetId);
  const isSlow = preset?.artistLane === "DL006";
  return {
    mode: "one_shot",
    engine: "studio",
    key: isSlow ? "F minor" : "C minor",
    bpm: defaultBpmFromPreset(presetId),
    bars: 4,
    length: "medium",
    stereo: preset?.artistLane === "DL006" ? 0.35 : 0.55,
    wildness: preset?.artistLane === "DL002" ? 0.55 : 0.4,
  };
}

export type ParseSpecResult =
  | { ok: true; spec: GenerationSpec }
  | { ok: false; error: string };

function parseMode(v: unknown): GenerationMode | undefined {
  return typeof v === "string" && MODES.includes(v as GenerationMode)
    ? (v as GenerationMode)
    : undefined;
}

function parseEngine(v: unknown): GenerationEngine | undefined {
  return typeof v === "string" && ENGINES.includes(v as GenerationEngine)
    ? (v as GenerationEngine)
    : undefined;
}

function parseLength(v: unknown): LoopLength | undefined {
  return typeof v === "string" && LENGTHS.includes(v as LoopLength)
    ? (v as LoopLength)
    : undefined;
}

/** Merge API partial spec with preset defaults (WO-SAAS-007). */
export function parseGenerationSpec(
  partial: Record<string, unknown> | undefined,
  presetId: string,
): ParseSpecResult {
  const base = defaultGenerationSpec(presetId);

  const mode = parseMode(partial?.mode) ?? base.mode;
  const engine = parseEngine(partial?.engine) ?? base.engine;
  const length = parseLength(partial?.length) ?? base.length;

  let key = base.key;
  if (typeof partial?.key === "string") {
    const k = partial.key.trim();
    if (k.length > 32) {
      return { ok: false, error: "key_too_long" };
    }
    if (k.length > 0) key = k;
  }

  let bpm = base.bpm;
  if (partial?.bpm !== undefined && partial?.bpm !== null) {
    const n = Number(partial.bpm);
    if (!Number.isFinite(n)) return { ok: false, error: "invalid_bpm" };
    bpm = clamp(Math.round(n), 60, 200);
  }

  let bars = base.bars;
  if (partial?.bars !== undefined && partial?.bars !== null) {
    const n = Number(partial.bars);
    if (!Number.isFinite(n)) return { ok: false, error: "invalid_bars" };
    bars = clamp(Math.round(n), 1, 16);
  }

  let stereo = base.stereo;
  if (partial?.stereo !== undefined && partial?.stereo !== null) {
    const n = Number(partial.stereo);
    if (!Number.isFinite(n)) return { ok: false, error: "invalid_stereo" };
    stereo = clamp(n, 0, 1);
  }

  let wildness = base.wildness;
  if (partial?.wildness !== undefined && partial?.wildness !== null) {
    const n = Number(partial.wildness);
    if (!Number.isFinite(n)) return { ok: false, error: "invalid_wildness" };
    wildness = clamp(n, 0, 1);
  }

  return {
    ok: true,
    spec: { mode, engine, key, bpm, bars, length, stereo, wildness },
  };
}

/** How strongly prompt→param jitter applies (studio = tighter). */
export function wildnessJitterScale(spec: GenerationSpec): number {
  const engineScale = spec.engine === "creative" ? 1.25 : 0.85;
  return (0.5 + spec.wildness) * engineScale;
}

/** ILLUGEN-shaped: creative → 3 candidates, studio → 2. */
export function variationCountForEngine(engine: GenerationEngine): number {
  return engine === "creative" ? 3 : 2;
}

export function variationLabel(index: number): string {
  return String.fromCharCode(65 + index);
}
