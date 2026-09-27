import { NextResponse } from "next/server";

import { isStripeConfigured } from "@/lib/stripe/server";

/** Readiness for ops agents / load balancers — no secrets in response (WO-SAAS-019). */
export async function GET() {
  const supabasePublic =
    Boolean(process.env.NEXT_PUBLIC_SUPABASE_URL) &&
    Boolean(process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY);
  const supabaseService = Boolean(process.env.SUPABASE_SERVICE_ROLE_KEY);
  const stripe = isStripeConfigured();

  const ready = supabasePublic;

  return NextResponse.json(
    {
      ok: ready,
      service: "disklordz-website",
      checks: {
        supabasePublic,
        supabaseService,
        stripe,
        kitStorage: supabaseService ? "supabase" : "local",
      },
    },
    { status: ready ? 200 : 503 },
  );
}
