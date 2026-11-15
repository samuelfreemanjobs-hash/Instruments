import { createBrowserClient } from "@supabase/ssr";

import { readClientPublicConfig } from "@/lib/client-public-config";

export function createClient() {
  const { supabaseUrl, supabaseAnonKey } = readClientPublicConfig();
  if (!supabaseUrl || !supabaseAnonKey) {
    return null;
  }
  return createBrowserClient(supabaseUrl, supabaseAnonKey);
}
