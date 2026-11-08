import { isSupabaseConfiguredFromEnv, publicRuntimeConfig } from "@/lib/public-env";

export function isSupabaseConfigured(): boolean {
  return isSupabaseConfiguredFromEnv();
}

export function supabasePublicConfig() {
  const c = publicRuntimeConfig();
  return { url: c.supabaseUrl, anonKey: c.supabaseAnonKey };
}
