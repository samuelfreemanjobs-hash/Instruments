import { createClient, type SupabaseClient } from "@supabase/supabase-js";

let admin: SupabaseClient | null | undefined;

/** Server-only Supabase client (service role). Never expose to the browser. */
export function getSupabaseAdmin(): SupabaseClient | null {
  if (admin !== undefined) {
    return admin;
  }
  const url = process.env.NEXT_PUBLIC_SUPABASE_URL;
  const key = process.env.SUPABASE_SERVICE_ROLE_KEY;
  if (!url || !key) {
    admin = null;
    return admin;
  }
  admin = createClient(url, key, {
    auth: { persistSession: false, autoRefreshToken: false },
  });
  return admin;
}

export function isPersistentKitStorageEnabled(): boolean {
  return getSupabaseAdmin() !== null;
}

export const KITS_BUCKET = "disklordz-kits";
