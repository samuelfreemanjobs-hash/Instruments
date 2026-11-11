import { NextResponse } from "next/server";

import { hostedDealsList } from "@/lib/hosted-catalog";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST() {
  if (shouldProxyToExternalDealApi()) return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  const top = hostedDealsList().sort((a, b) => b.stealScore - a.stealScore).slice(0, 5);
  return NextResponse.json({
    summary: `Top ${top.length} steals in bundled demo catalog.`,
    picks: top.map((d) => ({ id: d.id, title: d.title, stealScore: d.stealScore })),
    generatedAt: new Date().toISOString(),
  });
}
