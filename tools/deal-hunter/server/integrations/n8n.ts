export type N8nEvent =
  | "deal_alert"
  | "email_digest"
  | "autopilot_complete"
  | "community_submitted";

export async function emitN8nEvent(
  event: N8nEvent,
  payload: Record<string, unknown>,
): Promise<{ sent: boolean; skipped?: string }> {
  const url =
    event === "email_digest"
      ? process.env.N8N_WEBHOOK_EMAIL
      : process.env.N8N_WEBHOOK_DEAL_ALERT ?? process.env.N8N_WEBHOOK_URL;

  if (!url) {
    return { sent: false, skipped: "N8N webhook URL not configured" };
  }

  try {
    const res = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ event, emittedAt: new Date().toISOString(), ...payload }),
    });
    return { sent: res.ok };
  } catch {
    return { sent: false, skipped: "n8n_unreachable" };
  }
}

export function assertN8nCronAuth(headerSecret: string | undefined): boolean {
  const expected = process.env.N8N_WEBHOOK_SECRET;
  if (!expected) return process.env.NODE_ENV !== "production";
  return headerSecret === expected;
}
