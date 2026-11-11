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

export const BLOCKED_PUBLIC_API_PREFIXES = ["/api/admin", "/api/notifications/dispatch-steals"] as const;
