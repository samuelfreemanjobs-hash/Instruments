import { syncCatalogToSupabase } from "./supabase-sync.js";
import { refreshSupabaseDealCache } from "../catalog-pool.js";

let timer: ReturnType<typeof setInterval> | null = null;

export async function runCatalogSyncJob(): Promise<{
  sync: Awaited<ReturnType<typeof syncCatalogToSupabase>>;
  cache: Awaited<ReturnType<typeof refreshSupabaseDealCache>>;
}> {
  const sync = await syncCatalogToSupabase();
  const cache = await refreshSupabaseDealCache();
  return { sync, cache };
}

export function startCatalogSyncScheduler(): void {
  const minutes = Number(process.env.CATALOG_SYNC_INTERVAL_MINUTES ?? 0);
  if (!minutes || minutes < 5) return;
  if (timer) return;

  const ms = minutes * 60_000;
  void runCatalogSyncJob();
  timer = setInterval(() => {
    void runCatalogSyncJob();
  }, ms);
  console.log(`Catalog sync scheduler: every ${minutes} minutes`);
}
