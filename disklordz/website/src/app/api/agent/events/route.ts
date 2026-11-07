import { NextRequest, NextResponse } from "next/server";

import { listAgentEvents } from "@/lib/grok/events";

/** PWA polls for Grok completion events by clientRef (requestId from POST /api/agent). */
export async function GET(req: NextRequest) {
  const clientRef = req.nextUrl.searchParams.get("clientRef")?.trim();
  if (!clientRef || clientRef.length > 128) {
    return NextResponse.json({ error: "clientRef_required" }, { status: 400 });
  }
  const since = req.nextUrl.searchParams.get("since")?.trim() || undefined;
  const events = await listAgentEvents(clientRef, since);
  return NextResponse.json({ events });
}
