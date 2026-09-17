"use client";

import { useCallback, useEffect, useMemo, useRef, useState } from "react";

import { GenerationSpecFields } from "@/components/GenerationSpecFields";
import { VariationPreview } from "@/components/VariationPreview";
import {
  defaultGenerationSpec,
  type GenerationSpec,
} from "@/lib/generation/generation-spec";
import { exportKitToFolder, supportsDirectoryPicker } from "@/lib/daw-folder-export";
import { creditCostForSpec, PRODUCT_PACK_CREDIT_COST } from "@/lib/generation/mode-utils";
import type { KitManifest, ProductPackManifest } from "@/lib/manifest";
import { STYLE_PRESETS, type StylePreset } from "@/lib/presets";

type FrozenRequest = {
  prompt: string;
  presetId: string;
  spec: GenerationSpec;
};

type GenerateResponse = {
  batchId: string;
  variationCount: number;
  storageBackend?: "local" | "supabase";
  variations: { label: string; manifest: KitManifest }[];
  manifest?: KitManifest;
  savedToAccount?: boolean;
  rateLimit?: { remaining: number; limit: number };
  billing?: {
    plan: string;
    creditsBalance: number;
    unlimited: boolean;
    generationCreditCost: number;
  };
};

export function KitGenerator() {
  const [prompt, setPrompt] = useState("dirty 90s boom bap kick with tape grit");
  const [presetId, setPresetId] = useState<string>(STYLE_PRESETS[0].id);
  const [spec, setSpec] = useState<GenerationSpec>(() =>
    defaultGenerationSpec(STYLE_PRESETS[0].id),
  );
  const [specOpen, setSpecOpen] = useState(false);
  const [variations, setVariations] = useState<KitManifest[]>([]);
  const [batchId, setBatchId] = useState<string | null>(null);
  const [storageBackend, setStorageBackend] = useState<string | null>(null);
  const [selectedVariation, setSelectedVariation] = useState(0);
  const [frozen, setFrozen] = useState<FrozenRequest | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [playingKey, setPlayingKey] = useState<string | null>(null);
  const [savedToAccount, setSavedToAccount] = useState(false);
  const [remaining, setRemaining] = useState<number | null>(null);
  const [dailyLimit, setDailyLimit] = useState(20);
  const [billing, setBilling] = useState<{
    plan: string;
    creditsBalance: number;
    unlimited: boolean;
  } | null>(null);
  const [ideaLoading, setIdeaLoading] = useState(false);
  const [ragNote, setRagNote] = useState<string | null>(null);
  const [productPack, setProductPack] = useState<ProductPackManifest | null>(null);
  const [packLoading, setPackLoading] = useState(false);
  const audioRef = useRef<HTMLAudioElement | null>(null);

  useEffect(() => {
    fetch("/api/credits")
      .then((r) => r.json())
      .then((d) => {
        if (d.authenticated && d.plan) {
          setBilling({
            plan: d.plan,
            creditsBalance: d.creditsBalance ?? 0,
            unlimited: Boolean(d.unlimited),
          });
          return;
        }
        setBilling(null);
      })
      .catch(() => undefined);

    fetch("/api/rate-limit")
      .then((r) => r.json())
      .then((d) => {
        if (typeof d.remaining === "number") setRemaining(d.remaining);
        if (typeof d.limit === "number") setDailyLimit(d.limit);
      })
      .catch(() => undefined);
  }, []);

  const selectedPreset = useMemo(
    () => STYLE_PRESETS.find((p) => p.id === presetId),
    [presetId],
  );

  const activeManifest = variations[selectedVariation] ?? null;

  const selectPreset = useCallback((id: string) => {
    setPresetId(id);
    setSpec(defaultGenerationSpec(id));
  }, []);

  const applyRagSuggestion = useCallback(
    async (mode: "random" | "suggest") => {
      if (mode === "suggest" && prompt.trim().length < 3) {
        setError("Type at least a few words, then Enhance.");
        return;
      }
      setIdeaLoading(true);
      setError(null);
      setRagNote(null);
      try {
        const res = await fetch("/api/rag/suggest", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ mode, presetId, query: prompt }),
        });
        const data = await res.json();
        if (!res.ok) {
          setError(data.error ?? "Could not fetch idea");
          return;
        }
        if (data.prompt) {
          setPrompt(data.prompt);
        }
        if (data.presetId && data.presetId !== presetId) {
          setPresetId(data.presetId);
        }
        if (data.spec) {
          setSpec(data.spec as GenerationSpec);
        } else if (data.presetId) {
          setSpec(defaultGenerationSpec(data.presetId));
        }
        if (data.snippets?.[0]?.text) {
          setRagNote(data.snippets[0].text);
        }
      } catch {
        setError("Prompt assistant unavailable.");
      } finally {
        setIdeaLoading(false);
      }
    },
    [prompt, presetId],
  );

  const runGenerate = useCallback(async (request: FrozenRequest) => {
    setLoading(true);
    setError(null);
    setVariations([]);
    setBatchId(null);
    setSelectedVariation(0);
    setSavedToAccount(false);
    try {
      const res = await fetch("/api/generate", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          prompt: request.prompt,
          presetId: request.presetId,
          spec: request.spec,
        }),
      });
      const data = await res.json();
      if (!res.ok) {
        if (res.status === 402) {
          setError(
            (data.message as string) ??
              "Out of credits — upgrade to Pro on your account page.",
          );
          if (typeof data.creditsBalance === "number") {
            setBilling({
              plan: (data.plan as string) ?? "free",
              creditsBalance: data.creditsBalance,
              unlimited: false,
            });
          }
          return;
        }
        setError(data.message ?? data.error ?? "Generation failed");
        return;
      }
      const payload = data as GenerateResponse;
      const manifests =
        payload.variations?.map((v) => v.manifest) ??
        (payload.manifest ? [payload.manifest] : []);
      if (!manifests.length) {
        setError("No variations returned.");
        return;
      }
      setVariations(manifests);
      setBatchId(payload.batchId ?? null);
      setStorageBackend(payload.storageBackend ?? manifests[0]?.storageBackend ?? "local");
      setFrozen(request);
      setSavedToAccount(Boolean(payload.savedToAccount));
      if (payload.rateLimit) {
        setRemaining(payload.rateLimit.remaining);
        setDailyLimit(payload.rateLimit.limit);
      }
      if (payload.billing) {
        setBilling({
          plan: payload.billing.plan,
          creditsBalance: payload.billing.creditsBalance,
          unlimited: payload.billing.unlimited,
        });
      }
    } catch {
      setError("Network error — try again.");
    } finally {
      setLoading(false);
    }
  }, []);

  const generate = useCallback(() => {
    runGenerate({ prompt, presetId, spec });
  }, [prompt, presetId, spec, runGenerate]);

  const generateMore = useCallback(() => {
    if (!frozen) {
      setError("Generate a batch first to lock the spec for “Generate more”.");
      return;
    }
    runGenerate(frozen);
  }, [frozen, runGenerate]);

  const playSample = useCallback((url: string, playKey: string) => {
    if (audioRef.current) {
      audioRef.current.pause();
    }
    const audio = new Audio(url);
    audioRef.current = audio;
    setPlayingKey(playKey);
    audio.play().catch(() => setPlayingKey(null));
    audio.onended = () => setPlayingKey(null);
  }, []);

  const runProductPack = useCallback(async () => {
    setPackLoading(true);
    setError(null);
    setProductPack(null);
    try {
      const res = await fetch("/api/factory/batch", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ prompt, presetId, spec }),
      });
      const data = await res.json();
      if (!res.ok) {
        setError(data.message ?? data.error ?? "Product factory failed");
        return;
      }
      setProductPack(data.productPack as ProductPackManifest);
      if (data.billing) {
        setBilling({
          plan: data.billing.plan,
          creditsBalance: data.billing.creditsBalance,
          unlimited: data.billing.unlimited,
        });
      }
    } catch {
      setError("Product factory network error.");
    } finally {
      setPackLoading(false);
    }
  }, [prompt, presetId, spec]);

  const downloadProductPack = useCallback(async () => {
    if (!productPack) return;
    setPackLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/factory/download", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ productPack }),
      });
      if (!res.ok) {
        const data = await res.json();
        setError(data.error ?? "Pack download failed");
        return;
      }
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = `disklordz-pack-${productPack.presetId}.zip`;
      a.click();
      URL.revokeObjectURL(url);
    } catch {
      setError("Pack download failed.");
    } finally {
      setPackLoading(false);
    }
  }, [productPack]);

  const saveToFolder = useCallback(async () => {
    if (!activeManifest) return;
    if (!supportsDirectoryPicker()) {
      setError("Save to folder needs Chrome or Edge. Use ZIP download or the DAW inbox watcher.");
      return;
    }
    setLoading(true);
    setError(null);
    try {
      await exportKitToFolder(activeManifest);
    } catch {
      setError("Could not write to folder (permission denied or network).");
    } finally {
      setLoading(false);
    }
  }, [activeManifest]);

  const downloadZip = useCallback(async () => {
    if (!activeManifest) return;
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/download", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ manifest: activeManifest }),
      });
      if (!res.ok) {
        const data = await res.json();
        setError(data.error ?? "Download failed");
        return;
      }
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      const label = activeManifest.variationLabel ?? "A";
      a.download = `disklordz-${activeManifest.presetId}-var-${label}.zip`;
      a.click();
      URL.revokeObjectURL(url);
    } catch {
      setError("Download failed.");
    } finally {
      setLoading(false);
    }
  }, [activeManifest]);

  const variationCountHint =
    spec.engine === "creative" ? "3 variations" : "2 variations";

  return (
    <div className="mx-auto flex w-full max-w-3xl flex-col gap-8">
      <header className="space-y-2">
        <p className="text-sm font-medium uppercase tracking-widest text-emerald-400/90">
          Disklordz Supply · v0
        </p>
        <h1 className="text-3xl font-semibold tracking-tight text-zinc-50 sm:text-4xl">
          Drum kit from a vibe
        </h1>
        <p className="max-w-xl text-zinc-400">
          Type a prompt, pick a lane preset, audition {variationCountHint} per engine, download the
          kit you keep.{" "}
          <a href="/daw-inbox" className="text-emerald-400 hover:underline">DAW inbox</a> for
          folder export or a desktop watcher.
        </p>
        {billing?.unlimited ? (
          <p className="text-xs text-emerald-500/90">Pro — unlimited generations</p>
        ) : billing ? (
          <p className="text-xs text-zinc-500">
            Signed in: {billing.creditsBalance} credits left (
            {creditCostForSpec(spec)} per kit batch, {PRODUCT_PACK_CREDIT_COST} for product pack).{" "}
            <a href="/account" className="text-emerald-400 hover:underline">Upgrade</a>
          </p>
        ) : (
          remaining !== null && (
            <p className="text-xs text-zinc-500">
              Guest: {remaining} of {dailyLimit} batches left today (per IP).{" "}
              <a href="/login" className="text-emerald-400 hover:underline">Sign in</a> for credits.
            </p>
          )
        )}
      </header>

      <section className="space-y-3 rounded-2xl border border-zinc-800 bg-zinc-900/60 p-5">
        <label className="block text-sm font-medium text-zinc-300" htmlFor="prompt">
          Your prompt
        </label>
        <textarea
          id="prompt"
          rows={3}
          className="w-full rounded-xl border border-zinc-700 bg-zinc-950 px-3 py-2 text-zinc-100 placeholder:text-zinc-600 focus:border-emerald-500 focus:outline-none focus:ring-1 focus:ring-emerald-500"
          value={prompt}
          onChange={(e) => setPrompt(e.target.value)}
          placeholder="e.g. smoky Memphis rim with dusty 808"
        />

        <div className="flex flex-wrap gap-2">
          <button
            type="button"
            disabled={ideaLoading || loading}
            onClick={() => applyRagSuggestion("random")}
            className="rounded-lg border border-zinc-600 px-3 py-2 text-sm text-zinc-200 hover:border-emerald-500 disabled:opacity-50"
            title="Random lane-aligned prompt"
          >
            {ideaLoading ? "…" : "🎲 Random idea"}
          </button>
          <button
            type="button"
            disabled={ideaLoading || loading}
            onClick={() => applyRagSuggestion("suggest")}
            className="rounded-lg border border-zinc-600 px-3 py-2 text-sm text-zinc-200 hover:border-emerald-500 disabled:opacity-50"
          >
            Enhance prompt
          </button>
        </div>
        {ragNote && (
          <p className="text-xs text-zinc-500">
            Lane context: {ragNote}
          </p>
        )}

        <p className="text-sm font-medium text-zinc-300">Style preset</p>
        <div className="grid gap-2 sm:grid-cols-2">
          {STYLE_PRESETS.map((preset: StylePreset) => (
            <button
              key={preset.id}
              type="button"
              onClick={() => selectPreset(preset.id)}
              className={`rounded-xl border px-3 py-3 text-left transition ${
                presetId === preset.id
                  ? "border-emerald-500/80 bg-emerald-500/10"
                  : "border-zinc-800 bg-zinc-950 hover:border-zinc-600"
              }`}
            >
              <span className="block font-medium text-zinc-100">{preset.label}</span>
              <span className="mt-1 block text-xs text-zinc-500">
                {preset.artistLane} · {preset.bpmHint} BPM
              </span>
              <span className="mt-2 block text-xs text-zinc-400">{preset.description}</span>
            </button>
          ))}
        </div>

        {selectedPreset && (
          <p className="text-xs text-zinc-500">
            Tags: {selectedPreset.tags.join(" · ")}
          </p>
        )}

        <button
          type="button"
          className="text-sm font-medium text-emerald-400/90 hover:text-emerald-300"
          onClick={() => setSpecOpen((o) => !o)}
        >
          {specOpen ? "Hide generation spec" : "Generation spec (key, BPM, engine…)"}
        </button>
        {specOpen && (
          <GenerationSpecFields spec={spec} onChange={setSpec} disabled={loading} />
        )}

        <button
          type="button"
          disabled={loading}
          onClick={generate}
          className="mt-2 w-full rounded-xl bg-emerald-500 px-4 py-3 font-semibold text-zinc-950 transition hover:bg-emerald-400 disabled:opacity-50"
        >
          {loading ? "Generating…" : `Generate ${variationCountHint}`}
        </button>

        {error && (
          <p className="rounded-lg border border-red-900/50 bg-red-950/40 px-3 py-2 text-sm text-red-200">
            {error}
          </p>
        )}
      </section>

      {variations.length > 0 && (
        <section className="space-y-4 rounded-2xl border border-zinc-800 bg-zinc-900/40 p-5">
          <div className="flex flex-wrap items-center justify-between gap-2">
            <h2 className="text-lg font-semibold text-zinc-100">Candidates</h2>
            <div className="flex flex-col items-end gap-1 text-xs text-zinc-500">
              {batchId && <span className="font-mono">batch {batchId.slice(0, 8)}</span>}
              {savedToAccount && (
                <span className="text-emerald-400">Saved to your account</span>
              )}
            </div>
          </div>

          <VariationPreview
            variations={variations}
            selectedIndex={selectedVariation}
            onSelect={setSelectedVariation}
            playingKey={playingKey}
            onPlay={playSample}
          />

          {activeManifest?.generationSpec && (
            <p className="font-mono text-xs text-zinc-500">
              Frozen spec: {activeManifest.generationSpec.engine} ·{" "}
              {activeManifest.generationSpec.key} · {activeManifest.generationSpec.bpm} BPM
            </p>
          )}

          <div className="flex flex-col gap-2 sm:flex-row sm:flex-wrap">
            <button
              type="button"
              disabled={loading || !frozen}
              onClick={generateMore}
              className="min-w-[10rem] flex-1 rounded-xl border border-zinc-600 px-4 py-3 font-semibold text-zinc-100 transition hover:border-emerald-500 hover:text-emerald-300 disabled:opacity-50"
            >
              Generate more (same spec)
            </button>
            <button
              type="button"
              disabled={loading || !activeManifest}
              onClick={downloadZip}
              className="min-w-[10rem] flex-1 rounded-xl border border-emerald-600/60 bg-emerald-500/10 px-4 py-3 font-semibold text-emerald-200 transition hover:bg-emerald-500/20 disabled:opacity-50"
            >
              Download ZIP (variation {activeManifest?.variationLabel ?? "A"})
            </button>
            <button
              type="button"
              disabled={loading || !activeManifest}
              onClick={saveToFolder}
              className="min-w-[10rem] flex-1 rounded-xl border border-zinc-600 px-4 py-3 font-semibold text-zinc-100 transition hover:border-emerald-500 disabled:opacity-50"
              title="Chrome / Edge — pick a DAW samples folder"
            >
              Save WAVs to folder
            </button>
          </div>

          <p className="text-xs text-zinc-500">
            Storage: {storageBackend ?? "local"} · studio/creative engines · SHA-256 in manifest.json
          </p>
        </section>
      )}

      <section className="space-y-3 rounded-2xl border border-zinc-800 bg-zinc-900/40 p-5">
        <h2 className="text-lg font-semibold text-zinc-100">Product factory</h2>
        <p className="text-sm text-zinc-400">
          Turn the same brief into a storefront SKU layout ({PRODUCT_PACK_CREDIT_COST} credits):{" "}
          <span className="font-mono text-zinc-500">01_KICKS … 04_PERC</span> with{" "}
          {productPack?.samples.length ?? 17} WAVs.
        </p>
        <div className="flex flex-col gap-2 sm:flex-row">
          <button
            type="button"
            disabled={packLoading || loading}
            onClick={runProductPack}
            className="flex-1 rounded-xl border border-zinc-600 px-4 py-3 font-semibold text-zinc-100 transition hover:border-emerald-500 disabled:opacity-50"
          >
            {packLoading ? "Building pack…" : `Build product pack (${PRODUCT_PACK_CREDIT_COST} cr)`}
          </button>
          <button
            type="button"
            disabled={packLoading || !productPack}
            onClick={downloadProductPack}
            className="flex-1 rounded-xl border border-emerald-600/60 bg-emerald-500/10 px-4 py-3 font-semibold text-emerald-200 transition hover:bg-emerald-500/20 disabled:opacity-50"
          >
            Download pack ZIP
          </button>
        </div>
        {productPack && (
          <p className="font-mono text-xs text-zinc-500">
            pack {productPack.packId.slice(0, 8)} · {productPack.folders.map((f) => f.id).join(" · ")}
          </p>
        )}
      </section>
    </div>
  );
}
