import { NextResponse } from "next/server";

import { hostedDealsList } from "@/lib/hosted-catalog";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST() {
  if (shouldProxyToExternalDealApi()) return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  const picks = hostedDealsList().sort((a, b) => b.stealScore - a.stealScore).slice(0, 6);
  return NextResponse.json({
    recommendations: picks.map((d) => ({
      dealId: d.id,
      title: d.title,
      reason: "High steal score in demo refresh",
      price: d.price,
    })),
  });
}
