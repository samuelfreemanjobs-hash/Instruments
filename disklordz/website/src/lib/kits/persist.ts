import type { SupabaseClient } from "@supabase/supabase-js";

import type { KitManifest } from "@/lib/manifest";

export async function saveKitForUser(
  supabase: SupabaseClient,
  userId: string,
  manifest: KitManifest,
): Promise<{ ok: true } | { ok: false; error: string }> {
  const { error } = await supabase.from("saved_kits").upsert(
    {
      user_id: userId,
      kit_id: manifest.kitId,
      prompt: manifest.prompt,
      preset_id: manifest.presetId,
      artist_lane: manifest.artistLane,
      manifest,
    },
    { onConflict: "user_id,kit_id" },
  );

  if (error) {
    return { ok: false, error: error.message };
  }
  return { ok: true };
}
