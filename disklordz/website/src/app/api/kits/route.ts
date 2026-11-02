import { NextRequest, NextResponse } from "next/server";

import type { KitManifest } from "@/lib/manifest";
import { parseHistoryFilters } from "@/lib/kits/history-query";
import { saveKitForUser } from "@/lib/kits/persist";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function GET(req: NextRequest) {
  if (!isSupabaseConfigured()) {
    return NextResponse.json({ error: "auth_not_configured" }, { status: 503 });
  }

  const supabase = await createClient();
  if (!supabase) {
    return NextResponse.json({ error: "auth_not_configured" }, { status: 503 });
  }

  const {
    data: { user },
  } = await supabase.auth.getUser();
  if (!user) {
    return NextResponse.json({ error: "unauthorized" }, { status: 401 });
  }

  const filters = parseHistoryFilters(new URL(req.url).searchParams);

  let query = supabase
    .from("saved_kits")
    .select(
      "kit_id, prompt, preset_id, artist_lane, manifest, created_at, spec_mode, spec_engine, spec_key, spec_bpm, variation_batch_id, variation_label",
    )
    .eq("user_id", user.id)
    .order("created_at", { ascending: false })
    .limit(filters.limit);

  if (filters.q) {
    query = query.ilike("prompt", `%${filters.q}%`);
  }
  if (filters.mode) {
    query = query.eq("spec_mode", filters.mode);
  }
  if (filters.engine) {
    query = query.eq("spec_engine", filters.engine);
  }
  if (filters.key) {
    query = query.ilike("spec_key", `%${filters.key}%`);
  }
  if (filters.presetId) {
    query = query.eq("preset_id", filters.presetId);
  }
  if (filters.batchId) {
    query = query.eq("variation_batch_id", filters.batchId);
  }
  if (filters.bpmMin !== undefined && Number.isFinite(filters.bpmMin)) {
    query = query.gte("spec_bpm", filters.bpmMin);
  }
  if (filters.bpmMax !== undefined && Number.isFinite(filters.bpmMax)) {
    query = query.lte("spec_bpm", filters.bpmMax);
  }
  if (filters.from) {
    const from =
      filters.from.length === 10 ? `${filters.from}T00:00:00.000Z` : filters.from;
    query = query.gte("created_at", from);
  }
  if (filters.to) {
    const to =
      filters.to.length === 10 ? `${filters.to}T23:59:59.999Z` : filters.to;
    query = query.lte("created_at", to);
  }

  const { data, error } = await query;

  if (error) {
    return NextResponse.json({ error: error.message }, { status: 500 });
  }

  return NextResponse.json({ kits: data ?? [], filters });
}

export async function POST(req: Request) {
  if (!isSupabaseConfigured()) {
    return NextResponse.json({ error: "auth_not_configured" }, { status: 503 });
  }

  const supabase = await createClient();
  if (!supabase) {
    return NextResponse.json({ error: "auth_not_configured" }, { status: 503 });
  }

  const {
    data: { user },
  } = await supabase.auth.getUser();
  if (!user) {
    return NextResponse.json({ error: "unauthorized" }, { status: 401 });
  }

  let manifest: KitManifest;
  try {
    const body = await req.json();
    manifest = body.manifest as KitManifest;
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  if (manifest?.format !== "DISKLORDZ_DRUM_KIT_MANIFEST") {
    return NextResponse.json({ error: "invalid_manifest" }, { status: 400 });
  }

  const saved = await saveKitForUser(supabase, user.id, manifest);
  if (!saved.ok) {
    return NextResponse.json({ error: saved.error }, { status: 500 });
  }

  return NextResponse.json({ ok: true, kitId: manifest.kitId });
}
