import { NextResponse } from "next/server";

import { hostedPricing } from "@/lib/hosted-api-handlers";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const body = (await req.json().catch(() => ({}))) as { question?: string };
  return NextResponse.json(hostedPricing(String(body.question ?? "")));
}
