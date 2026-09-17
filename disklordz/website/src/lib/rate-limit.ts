export const DAILY_LIMIT = Number(process.env.SAAS_DAILY_GEN_LIMIT ?? "20");
const WINDOW_MS = 24 * 60 * 60 * 1000;

type Bucket = { count: number; windowStart: number };

const buckets = new Map<string, Bucket>();

export type RateLimitResult =
  | { ok: true; remaining: number; limit: number }
  | { ok: false; retryAfterSec: number; limit: number };

export function checkRateLimit(key: string): RateLimitResult {
  const now = Date.now();
  let bucket = buckets.get(key);
  if (!bucket || now - bucket.windowStart > WINDOW_MS) {
    bucket = { count: 0, windowStart: now };
    buckets.set(key, bucket);
  }
  if (bucket.count >= DAILY_LIMIT) {
    const retryAfterSec = Math.ceil((bucket.windowStart + WINDOW_MS - now) / 1000);
    return { ok: false, retryAfterSec, limit: DAILY_LIMIT };
  }
  bucket.count += 1;
  return { ok: true, remaining: DAILY_LIMIT - bucket.count, limit: DAILY_LIMIT };
}

export function peekRateLimit(key: string): { remaining: number; limit: number } {
  const now = Date.now();
  const bucket = buckets.get(key);
  if (!bucket || now - bucket.windowStart > WINDOW_MS) {
    return { remaining: DAILY_LIMIT, limit: DAILY_LIMIT };
  }
  return { remaining: Math.max(0, DAILY_LIMIT - bucket.count), limit: DAILY_LIMIT };
}
