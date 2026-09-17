import { NextResponse } from "next/server";

import { getBillingSnapshot } from "@/lib/credits";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function GET() {
  if (!isSupabaseConfigured()) {
    return NextResponse.json({ authenticated: false });
  }

  const supabase = await createClient();
  if (!supabase) {
    return NextResponse.json({ authenticated: false });
  }

  const {
    data: { user },
  } = await supabase.auth.getUser();

  if (!user) {
    return NextResponse.json({ authenticated: false });
  }

  const billing = await getBillingSnapshot(user.id);
  if (!billing) {
    return NextResponse.json({
      authenticated: true,
      billingUnavailable: true,
      message: "Set SUPABASE_SERVICE_ROLE_KEY for credit billing.",
    });
  }

  return NextResponse.json({
    authenticated: true,
    plan: billing.plan,
    creditsBalance: billing.creditsBalance,
    generationCreditCost: billing.generationCreditCost,
    unlimited: billing.plan === "pro",
  });
}
