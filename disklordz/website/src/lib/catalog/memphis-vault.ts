import { readFileSync } from "node:fs";
import path from "node:path";

export const MEMPHIS_VAULT_SKU = "memphis-vault-vol-1";

export const MEMPHIS_VAULT_PREVIEWS = [
  "01_Kicks/DLZ_MEM_KICK_001_C2.wav",
  "02_808s/DLZ_MEM_808_001_F2.wav",
  "03_Snares/DLZ_MEM_SNARE_001.wav",
  "06_Cowbells/DLZ_MEM_COWBELL_001_F4.wav",
] as const;

export type VaultPreview = {
  file: string;
  label: string;
  seconds: number;
  key: string | null;
};

export type VaultFolder = {
  name: string;
  count: number;
};

export type MemphisVault = {
  product: string;
  sku: string;
  tagline: string;
  origin: string;
  sampleRate: number;
  bitDepth: number;
  channels: number;
  count: number;
  license: string;
  folders: VaultFolder[];
  previews: VaultPreview[];
};

type ManifestSound = {
  file: string;
  seconds: number;
  key: string | null;
};

type Manifest = {
  product: string;
  sku: string;
  tagline: string;
  origin: string;
  sampleRate: number;
  bitDepth: number;
  channels: number;
  count: number;
  license: string;
  sounds: ManifestSound[];
};

export function memphisVaultDir(): string {
  return path.resolve(process.cwd(), "../sound-factory/kits/memphis-vault-vol-1");
}

export function readMemphisVault(): MemphisVault {
  const raw = readFileSync(path.join(memphisVaultDir(), "manifest.json"), "utf8");
  const manifest = JSON.parse(raw) as Manifest;
  const folders = new Map<string, number>();
  for (const sound of manifest.sounds) {
    const folder = sound.file.split("/")[0] ?? "sounds";
    folders.set(folder, (folders.get(folder) ?? 0) + 1);
  }
  const byFile = new Map(manifest.sounds.map((sound) => [sound.file, sound]));
  const previews = MEMPHIS_VAULT_PREVIEWS.flatMap((file) => {
    const sound = byFile.get(file);
    if (!sound) return [];
    return [
      {
        file,
        label: path.basename(file),
        seconds: sound.seconds,
        key: sound.key,
      },
    ];
  });
  return {
    product: manifest.product,
    sku: manifest.sku,
    tagline: manifest.tagline,
    origin: manifest.origin,
    sampleRate: manifest.sampleRate,
    bitDepth: manifest.bitDepth,
    channels: manifest.channels,
    count: manifest.count,
    license: manifest.license,
    folders: [...folders.entries()].map(([name, count]) => ({ name, count })),
    previews,
  };
}
