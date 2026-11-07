import { NextRequest, NextResponse } from "next/server";

import { recordAgentEvent } from "@/lib/grok/events";
import { parseGrokCompletionBody, verifyAgentWebhookSignature } from "@/lib/grok/webhook";

/**
 * Inbound webhook: Grok Bot posts completion summaries here.
 * Configure callback URL in Grok: https://<your-vercel-app>/api/agent/webhook
 */
export async function POST(req: NextRequest) {
  const raw = await req.text();
  const sig = req.headers.get("x-grok-signature");

  if (!verifyAgentWebhookSignature(raw, sig)) {
    return NextResponse.json({ error: "invalid_signature" }, { status: 401 });
  }

  let body: unknown;
  try {
    body = JSON.parse(raw);
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  const parsed = parseGrokCompletionBody(body);
  if (!parsed) {
    return NextResponse.json({ error: "missing_fields" }, { status: 400 });
  }

  await recordAgentEvent(parsed.clientRef, parsed.status, parsed.summary);

  return NextResponse.json({ ok: true });
}
