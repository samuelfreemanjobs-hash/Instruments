const DAILY_LIMIT = Number(process.env.SAAS_DAILY_GEN_LIMIT ?? "20");
const WINDOW_MS = 24 * 60 * 60 * 1000;

type Bucket = { count: number; windowStart: number };

const buckets = new Map<string, Bucket>();

export function checkRateLimit(key: string): { ok: true } | { ok: false; retryAfterSec: number } {
  const now = Date.now();
  let bucket = buckets.get(key);
  if (!bucket || now - bucket.windowStart > WINDOW_MS) {
    bucket = { count: 0, windowStart: now };
    buckets.set(key, bucket);
  }
  if (bucket.count >= DAILY_LIMIT) {
    const retryAfterSec = Math.ceil((bucket.windowStart + WINDOW_MS - now) / 1000);
    return { ok: false, retryAfterSec };
  }
  bucket.count += 1;
  return { ok: true };
}
