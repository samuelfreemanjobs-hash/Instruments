/** Allow only same-site relative paths after auth (blocks open redirects). */
export function safeRedirectPath(next: string | null | undefined, fallback = "/dashboard"): string {
  const raw = (next ?? "").trim();
  if (
    !raw.startsWith("/") ||
    raw.startsWith("//") ||
    raw.includes(":") ||
    raw.includes("@") ||
    raw.includes("?") ||
    raw.includes("#") ||
    raw.includes("%")
  ) {
    return fallback;
  }
  if (raw.includes("\\")) return fallback;
  const segments = raw.split("/");
  const out: string[] = [];
  for (const seg of segments) {
    if (!seg || seg === ".") continue;
    if (seg === "..") {
      if (out.length === 0) return fallback;
      out.pop();
      continue;
    }
    out.push(seg);
  }
  const path = `/${out.join("/")}`;
  if (path.length > 512) return fallback;
  return path;
}
