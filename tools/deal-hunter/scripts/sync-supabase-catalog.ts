#!/usr/bin/env tsx
import { refreshSupabaseDealCache } from "../server/catalog-pool.js";
import { syncCatalogToSupabase } from "../server/integrations/supabase-sync.js";

const result = await syncCatalogToSupabase();
const cache = await refreshSupabaseDealCache();
console.log(JSON.stringify({ ...result, catalogCache: cache }, null, 2));
process.exit(result.ok ? 0 : 1);
