import { NextResponse, type NextRequest } from "next/server";

import { BLOCKED_PUBLIC_API_PREFIXES } from "../deal-api-proxy-allowlist";
import { updateSession } from "@/lib/supabase/middleware";

export async function middleware(request: NextRequest) {
  const path = request.nextUrl.pathname;
  for (const prefix of BLOCKED_PUBLIC_API_PREFIXES) {
    if (path === prefix || path.startsWith(`${prefix}/`)) {
      return new NextResponse("Forbidden", { status: 403 });
    }
  }
  return updateSession(request);
}

export const config = {
  matcher: ["/((?!_next/static|_next/image|favicon.ico|.*\\.(?:svg|png|jpg|jpeg|gif|webp)$).*)"],
};
