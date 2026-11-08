"use server";

import { inngest, inngestConfigured } from "@/inngest/client";
import { requireAdminCaller } from "@/lib/require-admin";

export async function enqueueCatalogSync() {
  const gate = await requireAdminCaller();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  if (!inngestConfigured()) return { ok: false as const, error: "inngest_not_configured" };
  await inngest.send({ name: "bfm/catalog.sync", data: { source: "admin" } });
  return { ok: true as const };
}

export async function enqueueStealDispatch() {
  const gate = await requireAdminCaller();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  if (!inngestConfigured()) return { ok: false as const, error: "inngest_not_configured" };
  await inngest.send({ name: "bfm/steals.dispatch", data: { source: "admin" } });
  return { ok: true as const };
}
