import { NextResponse } from "next/server";

import { processDueLaunchEmails } from "@/lib/launch/process-queue";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

/** Vercel Cron — processes due launch emails. Auth: CRON_SECRET header or query. */
export async function GET(req: Request) {
  const secret = process.env.CRON_SECRET;
  if (secret) {
    const url = new URL(req.url);
    const provided =
      req.headers.get("authorization")?.replace(/^Bearer\s+/i, "") ??
      url.searchParams.get("secret");
    if (provided !== secret) {
      return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
    }
  }

  const supabase = getSupabaseAdmin();
  if (!supabase) {
    return NextResponse.json({ ok: false, error: "Supabase admin not configured" }, { status: 503 });
  }

  try {
    const result = await processDueLaunchEmails(supabase);
    return NextResponse.json({ ok: true, ...result });
  } catch (e) {
    console.error("[cron/launch-queue]", e);
    return NextResponse.json({ ok: false, error: "Processing failed" }, { status: 500 });
  }
}
