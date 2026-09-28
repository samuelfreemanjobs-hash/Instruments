export type HistoryFilters = {
  q?: string;
  mode?: string;
  engine?: string;
  key?: string;
  presetId?: string;
  batchId?: string;
  bpmMin?: number;
  bpmMax?: number;
  from?: string;
  to?: string;
  limit: number;
};

export function parseHistoryFilters(searchParams: URLSearchParams): HistoryFilters {
  const limitRaw = Number(searchParams.get("limit") ?? "50");
  const limit = Number.isFinite(limitRaw) ? Math.min(100, Math.max(1, limitRaw)) : 50;

  const bpmMin = searchParams.get("bpm_min");
  const bpmMax = searchParams.get("bpm_max");

  return {
    q: searchParams.get("q")?.trim() || undefined,
    mode: searchParams.get("mode")?.trim() || undefined,
    engine: searchParams.get("engine")?.trim() || undefined,
    key: searchParams.get("key")?.trim() || undefined,
    presetId: searchParams.get("preset_id")?.trim() || undefined,
    batchId: searchParams.get("batch_id")?.trim() || undefined,
    bpmMin: bpmMin ? Number(bpmMin) : undefined,
    bpmMax: bpmMax ? Number(bpmMax) : undefined,
    from: searchParams.get("from")?.trim() || undefined,
    to: searchParams.get("to")?.trim() || undefined,
    limit,
  };
}
