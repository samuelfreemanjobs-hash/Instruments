import "server-only";

import { createHmac, timingSafeEqual } from "crypto";

export function verifyAgentWebhookSignature(
  rawBody: string,
  signatureHeader: string | null,
): boolean {
  const secret = process.env.GROK_WEBHOOK_SECRET?.trim();
  if (!secret) return true;
  if (!signatureHeader) return false;
  const expected = createHmac("sha256", secret).update(rawBody).digest("hex");
  const provided = signatureHeader.replace(/^sha256=/i, "").trim();
  try {
    return timingSafeEqual(Buffer.from(expected), Buffer.from(provided));
  } catch {
    return false;
  }
}

export function parseGrokCompletionBody(body: unknown): {
  clientRef: string;
  status: string;
  summary: string;
} | null {
  if (!body || typeof body !== "object") return null;
  const o = body as Record<string, unknown>;
  const clientRef = String(
    o.client_ref ?? o.clientRef ?? o.request_id ?? o.requestId ?? "",
  ).slice(0, 128);
  const summary = String(o.summary ?? o.text ?? o.message ?? "").slice(0, 16_000);
  const status = String(o.status ?? o.state ?? "completed").slice(0, 128);
  if (!clientRef || !summary) return null;
  return { clientRef, status, summary };
}
