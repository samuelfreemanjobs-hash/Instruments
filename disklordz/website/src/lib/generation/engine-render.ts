import type { GenerationEngine } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import type { SampleName } from "@/lib/generation/synth";
import { renderSample } from "@/lib/generation/synth";
import type { SampleProvenance } from "@/lib/manifest";

function saturate(x: number, drive: number): number {
  return Math.tanh(x * drive) / Math.tanh(drive);
}

/** WO-SAAS-013: distinct studio vs creative render paths. */
export function renderSampleForEngine(
  name: SampleName,
  params: DrumParams,
  engine: GenerationEngine,
): Float32Array {
  const base = renderSample(name, params);
  if (engine === "studio") {
    const out = new Float32Array(base.length);
    for (let i = 0; i < base.length; i++) {
      out[i] = saturate(base[i], 1.15);
    }
    return out;
  }

  const out = new Float32Array(base.length);
  const detune = 1 + (params.seed % 7) * 0.002;
  for (let i = 0; i < base.length; i++) {
    const src = base[Math.min(base.length - 1, Math.floor(i * detune))];
    const n = Math.sin(i * 0.37 + params.seed) * 0.04;
    out[i] = saturate(src * 1.08 + n, 1.45);
  }
  return out;
}

export function provenanceForEngine(engine: GenerationEngine): SampleProvenance {
  return engine === "creative" ? "factory_creative_v1" : "factory_studio_v1";
}
