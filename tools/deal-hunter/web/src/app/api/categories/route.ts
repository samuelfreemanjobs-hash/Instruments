import { NextResponse } from "next/server";

import { hostedCategories } from "@/lib/hosted-api-handlers";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET() {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  return NextResponse.json(hostedCategories());
}
