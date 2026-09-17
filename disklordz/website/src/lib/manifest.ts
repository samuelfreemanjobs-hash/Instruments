import type { GenerationSpec } from "@/lib/generation/generation-spec";

export type SampleAsset = {
  name: string;
  filename: string;
  url: string;
  sha256: string;
  provenance: "stub_factory_v0" | "factory_parametric_v1";
  sourceId: string;
};

export type KitManifest = {
  format: "DISKLORDZ_DRUM_KIT_MANIFEST";
  version: 1;
  kitId: string;
  createdAt: string;
  prompt: string;
  presetId: string;
  artistLane: string;
  sampleRate: 44100;
  samples: SampleAsset[];
  license: "personal_and_commercial_v0_preview";
  /** WO-SAAS-009: where WAV bytes are persisted */
  storageBackend?: "local" | "supabase";
  /** Internal generation metadata (optional in API responses). */
  factoryParams?: Record<string, number>;
  /** Resolved musical + mode spec (WO-SAAS-007). */
  generationSpec?: GenerationSpec;
  /** WO-SAAS-008 variation batch metadata. */
  variationIndex?: number;
  variationBatchId?: string;
  variationTotal?: number;
  variationLabel?: string;
};
