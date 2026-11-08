import { createBrowserClient } from "@supabase/ssr";

import { isSupabaseConfigured, supabasePublicConfig } from "./config";

export function createClient() {
  if (!isSupabaseConfigured()) {
    return null;
  }
  const { url, anonKey } = supabasePublicConfig();
  return createBrowserClient(url, anonKey);
}
