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
  /** Internal generation metadata (optional in API responses). */
  factoryParams?: Record<string, number>;
};
