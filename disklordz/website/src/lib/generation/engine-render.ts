import type { GenerationEngine } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import type { SampleName } from "@/lib/generation/synth";
import { renderSample } from "@/lib/generation/synth";
import type { SampleProvenance } from "@/lib/manifest";
import { softClip } from "@/lib/generation/dsp-core";

/** Prompt-derived grit 0..1 for post-process (memphis/screw/phonk). */
export function gritFromParams(params: DrumParams, prompt: string): number {
  const p = prompt.toLowerCase();
  let g = 0.12;
  if (/\b(screw|tape|dusty|lo-?fi|vinyl)\b/.test(p)) g += 0.35;
  if (/\b(phonk|memphis|distort|grit|dirty)\b/.test(p)) g += 0.3;
  if (/\b(clean|cyber|digital)\b/.test(p)) g -= 0.08;
  return Math.min(1, Math.max(0, g + (params.seed % 17) * 0.004));
}

/** Raw sample before master bus (WO-SAAS-017). */
export function renderSampleForEngine(
  name: SampleName,
  params: DrumParams,
  engine: GenerationEngine,
): Float32Array {
  const base = renderSample(name, params);
  if (engine === "studio") {
    const out = new Float32Array(base.length);
    for (let i = 0; i < base.length; i++) {
      out[i] = softClip(base[i], 1.08);
    }
    return out;
  }

  const out = new Float32Array(base.length);
  const detune = 1 + (params.seed % 7) * 0.0015;
  for (let i = 0; i < base.length; i++) {
    const src = base[Math.min(base.length - 1, Math.floor(i * detune))];
    out[i] = softClip(src * 1.05, 1.25);
  }
  return out;
}

export function provenanceForEngine(engine: GenerationEngine): SampleProvenance {
  return engine === "creative" ? "factory_creative_v2" : "factory_studio_v2";
}
