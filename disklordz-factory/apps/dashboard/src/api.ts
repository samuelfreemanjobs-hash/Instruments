const API_BASE = import.meta.env.VITE_API_URL ?? "/api";

async function fetchJson<T>(path: string, init?: RequestInit): Promise<T> {
  const res = await fetch(`${API_BASE}${path}`, {
    ...init,
    headers: {
      "Content-Type": "application/json",
      ...init?.headers,
    },
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || res.statusText);
  }
  if (res.status === 204) {
    return undefined as T;
  }
  return res.json() as Promise<T>;
}

export type FactoryDashboard = {
  active_projects: number;
  active_eps: number;
  active_products: number;
  today_generated: number;
  today_approved: number;
  today_extracted: number;
  catalog_audio_assets: number;
  catalog_tracks: number;
  catalog_products: number;
  approval_pending: number;
  queue_production: number;
  queue_qa: number;
  queue_publishing: number;
  revenue_total_usd: number;
};

export type ApprovalItem = {
  asset_id: string;
  title: string;
  audio_qa: string;
  visual_qa: string;
  product_qa: string;
  rights_qa: string;
  status: string;
};

export type NightShiftRun = {
  run_id: string;
  batch_id: string;
  mission: string;
  status: string;
  assets_created: number;
  approval_items: number;
  finished_at: string | null;
};

export type NightShiftStep = {
  schedule_time: string;
  label: string;
  factory_stage: string;
  status: string;
};

export const api = {
  dashboard: () => fetchJson<FactoryDashboard>("/factory/dashboard"),
  approvalQueue: () => fetchJson<ApprovalItem[]>("/approval-queue"),
  runNightShift: (mission?: string) =>
    fetchJson<NightShiftRun>("/night-shift/run", {
      method: "POST",
      body: JSON.stringify(
        mission
          ? { mission, target_count: 25, artist_ids: ["DL002", "DL003"] }
          : {},
      ),
    }),
  latestNightShift: () => fetchJson<NightShiftRun | null>("/night-shift/runs/latest"),
  nightShiftSchedule: () => fetchJson<NightShiftStep[]>("/night-shift/schedule"),
  approve: (assetId: string) =>
    fetchJson<ApprovalItem>(`/approval-queue/${encodeURIComponent(assetId)}/approve`, {
      method: "POST",
    }),
  reject: (assetId: string) =>
    fetchJson<ApprovalItem>(`/approval-queue/${encodeURIComponent(assetId)}/reject`, {
      method: "POST",
    }),
};
