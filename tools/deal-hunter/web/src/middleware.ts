import { NextResponse, type NextRequest } from "next/server";

import { isBlockedPublicApiPath } from "../deal-api-proxy-allowlist";
import { updateSession } from "@/lib/supabase/middleware";

export async function middleware(request: NextRequest) {
  if (isBlockedPublicApiPath(request.nextUrl.pathname)) {
    return new NextResponse("Forbidden", { status: 403 });
  }
  return updateSession(request);
}

export const config = {
  matcher: ["/((?!_next/static|_next/image|favicon.ico|.*\\.(?:svg|png|jpg|jpeg|gif|webp)$).*)"],
};
