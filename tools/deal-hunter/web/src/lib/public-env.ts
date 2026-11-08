/** Public runtime config: bound secrets (server) or NEXT_PUBLIC_* (local). */
export function publicRuntimeConfig() {
  return {
    supabaseUrl: (process.env.SUPABASE_URL ?? process.env.NEXT_PUBLIC_SUPABASE_URL ?? "").trim(),
    supabaseAnonKey: (process.env.SUPABASE_ANON_KEY ?? process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY ?? "").trim(),
    useSupabaseCatalog:
      (process.env.USE_SUPABASE_CATALOG ?? process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG ?? "") === "1",
    vapidPublicKey: (process.env.VAPID_PUBLIC_KEY ?? process.env.NEXT_PUBLIC_VAPID_PUBLIC_KEY ?? "").trim(),
  };
}

export function isSupabaseConfiguredFromEnv(): boolean {
  const c = publicRuntimeConfig();
  return Boolean(c.supabaseUrl && c.supabaseAnonKey);
}
