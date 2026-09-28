import { randomUUID } from "crypto";

import { writeNamedSample } from "@/lib/generation/factory";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { PRODUCT_PACK_CREDIT_COST } from "@/lib/generation/mode-utils";
import type { VariationContext } from "@/lib/generation/prompt-params";
import { resolveDrumParams } from "@/lib/generation/prompt-params";
import type { SampleName } from "@/lib/generation/synth";
import { activeKitStorageBackend } from "@/lib/kit-storage";
import type { ProductPackManifest, ProductPackFolder } from "@/lib/manifest";
import { getPreset } from "@/lib/presets";

type FolderDef = {
  id: string;
  label: string;
  samples: { name: SampleName; count: number }[];
};

const PACK_FOLDERS: FolderDef[] = [
  {
    id: "01_KICKS",
    label: "Kicks",
    samples: [{ name: "kick", count: 4 }],
  },
  {
    id: "02_SNARES",
    label: "Snares",
    samples: [{ name: "snare", count: 4 }],
  },
  {
    id: "03_HATS",
    label: "Hats",
    samples: [
      { name: "hat_closed", count: 3 },
      { name: "hat_open", count: 2 },
    ],
  },
  {
    id: "04_PERC",
    label: "Perc",
    samples: [
      { name: "rim", count: 2 },
      { name: "clap", count: 2 },
    ],
  },
];

/** WO-SAAS-015: brief → folder tree of WAVs + storefront manifest. */
export async function buildProductPack(
  prompt: string,
  presetId: string,
  baseUrl: string,
  generationSpec: GenerationSpec,
  batchId?: string,
): Promise<ProductPackManifest> {
  const preset = getPreset(presetId);
  if (!preset) {
    throw new Error(`Unknown preset: ${presetId}`);
  }

  const packId = randomUUID();
  const samples: ProductPackManifest["samples"] = [];
  const folders: ProductPackFolder[] = [];
  let fileIndex = 0;

  for (const folder of PACK_FOLDERS) {
    let folderCount = 0;
    for (const slot of folder.samples) {
      for (let i = 0; i < slot.count; i++) {
        const variation: VariationContext = {
          variationIndex: fileIndex,
          batchId: batchId ?? packId,
        };
        const params = resolveDrumParams(prompt, presetId, generationSpec, variation);
        const filename = `${folder.id}/${slot.name}_${String(i + 1).padStart(2, "0")}.wav`;
        const asset = await writeNamedSample(
          packId,
          baseUrl,
          slot.name,
          filename,
          folder.id,
          params,
          generationSpec.engine,
          presetId,
          i + 1,
        );
        samples.push(asset);
        folderCount += 1;
        fileIndex += 1;
      }
    }
    folders.push({ id: folder.id, label: folder.label, sampleCount: folderCount });
  }

  return {
    format: "DISKLORDZ_PRODUCT_PACK_MANIFEST",
    version: 1,
    packId,
    createdAt: new Date().toISOString(),
    prompt: prompt.trim(),
    presetId,
    artistLane: preset.artistLane,
    sampleRate: 44100,
    folders,
    samples,
    license: "personal_and_commercial_v0_preview",
    storageBackend: activeKitStorageBackend(),
    generationSpec,
    creditCost: PRODUCT_PACK_CREDIT_COST,
  };
}
