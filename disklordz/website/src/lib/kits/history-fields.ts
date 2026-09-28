import type { KitManifest } from "@/lib/manifest";

export type SavedKitHistoryFields = {
  spec_mode: string | null;
  spec_engine: string | null;
  spec_key: string | null;
  spec_bpm: number | null;
  variation_batch_id: string | null;
  variation_label: string | null;
};

export function historyFieldsFromManifest(manifest: KitManifest): SavedKitHistoryFields {
  const spec = manifest.generationSpec;
  return {
    spec_mode: spec?.mode ?? null,
    spec_engine: spec?.engine ?? null,
    spec_key: spec?.key ?? null,
    spec_bpm: spec?.bpm ?? null,
    variation_batch_id: manifest.variationBatchId ?? null,
    variation_label: manifest.variationLabel ?? null,
  };
}
