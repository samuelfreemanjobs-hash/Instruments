// web/src/middleware.ts
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";

export function middleware(request: NextRequest) {
  const { pathname } = request.nextUrl;
  if (!pathname.startsWith("/admin")) return NextResponse.next();

  const session = request.cookies.get("session")?.value;
  if (!session) {
    return NextResponse.redirect(new URL("/auth/login?next=" + pathname, request.url));
  }
  // Decode session and check role === ADMIN (implement in lib/auth)
  return NextResponse.next();
}

export const config = {
  matcher: ["/admin/:path*"],
};
