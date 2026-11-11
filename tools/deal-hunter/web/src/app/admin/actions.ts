"use server";

import { requireAdminCaller } from "@/lib/require-admin";

const origin = () => process.env.DEAL_API_ORIGIN ?? "http://127.0.0.1:3847";

function adminHeaders() {
  const secret = process.env.BFM_ADMIN_SECRET ?? process.env.SYNC_CATALOG_SECRET ?? "";
  const h: Record<string, string> = { "Content-Type": "application/json" };
  if (secret) h["x-admin-secret"] = secret;
  if (process.env.N8N_WEBHOOK_SECRET) h["x-n8n-secret"] = process.env.N8N_WEBHOOK_SECRET;
  return h;
}

export async function fetchAdminStatus() {
  const gate = await requireAdminCaller();
  if (!gate.ok) return { ok: false as const, error: gate.error };

  const res = await fetch(`${origin()}/api/admin/status`, {
    headers: adminHeaders(),
    cache: "no-store",
  });
  if (!res.ok) {
    const t = await res.text();
    return { ok: false as const, error: t || res.statusText };
  }
  const data = (await res.json()) as {
    catalogSource: string;
    vapidConfigured: boolean;
    inngestNote: string;
    integrations: {
      retailerSlug: string;
      health: string;
      dataFreshnessMinutes: number;
      lastSyncAt: string;
    }[];
  };
  return { ok: true as const, data };
}

export async function triggerCatalogSync() {
  const gate = await requireAdminCaller();
  if (!gate.ok) return { ok: false as const, error: gate.error };

  const res = await fetch(`${origin()}/api/admin/sync-catalog/cron`, {
    method: "POST",
    headers: adminHeaders(),
  });
  if (!res.ok) return { ok: false as const, error: await res.text() };
  return { ok: true as const, body: await res.json() };
}

export async function triggerStealDispatch() {
  const gate = await requireAdminCaller();
  if (!gate.ok) return { ok: false as const, error: gate.error };

  const res = await fetch(`${origin()}/api/notifications/dispatch-steals`, {
    method: "POST",
    headers: adminHeaders(),
  });
  if (!res.ok) return { ok: false as const, error: await res.text() };
  return { ok: true as const, body: await res.json() };
}
