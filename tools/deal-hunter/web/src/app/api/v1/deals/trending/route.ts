import { NextResponse } from "next/server";

import { hostedDealsList } from "@/lib/hosted-catalog";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET() {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const deals = hostedDealsList().sort((a, b) => b.stealScore - a.stealScore);
  return NextResponse.json({
    hiddenGems: deals.filter((d) => d.stealScore >= 78).slice(0, 8),
    priceAlerts: deals.filter((d) => d.discountPct >= 35).slice(0, 8),
    fetchedAt: new Date().toISOString(),
  });
}
