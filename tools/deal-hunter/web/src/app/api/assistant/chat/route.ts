import { NextResponse } from "next/server";

import { hostedChatAnswer } from "@/lib/hosted-api-handlers";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const body = (await req.json().catch(() => ({}))) as { message?: string };
  const out = hostedChatAnswer(String(body.message ?? ""));
  return NextResponse.json({
    message: out.message,
    deals: out.deals,
    intelligenceSummary: out.intelligenceSummary,
    generatedAt: out.generatedAt,
  });
}
