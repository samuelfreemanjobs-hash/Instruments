import { randomUUID } from "crypto";

import { sha256Buffer, writeKitFile } from "@/lib/kit-store";
import type { KitManifest, SampleAsset } from "@/lib/manifest";
import { resolveDrumParams } from "@/lib/generation/prompt-params";
import { SAMPLE_NAMES, renderSample } from "@/lib/generation/synth";
import { encodeWav } from "@/lib/generation/wav";
import { getPreset } from "@/lib/presets";

export async function buildFactoryKit(
  prompt: string,
  presetId: string,
  baseUrl: string,
): Promise<KitManifest> {
  const preset = getPreset(presetId);
  if (!preset) {
    throw new Error(`Unknown preset: ${presetId}`);
  }

  const params = resolveDrumParams(prompt, presetId);
  const kitId = randomUUID();
  const samples: SampleAsset[] = [];

  for (const name of SAMPLE_NAMES) {
    const pcm = renderSample(name, params);
    const wav = encodeWav(pcm);
    const filename = `${name}.wav`;
    await writeKitFile(kitId, filename, wav);
    const sha256 = sha256Buffer(wav);
    samples.push({
      name,
      filename,
      url: `${baseUrl}/api/samples/${kitId}/${filename}`,
      sha256,
      provenance: "factory_parametric_v1",
      sourceId: `factory://${presetId}/${name}?seed=${params.seed}`,
    });
  }

  return {
    format: "DISKLORDZ_DRUM_KIT_MANIFEST",
    version: 1,
    kitId,
    createdAt: new Date().toISOString(),
    prompt: prompt.trim(),
    presetId,
    artistLane: preset.artistLane,
    sampleRate: 44100,
    samples,
    license: "personal_and_commercial_v0_preview",
    factoryParams: {
      seed: params.seed,
      kickPitch: params.kickPitch,
      kickDecay: params.kickDecay,
    },
  };
}
