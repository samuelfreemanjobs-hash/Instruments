import { inngest } from "./client";

const apiOrigin = () => process.env.DEAL_API_ORIGIN ?? "http://127.0.0.1:3847";
const cronHeaders = () => {
  const h: Record<string, string> = { "Content-Type": "application/json" };
  if (process.env.N8N_WEBHOOK_SECRET) h["x-n8n-secret"] = process.env.N8N_WEBHOOK_SECRET;
  return h;
};

export const catalogSyncCron = inngest.createFunction(
  { id: "bfm-catalog-sync", name: "BFM catalog sync" },
  { cron: "0 * * * *" },
  async () => {
    const res = await fetch(`${apiOrigin()}/api/admin/sync-catalog/cron`, {
      method: "POST",
      headers: cronHeaders(),
    });
    const body = await res.text();
    return { status: res.status, body: body.slice(0, 500) };
  },
);

export const stealDispatchCron = inngest.createFunction(
  { id: "bfm-steal-dispatch", name: "BFM steal dispatch" },
  { cron: "*/15 * * * *" },
  async () => {
    const res = await fetch(`${apiOrigin()}/api/notifications/dispatch-steals`, {
      method: "POST",
      headers: cronHeaders(),
    });
    return { status: res.status, body: await res.text() };
  },
);

export const inngestFunctions = [catalogSyncCron, stealDispatchCron];
