import { NextResponse } from "next/server";

import { hostedStealHits } from "@/lib/hosted-api-handlers";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const min = Number(new URL(req.url).searchParams.get("minStealScore") ?? 85);
  return NextResponse.json(hostedStealHits(min));
}
