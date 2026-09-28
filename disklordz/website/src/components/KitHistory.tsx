"use client";

import Link from "next/link";
import { useCallback, useEffect, useMemo, useState } from "react";

import type { KitManifest } from "@/lib/manifest";
import { STYLE_PRESETS } from "@/lib/presets";

type KitRow = {
  kit_id: string;
  prompt: string;
  preset_id: string;
  artist_lane: string;
  created_at: string;
  spec_mode: string | null;
  spec_engine: string | null;
  spec_key: string | null;
  spec_bpm: number | null;
  variation_batch_id: string | null;
  variation_label: string | null;
  manifest: KitManifest;
};

type Filters = {
  q: string;
  mode: string;
  engine: string;
  key: string;
  presetId: string;
  bpmMin: string;
  bpmMax: string;
  from: string;
  to: string;
};

const emptyFilters: Filters = {
  q: "",
  mode: "",
  engine: "",
  key: "",
  presetId: "",
  bpmMin: "",
  bpmMax: "",
  from: "",
  to: "",
};

export function KitHistory() {
  const [filters, setFilters] = useState<Filters>(emptyFilters);
  const [kits, setKits] = useState<KitRow[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [expandedId, setExpandedId] = useState<string | null>(null);
  const [batchLoading, setBatchLoading] = useState(false);
  const [batchSiblings, setBatchSiblings] = useState<KitRow[]>([]);

  const queryString = useMemo(() => {
    const p = new URLSearchParams();
    if (filters.q) p.set("q", filters.q);
    if (filters.mode) p.set("mode", filters.mode);
    if (filters.engine) p.set("engine", filters.engine);
    if (filters.key) p.set("key", filters.key);
    if (filters.presetId) p.set("preset_id", filters.presetId);
    if (filters.bpmMin) p.set("bpm_min", filters.bpmMin);
    if (filters.bpmMax) p.set("bpm_max", filters.bpmMax);
    if (filters.from) p.set("from", filters.from);
    if (filters.to) p.set("to", filters.to);
    p.set("limit", "50");
    return p.toString();
  }, [filters]);

  const load = useCallback(async () => {
    setLoading(true);
    setError(null);
    try {
      const res = await fetch(`/api/kits?${queryString}`);
      const data = await res.json();
      if (!res.ok) {
        setError(data.error ?? "Failed to load history");
        return;
      }
      setKits(data.kits ?? []);
    } catch {
      setError("Failed to load history");
    } finally {
      setLoading(false);
    }
  }, [queryString]);

  useEffect(() => {
    const t = setTimeout(load, 200);
    return () => clearTimeout(t);
  }, [load]);

  const expandKit = async (kit: KitRow) => {
    if (expandedId === kit.kit_id) {
      setExpandedId(null);
      setBatchSiblings([]);
      return;
    }
    setExpandedId(kit.kit_id);
    setBatchSiblings([]);
    if (!kit.variation_batch_id) {
      return;
    }
    setBatchLoading(true);
    try {
      const p = new URLSearchParams({ batch_id: kit.variation_batch_id, limit: "10" });
      const res = await fetch(`/api/kits?${p}`);
      const data = await res.json();
      if (res.ok) {
        setBatchSiblings(data.kits ?? []);
      }
    } finally {
      setBatchLoading(false);
    }
  };

  const downloadKit = async (manifest: KitManifest) => {
    const res = await fetch("/api/download", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ manifest }),
    });
    if (!res.ok) {
      return;
    }
    const blob = await res.blob();
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = `disklordz-${manifest.presetId}-${manifest.variationLabel ?? "kit"}.zip`;
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="space-y-4">
      <div className="rounded-2xl border border-zinc-800 bg-zinc-900/40 p-4 space-y-3">
        <h2 className="text-sm font-semibold text-zinc-200">Search history</h2>
        <input
          type="search"
          placeholder="Keyword in prompt…"
          className="w-full rounded-lg border border-zinc-700 bg-zinc-950 px-3 py-2 text-sm text-zinc-100"
          value={filters.q}
          onChange={(e) => setFilters((f) => ({ ...f, q: e.target.value }))}
        />
        <div className="grid gap-2 sm:grid-cols-2 lg:grid-cols-3">
          <select
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.mode}
            onChange={(e) => setFilters((f) => ({ ...f, mode: e.target.value }))}
          >
            <option value="">Any mode</option>
            <option value="one_shot">One shot</option>
            <option value="loop">Loop</option>
            <option value="sfx">SFX</option>
          </select>
          <select
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.engine}
            onChange={(e) => setFilters((f) => ({ ...f, engine: e.target.value }))}
          >
            <option value="">Any engine</option>
            <option value="studio">Studio</option>
            <option value="creative">Creative</option>
          </select>
          <select
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.presetId}
            onChange={(e) => setFilters((f) => ({ ...f, presetId: e.target.value }))}
          >
            <option value="">Any preset</option>
            {STYLE_PRESETS.map((p) => (
              <option key={p.id} value={p.id}>{p.label}</option>
            ))}
          </select>
          <input
            type="text"
            placeholder="Key (e.g. F minor)"
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.key}
            onChange={(e) => setFilters((f) => ({ ...f, key: e.target.value }))}
          />
          <input
            type="number"
            placeholder="BPM min"
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.bpmMin}
            onChange={(e) => setFilters((f) => ({ ...f, bpmMin: e.target.value }))}
          />
          <input
            type="number"
            placeholder="BPM max"
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.bpmMax}
            onChange={(e) => setFilters((f) => ({ ...f, bpmMax: e.target.value }))}
          />
          <input
            type="date"
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.from}
            onChange={(e) => setFilters((f) => ({ ...f, from: e.target.value }))}
          />
          <input
            type="date"
            className="rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-sm text-zinc-100"
            value={filters.to}
            onChange={(e) => setFilters((f) => ({ ...f, to: e.target.value }))}
          />
        </div>
        <button
          type="button"
          className="text-xs text-zinc-500 hover:text-zinc-300"
          onClick={() => setFilters(emptyFilters)}
        >
          Clear filters
        </button>
      </div>

      {loading && <p className="text-sm text-zinc-500">Loading…</p>}
      {error && <p className="text-sm text-red-300">{error}</p>}

      {!loading && kits.length === 0 && (
        <p className="rounded-xl border border-zinc-800 bg-zinc-900/40 px-4 py-6 text-sm text-zinc-400">
          No kits match. Generate while signed in — kits auto-save with searchable spec fields.
        </p>
      )}

      <ul className="space-y-2">
        {kits.map((kit) => (
          <li
            key={kit.kit_id}
            className="rounded-xl border border-zinc-800 bg-zinc-950 px-4 py-3"
          >
            <button
              type="button"
              className="w-full text-left"
              onClick={() => expandKit(kit)}
            >
              <p className="font-medium text-zinc-100">{kit.prompt}</p>
              <p className="mt-1 text-xs text-zinc-500">
                {kit.preset_id}
                {kit.spec_engine && ` · ${kit.spec_engine}`}
                {kit.spec_mode && ` · ${kit.spec_mode}`}
                {kit.spec_key && ` · ${kit.spec_key}`}
                {kit.spec_bpm != null && ` · ${kit.spec_bpm} BPM`}
                {kit.variation_label && ` · var ${kit.variation_label}`}
                {" · "}
                {new Date(kit.created_at).toLocaleString()}
              </p>
            </button>

            {expandedId === kit.kit_id && (
              <div className="mt-3 border-t border-zinc-800 pt-3 space-y-2">
                <p className="font-mono text-xs text-zinc-600">{kit.kit_id}</p>
                {kit.variation_batch_id && (
                  <p className="text-xs text-zinc-500">
                    Batch {kit.variation_batch_id.slice(0, 8)}…
                    {batchLoading ? " (loading variations…)" : ""}
                  </p>
                )}
                {batchSiblings.length > 1 && (
                  <ul className="text-xs text-zinc-400 space-y-1">
                    {batchSiblings.map((s) => (
                      <li key={s.kit_id}>
                        Variation {s.variation_label ?? "?"} —{" "}
                        <button
                          type="button"
                          className="text-emerald-400 hover:underline"
                          onClick={() => downloadKit(s.manifest)}
                        >
                          download ZIP
                        </button>
                      </li>
                    ))}
                  </ul>
                )}
                <div className="flex flex-wrap gap-2">
                  <button
                    type="button"
                    onClick={() => downloadKit(kit.manifest)}
                    className="rounded-lg border border-zinc-600 px-3 py-1.5 text-xs text-emerald-400 hover:border-emerald-500"
                  >
                    Download this kit
                  </button>
                  <Link
                    href="/"
                    className="rounded-lg border border-zinc-700 px-3 py-1.5 text-xs text-zinc-400 hover:text-zinc-200"
                  >
                    New kit from scratch
                  </Link>
                </div>
              </div>
            )}
          </li>
        ))}
      </ul>
    </div>
  );
}
