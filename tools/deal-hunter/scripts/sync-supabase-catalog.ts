#!/usr/bin/env tsx
import { syncCatalogToSupabase } from "../server/integrations/supabase-sync.js";

const result = await syncCatalogToSupabase();
console.log(JSON.stringify(result, null, 2));
process.exit(result.ok ? 0 : 1);
