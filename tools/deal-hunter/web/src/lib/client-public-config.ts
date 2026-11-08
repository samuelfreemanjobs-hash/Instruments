export type ClientPublicConfig = {
  supabaseUrl: string;
  supabaseAnonKey: string;
  useSupabaseCatalog: boolean;
  vapidPublicKey: string;
};

export function readClientPublicConfig(): ClientPublicConfig {
  if (typeof document !== "undefined") {
    const el = document.getElementById("bfm-public-config");
    if (el?.textContent) {
      try {
        return JSON.parse(el.textContent) as ClientPublicConfig;
      } catch {
        /* fall through */
      }
    }
  }
  return {
    supabaseUrl: process.env.NEXT_PUBLIC_SUPABASE_URL ?? "",
    supabaseAnonKey: process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY ?? "",
    useSupabaseCatalog: process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG === "1",
    vapidPublicKey: process.env.NEXT_PUBLIC_VAPID_PUBLIC_KEY ?? "",
  };
}
