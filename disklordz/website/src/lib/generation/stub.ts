import { createHash, randomUUID } from "crypto";
import { readFile } from "fs/promises";
import path from "path";

import type { KitManifest, SampleAsset } from "@/lib/manifest";
import { getPreset } from "@/lib/presets";

const SAMPLE_NAMES = [
  "kick",
  "snare",
  "hat_closed",
  "hat_open",
  "rim",
  "clap",
] as const;

async function sha256File(filePath: string): Promise<string> {
  const buf = await readFile(filePath);
  return createHash("sha256").update(buf).digest("hex");
}

export async function buildStubKit(
  prompt: string,
  presetId: string,
  baseUrl: string,
): Promise<KitManifest> {
  const preset = getPreset(presetId);
  if (!preset) {
    throw new Error(`Unknown preset: ${presetId}`);
  }

  const kitId = randomUUID();
  const samplesDir = path.join(process.cwd(), "public", "samples", presetId);
  const samples: SampleAsset[] = [];

  for (const name of SAMPLE_NAMES) {
    const filename = `${name}.wav`;
    const filePath = path.join(samplesDir, filename);
    const sha256 = await sha256File(filePath);
    samples.push({
      name,
      filename,
      url: `${baseUrl}/samples/${presetId}/${filename}`,
      sha256,
      provenance: "stub_factory_v0",
      sourceId: `stub://${presetId}/${name}`,
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
  };
}
