import { isSupabaseConfiguredFromEnv, publicRuntimeConfig } from "@/lib/public-env";

/** Shown when Supabase is missing or catalog mode is explicitly off. */
export function CatalogModeNote() {
  if (!isSupabaseConfiguredFromEnv()) {
    return (
      <p className="border-b border-amber-500/30 bg-amber-500/10 px-4 py-2 text-center text-sm text-amber-100">
        Sign-in and live deals need <strong>Supabase</strong> secrets in Publish Settings, then run the GitHub go-live
        workflow to sync catalog.
      </p>
    );
  }
  const raw = (process.env.USE_SUPABASE_CATALOG ?? "").trim();
  if (raw === "0") {
    return (
      <p className="border-b border-amber-500/30 bg-amber-500/10 px-4 py-2 text-center text-sm text-amber-100">
        Bundled demo catalog is on (<strong>USE_SUPABASE_CATALOG=0</strong>). Set to <strong>1</strong> after catalog
        sync, then republish.
      </p>
    );
  }
  if (!publicRuntimeConfig().useSupabaseCatalog) return null;
  return (
    <p className="border-b border-emerald-500/20 bg-emerald-500/10 px-4 py-2 text-center text-sm text-emerald-100">
      Live Supabase catalog is enabled. Run go-live sync if search is empty.
    </p>
  );
}
