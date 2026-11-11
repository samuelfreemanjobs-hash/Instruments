import { NextResponse } from "next/server";

import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET() {
  if (shouldProxyToExternalDealApi()) return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  return NextResponse.json({
    agents: [
      { id: "pricing", name: "Pricing", status: "demo" },
      { id: "discovery", name: "Discovery", status: "demo" },
    ],
  });
}
