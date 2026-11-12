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
  const raw = process.env.NEXT_PUBLIC_USE_SUPABASE_CATALOG ?? "";
  const url = process.env.NEXT_PUBLIC_SUPABASE_URL ?? "";
  const key = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY ?? "";
  return {
    supabaseUrl: url,
    supabaseAnonKey: key,
    useSupabaseCatalog: raw === "1" || (raw !== "0" && Boolean(url && key)),
    vapidPublicKey: process.env.NEXT_PUBLIC_VAPID_PUBLIC_KEY ?? "",
  };
}
