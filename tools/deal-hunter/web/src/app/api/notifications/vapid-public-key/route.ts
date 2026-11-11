import { NextResponse } from "next/server";

import { publicRuntimeConfig } from "@/lib/public-env";
import { shouldProxyToExternalDealApi } from "@/lib/deal-api-origin";

export async function GET() {
  if (shouldProxyToExternalDealApi()) {
    return NextResponse.json({ error: "use_proxy" }, { status: 404 });
  }
  const key = publicRuntimeConfig().vapidPublicKey;
  return NextResponse.json({ configured: Boolean(key), publicKey: key || null });
}
