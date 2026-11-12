/**
 * Paths proxied to DEAL_API_ORIGIN. Admin/cron routes are intentionally omitted.
 */
export const DEAL_API_PROXY_SOURCES = [
  "/api/v1/:path*",
  "/api/search/:path*",
  "/api/catalog/source",
  "/api/agents/:path*",
  "/api/categories",
  "/api/orchestrator/chat",
  "/api/assistant/chat",
  "/api/discover/:path*",
  "/api/recommendations",
  "/api/intelligence/:path*",
  "/api/autopilot/agents",
  "/api/autopilot/digest",
  "/api/coupons",
  "/api/calendar",
  "/api/notifications/vapid-public-key",
  "/api/notifications/steal-hits",
  "/api/notifications/subscribe",
] as const;

/** Legacy export — prefer isBlockedPublicApiPath for middleware. */
export const BLOCKED_PUBLIC_API_PREFIXES = ["/api/admin", "/api/notifications/dispatch-steals"] as const;

const BLOCKED_API_SEGMENTS = [
  ["admin"],
  ["notifications", "dispatch-steals"],
] as const;

/** Collapse `.` / `..` after decode (blocks `/api/x/../admin` style tricks). */
export function normalizeRequestPath(pathname: string): string | null {
  let path = pathname;
  try {
    path = decodeURI(pathname);
  } catch {
    return null;
  }
  if (!path.startsWith("/")) return null;
  path = path.replace(/\\/g, "/");
  const raw = path.split("/");
  const segments: string[] = [];
  for (const seg of raw) {
    if (!seg || seg === ".") continue;
    if (seg === "..") {
      if (segments.length === 0) return null;
      segments.pop();
      continue;
    }
    segments.push(seg);
  }
  return `/${segments.join("/")}`;
}

/** Segment-exact block list (avoids `/api/administrator` false positives). */
export function isBlockedPublicApiPath(pathname: string): boolean {
  const path = normalizeRequestPath(pathname);
  if (!path) return false;
  const segments = path.split("/").filter(Boolean);
  if (segments[0] !== "api") return false;
  for (const blocked of BLOCKED_API_SEGMENTS) {
    if (blocked.length > segments.length - 1) continue;
    let matches = true;
    for (let i = 0; i < blocked.length; i++) {
      if (segments[i + 1] !== blocked[i]) {
        matches = false;
        break;
      }
    }
    if (matches) return true;
  }
  return false;
}
