/** Allow only same-site relative paths after auth (blocks open redirects). */
export function safeRedirectPath(next: string | null | undefined, fallback = "/dashboard"): string {
  if (!next || !next.startsWith("/") || next.startsWith("//") || next.includes(":")) {
    return fallback;
  }
  if (next.length > 512) return fallback;
  return next;
}
