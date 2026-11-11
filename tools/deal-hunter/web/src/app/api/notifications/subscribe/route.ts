import { NextResponse } from "next/server";

import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function POST() {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  return NextResponse.json({
    ok: true,
    mode: "bundled_demo",
    note: "Push stored when DEAL_API_ORIGIN + VAPID are configured on the API host.",
  });
}
