import { NextResponse } from "next/server";

import { hostedDealsList } from "@/lib/hosted-catalog";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const minScore = Number(new URL(req.url).searchParams.get("minDealScore") ?? 0);
  const data = hostedDealsList(minScore);
  return NextResponse.json({ data, count: data.length, priceDrops: 0 });
}
