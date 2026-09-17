import type { GenerationEngine } from "@/lib/generation/generation-spec";
import type { GenerationSpec, LoopLength } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import { renderSampleForEngine } from "@/lib/generation/engine-render";

const SAMPLE_RATE = 44100;

function secondsForSfxLength(length: LoopLength): number {
  switch (length) {
    case "short":
      return 0.45;
    case "medium":
      return 1.2;
    case "long":
      return 2.8;
    default:
      return 0.45;
  }
}

/** WO-SAAS-014: layered impact SFX with tiered duration. */
export function renderSfx(
  params: DrumParams,
  spec: GenerationSpec,
  engine: GenerationEngine,
): Float32Array {
  const seconds = secondsForSfxLength(spec.length);
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);

  const kick = renderSampleForEngine("kick", params, engine);
  const rim = renderSampleForEngine("rim", params, engine);
  const clap = renderSampleForEngine("clap", params, engine);

  for (let i = 0; i < n; i++) {
    const t = i / SAMPLE_RATE;
    const env = Math.exp(-t / (seconds * 0.55));
    let s = 0;
    if (i < kick.length) s += kick[i] * 0.7;
    if (i < rim.length) s += rim[i] * 0.5;
    if (i < clap.length) s += clap[i] * 0.35;
    out[i] = s * env;
  }

  let peak = 0;
  for (const v of out) peak = Math.max(peak, Math.abs(v));
  const scale = peak > 0 ? 0.89 / peak : 1;
  for (let i = 0; i < out.length; i++) out[i] *= scale;

  return out;
}
