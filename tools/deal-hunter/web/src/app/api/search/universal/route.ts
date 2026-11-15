import { NextResponse } from "next/server";

import { hostedUniversalSearch } from "@/lib/hosted-catalog";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET(req: Request) {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const body = hostedUniversalSearch(new URL(req.url).searchParams);
  return NextResponse.json(body);
}
