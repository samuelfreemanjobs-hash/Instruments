const LOCAL_RE = /^https?:\/\/(127\.0\.0\.1|localhost)(:\d+)?\/?$/i;

/** When false, Next serves the bundled demo catalog API routes. */
export function shouldProxyToExternalDealApi(): boolean {
  const origin = process.env.DEAL_API_ORIGIN?.trim();
  if (!origin) return false;
  return !LOCAL_RE.test(origin);
}

export function dealApiOriginForServer(): string {
  return process.env.DEAL_API_ORIGIN?.trim() || "http://127.0.0.1:3847";
}
