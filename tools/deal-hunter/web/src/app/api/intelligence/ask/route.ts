import { NextResponse } from "next/server";

import { hostedChatAnswer } from "@/lib/hosted-api-handlers";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const body = (await req.json().catch(() => ({}))) as { question?: string };
  const out = hostedChatAnswer(String(body.question ?? ""));
  return NextResponse.json({
    answer: out.answer,
    deals: out.deals,
    generatedAt: out.generatedAt,
  });
}
