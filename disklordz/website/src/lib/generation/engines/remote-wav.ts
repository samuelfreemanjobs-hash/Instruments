import type { GenerationSpec } from "@/lib/generation/generation-spec";
import type { DrumParams } from "@/lib/generation/prompt-params";
import type { SampleName } from "@/lib/generation/synth";

function engineUrlFor(spec: GenerationSpec): string | null {
  if (spec.engine === "creative") {
    return process.env.DISKLORDZ_AUDIOCRAFT_ENGINE_URL ?? null;
  }
  return (
    process.env.DISKLORDZ_STABLE_AUDIO_ENGINE_URL ??
    process.env.DISKLORDZ_AUDIOCRAFT_ENGINE_URL ??
    null
  );
}

/** POST JSON to optional AudioCraft / Stable Audio / Cog worker; expect base64 PCM or WAV bytes. */
export async function fetchRemoteSamplePcm(
  name: SampleName,
  params: DrumParams,
  spec: GenerationSpec,
  prompt: string,
): Promise<Float32Array | null> {
  const url = engineUrlFor(spec);
  if (!url) {
    return null;
  }
  try {
    const res = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        sample: name,
        prompt,
        spec,
        seed: params.seed,
        provider: spec.engine === "creative" ? "audiocraft" : "stable-audio-tools",
      }),
      signal: AbortSignal.timeout(120_000),
    });
    if (!res.ok) {
      return null;
    }
    const json = (await res.json()) as { pcm?: number[]; sampleRate?: number };
    if (!json.pcm?.length) {
      return null;
    }
    return Float32Array.from(json.pcm);
  } catch {
    return null;
  }
}
