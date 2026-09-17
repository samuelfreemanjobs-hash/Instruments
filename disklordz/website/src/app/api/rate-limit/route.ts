import { NextRequest, NextResponse } from "next/server";

import { peekRateLimit } from "@/lib/rate-limit";

export async function GET(req: NextRequest) {
  const ip =
    req.headers.get("x-forwarded-for")?.split(",")[0]?.trim() ??
    req.headers.get("x-real-ip") ??
    "anonymous";
  const stats = peekRateLimit(ip);
  return NextResponse.json(stats);
}
