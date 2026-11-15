import { NextResponse } from "next/server";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { shouldProxyToExternalDealApi, dealApiOriginForServer } from "@/lib/deal-api-origin";

const MAX_ENDPOINT = 512;

async function requireUser() {
  if (!isSupabaseConfigured()) return null;
  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  return data.user ?? null;
}

/** Store push subscription in Supabase (signed-in users only). */
export async function POST(req: Request) {
  const user = await requireUser();
  if (!user) {
    return NextResponse.json({ error: "sign_in_required" }, { status: 401 });
  }

  const body = (await req.json().catch(() => ({}))) as {
    endpoint?: string;
    minStealScore?: number;
    keys?: { p256dh?: string; auth?: string };
    email?: string;
    notifyEmail?: boolean;
  };

  const endpoint = String(body.endpoint ?? "").slice(0, MAX_ENDPOINT);
  if (!endpoint) return NextResponse.json({ error: "missing_endpoint" }, { status: 400 });

  const min = Math.min(100, Math.max(50, Math.round(Number(body.minStealScore) || 85)));
  const p256dh = body.keys?.p256dh?.slice(0, 256) ?? "";
  const auth = body.keys?.auth?.slice(0, 128) ?? "";

  if (p256dh && auth) {
    const supabase = await createClient();
    const { error } = await supabase!.from("user_push_subscriptions").upsert(
      {
        user_id: user.id,
        endpoint,
        p256dh,
        auth,
        min_steal_score: min,
        active: true,
        updated_at: new Date().toISOString(),
      },
      { onConflict: "user_id,endpoint" },
    );
    if (error) return NextResponse.json({ error: error.message }, { status: 500 });
  }

  if (shouldProxyToExternalDealApi() && process.env.BFM_ADMIN_SECRET) {
    const origin = dealApiOriginForServer();
    await fetch(`${origin}/api/notifications/subscribe`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "x-admin-secret": process.env.BFM_ADMIN_SECRET,
      },
      body: JSON.stringify({ ...body, endpoint, minStealScore: min }),
    }).catch(() => undefined);
  }

  return NextResponse.json({ ok: true, endpoint, minStealScore: min });
}

export async function PATCH(req: Request) {
  const user = await requireUser();
  if (!user) {
    return NextResponse.json({ error: "sign_in_required" }, { status: 401 });
  }

  const body = (await req.json().catch(() => ({}))) as { endpoint?: string; minStealScore?: number };
  const endpoint = String(body.endpoint ?? "").slice(0, MAX_ENDPOINT);
  if (!endpoint) return NextResponse.json({ error: "missing_endpoint" }, { status: 400 });
  const min = Math.min(100, Math.max(50, Math.round(Number(body.minStealScore) || 85)));

  const supabase = await createClient();
  const { data, error } = await supabase!
    .from("user_push_subscriptions")
    .update({ min_steal_score: min, updated_at: new Date().toISOString() })
    .eq("user_id", user.id)
    .eq("endpoint", endpoint)
    .select("id")
    .maybeSingle();

  if (error) return NextResponse.json({ error: error.message }, { status: 500 });
  if (!data) return NextResponse.json({ error: "not_found" }, { status: 404 });
  return NextResponse.json({ ok: true, minStealScore: min });
}