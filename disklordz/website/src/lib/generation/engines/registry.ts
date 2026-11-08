import type { GenerationSpec } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import type { SampleName } from "@/lib/generation/synth";
import { renderSampleForEngine } from "@/lib/generation/engine-render";
import { fetchRemoteSamplePcm } from "@/lib/generation/engines/remote-wav";

export type EngineBackend = "parametric" | "remote";

export function activeEngineBackend(): EngineBackend {
  const mode = (process.env.DISKLORDZ_ENGINE ?? "parametric").toLowerCase();
  return mode === "remote" ? "remote" : "parametric";
}

export async function renderSampleViaRegistry(
  name: SampleName,
  params: DrumParams,
  spec: GenerationSpec,
  prompt: string,
): Promise<Float32Array> {
  if (activeEngineBackend() !== "remote") {
    return renderSampleForEngine(name, params, spec.engine);
  }
  const remote = await fetchRemoteSamplePcm(name, params, spec, prompt);
  if (remote) {
    return remote;
  }
  return renderSampleForEngine(name, params, spec.engine);
}
