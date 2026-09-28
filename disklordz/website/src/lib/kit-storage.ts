import {
  getSupabaseAdmin,
  isPersistentKitStorageEnabled,
  KITS_BUCKET,
} from "@/lib/supabase/admin";
import { kitDirectory, readKitFile as readLocalKitFile, writeKitFile as writeLocalKitFile } from "@/lib/kit-store";

function objectPath(kitId: string, filename: string): string {
  return `${kitId}/${filename}`;
}

export type KitStorageBackend = "local" | "supabase";

export function activeKitStorageBackend(): KitStorageBackend {
  return isPersistentKitStorageEnabled() ? "supabase" : "local";
}

/** Write to local temp (always) and mirror to Supabase Storage when configured. */
export async function writeKitFile(
  kitId: string,
  filename: string,
  data: Buffer,
): Promise<void> {
  await writeLocalKitFile(kitId, filename, data);

  const admin = getSupabaseAdmin();
  if (!admin) {
    return;
  }

  const { error } = await admin.storage.from(KITS_BUCKET).upload(objectPath(kitId, filename), data, {
    contentType: "audio/wav",
    upsert: true,
  });

  if (error) {
    throw new Error(`kit_storage_upload_failed: ${error.message}`);
  }
}

/** Read kit WAV: local cache first, then Supabase Storage. */
export async function readKitFile(kitId: string, filename: string): Promise<Buffer> {
  try {
    return await readLocalKitFile(kitId, filename);
  } catch {
    const admin = getSupabaseAdmin();
    if (!admin) {
      throw new Error("not_found");
    }

    const { data, error } = await admin.storage
      .from(KITS_BUCKET)
      .download(objectPath(kitId, filename));

    if (error || !data) {
      throw new Error("not_found");
    }

    const buf = Buffer.from(await data.arrayBuffer());
    await writeLocalKitFile(kitId, filename, buf).catch(() => undefined);
    return buf;
  }
}

export async function ensureKitStorageReady(): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return;
  }

  const { data: buckets } = await admin.storage.listBuckets();
  const exists = buckets?.some((b) => b.name === KITS_BUCKET);
  if (exists) {
    return;
  }

  const { error } = await admin.storage.createBucket(KITS_BUCKET, {
    public: false,
    fileSizeLimit: 5 * 1024 * 1024,
  });
  if (error && !error.message.toLowerCase().includes("already")) {
    throw new Error(`kit_storage_bucket_failed: ${error.message}`);
  }
}

export { kitDirectory };
