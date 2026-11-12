import { isSupabaseConfiguredFromEnv, publicRuntimeConfig } from "@/lib/public-env";

/** Shown when auth secrets exist but the synced catalog flag is still off. */
export function CatalogModeNote() {
  if (!isSupabaseConfiguredFromEnv()) return null;
  if (publicRuntimeConfig().useSupabaseCatalog) return null;
  return (
    <p className="border-b border-amber-500/30 bg-amber-500/10 px-4 py-2 text-center text-sm text-amber-100">
      Bundled demo catalog is active. In Publish Secrets set <strong>USE_SUPABASE_CATALOG=1</strong> after your Supabase
      catalog is synced, then republish for live deals.
    </p>
  );
}
