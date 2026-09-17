import { randomUUID } from "crypto";

import { buildFactoryKit } from "@/lib/generation/factory";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import {
  variationCountForEngine,
  variationLabel,
} from "@/lib/generation/generation-spec";
import type { KitManifest } from "@/lib/manifest";

export async function buildVariationBatch(
  prompt: string,
  presetId: string,
  baseUrl: string,
  generationSpec: GenerationSpec,
  batchId: string = randomUUID(),
): Promise<{ batchId: string; manifests: KitManifest[] }> {
  const count = variationCountForEngine(generationSpec.engine);
  const manifests: KitManifest[] = [];

  for (let i = 0; i < count; i++) {
    const manifest = await buildFactoryKit(
      prompt,
      presetId,
      baseUrl,
      generationSpec,
      { variationIndex: i, batchId },
    );
    manifests.push({
      ...manifest,
      variationIndex: i,
      variationBatchId: batchId,
      variationTotal: count,
      variationLabel: variationLabel(i),
    });
  }

  return { batchId, manifests };
}
