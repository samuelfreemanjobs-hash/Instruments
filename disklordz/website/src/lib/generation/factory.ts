import { randomUUID } from "crypto";

import {
  provenanceForEngine,
  renderSampleForEngine,
} from "@/lib/generation/engine-render";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { renderDrumLoop } from "@/lib/generation/loop-render";
import { renderSfx } from "@/lib/generation/sfx-render";
import type { VariationContext } from "@/lib/generation/prompt-params";
import { resolveDrumParams } from "@/lib/generation/prompt-params";
import { SAMPLE_NAMES, type SampleName } from "@/lib/generation/synth";
import { encodeWav } from "@/lib/generation/wav";
import { activeKitStorageBackend, writeKitFile } from "@/lib/kit-storage";
import { sha256Buffer } from "@/lib/kit-store";
import type { KitManifest, SampleAsset } from "@/lib/manifest";
import { getPreset } from "@/lib/presets";

async function writeSampleAsset(
  kitId: string,
  baseUrl: string,
  asset: Omit<SampleAsset, "url" | "sha256"> & { pcm: Float32Array },
): Promise<SampleAsset> {
  const wav = encodeWav(asset.pcm);
  await writeKitFile(kitId, asset.filename, wav);
  const sha256 = sha256Buffer(wav);
  return {
    name: asset.name,
    filename: asset.filename,
    folder: asset.folder,
    provenance: asset.provenance,
    sourceId: asset.sourceId,
    sha256,
    url: `${baseUrl}/api/samples/${kitId}/${asset.filename.split("/").map(encodeURIComponent).join("/")}`,
  };
}

export async function buildFactoryKit(
  prompt: string,
  presetId: string,
  baseUrl: string,
  generationSpec: GenerationSpec,
  variation?: VariationContext,
): Promise<KitManifest> {
  const preset = getPreset(presetId);
  if (!preset) {
    throw new Error(`Unknown preset: ${presetId}`);
  }

  const params = resolveDrumParams(prompt, presetId, generationSpec, variation);
  const kitId = randomUUID();
  const provenance = provenanceForEngine(generationSpec.engine);
  const samples: SampleAsset[] = [];

  if (generationSpec.mode === "loop") {
    const pcm = renderDrumLoop(params, generationSpec, generationSpec.engine);
    samples.push(
      await writeSampleAsset(kitId, baseUrl, {
        name: "loop_main",
        filename: "loop_main.wav",
        provenance,
        sourceId: `factory://${presetId}/loop?seed=${params.seed}&bpm=${generationSpec.bpm}`,
        pcm,
      }),
    );
  } else if (generationSpec.mode === "sfx") {
    const pcm = renderSfx(params, generationSpec, generationSpec.engine);
    samples.push(
      await writeSampleAsset(kitId, baseUrl, {
        name: "sfx",
        filename: "sfx.wav",
        provenance,
        sourceId: `factory://${presetId}/sfx?seed=${params.seed}&len=${generationSpec.length}`,
        pcm,
      }),
    );
  } else {
    for (const name of SAMPLE_NAMES) {
      const pcm = renderSampleForEngine(name, params, generationSpec.engine);
      const filename = `${name}.wav`;
      samples.push(
        await writeSampleAsset(kitId, baseUrl, {
          name,
          filename,
          provenance,
          sourceId: `factory://${presetId}/${name}?seed=${params.seed}&engine=${generationSpec.engine}`,
          pcm,
        }),
      );
    }
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
    storageBackend: activeKitStorageBackend(),
    factoryParams: {
      seed: params.seed,
      kickPitch: params.kickPitch,
      kickDecay: params.kickDecay,
    },
    generationSpec,
  };
}

/** Re-export for product factory batch renders. */
export async function writeNamedSample(
  packId: string,
  baseUrl: string,
  name: SampleName,
  filename: string,
  folder: string,
  params: ReturnType<typeof resolveDrumParams>,
  engine: GenerationSpec["engine"],
  presetId: string,
  index: number,
): Promise<SampleAsset> {
  const pcm = renderSampleForEngine(name, params, engine);
  return writeSampleAsset(packId, baseUrl, {
    name: `${name}_${index}`,
    filename,
    folder,
    provenance: "factory_product_pack_v1",
    sourceId: `factory://pack/${presetId}/${name}/${index}?seed=${params.seed}`,
    pcm,
  });
}
