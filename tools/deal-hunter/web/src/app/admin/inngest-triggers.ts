"use server";

import { inngest, inngestConfigured } from "@/inngest/client";

export async function enqueueCatalogSync() {
  if (!inngestConfigured()) return { ok: false as const, error: "inngest_not_configured" };
  await inngest.send({ name: "bfm/catalog.sync", data: { source: "admin" } });
  return { ok: true as const };
}

export async function enqueueStealDispatch() {
  if (!inngestConfigured()) return { ok: false as const, error: "inngest_not_configured" };
  await inngest.send({ name: "bfm/steals.dispatch", data: { source: "admin" } });
  return { ok: true as const };
}
