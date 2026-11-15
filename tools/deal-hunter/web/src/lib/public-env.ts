function catalogFlagEnabled(): boolean {
  const raw = (process.env.USE_SUPABASE_CATALOG ?? process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG ?? "").trim();
  if (raw === "1") return true;
  if (raw === "0") return false;
  const url = (process.env.SUPABASE_URL ?? process.env.NEXT_PUBLIC_SUPABASE_URL ?? "").trim();
  const key = (process.env.SUPABASE_ANON_KEY ?? process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY ?? "").trim();
  return Boolean(url && key);
}

/** Public runtime config: bound secrets (server) or NEXT_PUBLIC_* (local). */
export function publicRuntimeConfig() {
  return {
    supabaseUrl: (process.env.SUPABASE_URL ?? process.env.NEXT_PUBLIC_SUPABASE_URL ?? "").trim(),
    supabaseAnonKey: (process.env.SUPABASE_ANON_KEY ?? process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY ?? "").trim(),
    useSupabaseCatalog: catalogFlagEnabled(),
    vapidPublicKey: (process.env.VAPID_PUBLIC_KEY ?? process.env.NEXT_PUBLIC_VAPID_PUBLIC_KEY ?? "").trim(),
  };
}

export function isSupabaseConfiguredFromEnv(): boolean {
  const c = publicRuntimeConfig();
  return Boolean(c.supabaseUrl && c.supabaseAnonKey);
}
