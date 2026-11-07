import "server-only";

import { getSupabaseAdmin } from "@/lib/supabase/admin";

export type AgentCompletionEvent = {
  clientRef: string;
  status: string;
  summary: string;
  receivedAt: string;
};

const memoryEvents = new Map<string, AgentCompletionEvent[]>();
const MEMORY_CAP = 40;

export async function recordAgentEvent(
  clientRef: string,
  status: string,
  summary: string,
): Promise<void> {
  const trimmed = summary.slice(0, 16_000);
  const event: AgentCompletionEvent = {
    clientRef,
    status: status.slice(0, 128),
    summary: trimmed,
    receivedAt: new Date().toISOString(),
  };

  const admin = getSupabaseAdmin();
  if (admin) {
    const { error } = await admin.from("grok_agent_events").insert({
      client_ref: clientRef,
      status: event.status,
      summary: event.summary,
    });
    if (!error) return;
    console.error("grok_agent_events insert failed, using memory fallback", error.message);
  }

  const list = memoryEvents.get(clientRef) ?? [];
  list.push(event);
  memoryEvents.set(clientRef, list.slice(-MEMORY_CAP));
}

export async function listAgentEvents(
  clientRef: string,
  since?: string,
): Promise<AgentCompletionEvent[]> {
  const admin = getSupabaseAdmin();
  if (admin) {
    let q = admin
      .from("grok_agent_events")
      .select("client_ref, status, summary, created_at")
      .eq("client_ref", clientRef)
      .order("created_at", { ascending: false })
      .limit(20);
    if (since) {
      q = q.gt("created_at", since);
    }
    const { data, error } = await q;
    if (!error && data) {
      return data.map((row) => ({
        clientRef: row.client_ref as string,
        status: row.status as string,
        summary: row.summary as string,
        receivedAt: row.created_at as string,
      }));
    }
  }

  const list = memoryEvents.get(clientRef) ?? [];
  if (!since) return [...list].reverse();
  return list.filter((e) => e.receivedAt > since).reverse();
}
