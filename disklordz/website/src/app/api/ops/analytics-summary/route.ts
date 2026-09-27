import { NextRequest, NextResponse } from "next/server";

import { getSupabaseAdmin } from "@/lib/supabase/admin";

/** WO-SAAS-025: ops funnel summary (requires OPS_API_KEY). */
export async function GET(req: NextRequest) {
  const key = process.env.OPS_API_KEY;
  if (!key) {
    return NextResponse.json({ error: "not_configured" }, { status: 503 });
  }
  const provided = req.headers.get("x-ops-key");
  if (provided !== key) {
    return NextResponse.json({ error: "unauthorized" }, { status: 401 });
  }

  const admin = getSupabaseAdmin();
  if (!admin) {
    return NextResponse.json({ error: "supabase_admin_required" }, { status: 503 });
  }

  const since = new Date(Date.now() - 7 * 24 * 60 * 60 * 1000).toISOString();
  const { data, error } = await admin
    .from("generation_events")
    .select("event, ok, preset_id, mode, engine")
    .gte("created_at", since);

  if (error) {
    return NextResponse.json({ error: "query_failed" }, { status: 500 });
  }

  const rows = data ?? [];
  const summary = {
    since,
    total: rows.length,
    success: rows.filter((r) => r.event === "generate_success").length,
    fail: rows.filter((r) => r.event === "generate_fail").length,
    byMode: {} as Record<string, number>,
    byPreset: {} as Record<string, number>,
  };

  for (const row of rows) {
    if (row.mode) summary.byMode[row.mode] = (summary.byMode[row.mode] ?? 0) + 1;
    if (row.preset_id) {
      summary.byPreset[row.preset_id] = (summary.byPreset[row.preset_id] ?? 0) + 1;
    }
  }

  return NextResponse.json(summary);
}
