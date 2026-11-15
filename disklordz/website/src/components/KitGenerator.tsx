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

const FEATURED_PRESET_IDS = [
  "midnight-circuit",
  "disklordz-screw",
  "mpc-ready-808",
  "terminal-mirage",
  "boulevard-86",
] as const;

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
  const [laneQuery, setLaneQuery] = useState("");
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
    const current = audioRef.current;
    if (current && playingKey === playKey && !current.paused) {
      current.pause();
      setPlayingKey(null);
      return;
    }
    if (current) current.pause();
    const audio = new Audio(url);
    audioRef.current = audio;
    setPlayingKey(playKey);
    audio.play().catch(() => setPlayingKey(null));
    audio.onended = () => setPlayingKey(null);
  }, [playingKey]);

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
  const featuredPresets = FEATURED_PRESET_IDS.flatMap((id) => {
    const preset = STYLE_PRESETS.find((item) => item.id === id);
    return preset ? [preset] : [];
  });
  const laneNeedle = laneQuery.trim().toLowerCase();
  const laneMatches =
    laneNeedle.length < 2
      ? []
      : STYLE_PRESETS.filter((preset) => {
          if ((FEATURED_PRESET_IDS as readonly string[]).includes(preset.id)) return false;
          const haystack = `${preset.label} ${preset.description} ${preset.tags.join(" ")}`.toLowerCase();
          return haystack.includes(laneNeedle);
        }).slice(0, 8);
  const selectedOffList =
    selectedPreset &&
    !featuredPresets.some((preset) => preset.id === selectedPreset.id) &&
    !laneMatches.some((preset) => preset.id === selectedPreset.id)
      ? selectedPreset
      : null;

  const visiblePresets = [
    ...(selectedOffList ? [selectedOffList] : []),
    ...(laneNeedle.length < 2 ? featuredPresets : laneMatches),
  ];

  return (
    <div className="mx-auto flex w-full max-w-5xl flex-col gap-8">
      <header className="space-y-2">
        <p className="font-condensed text-sm tracking-[0.22em] text-gold uppercase">Drum Factory</p>
        <h1 className="font-display text-4xl text-ivory sm:text-5xl">Original one-shots from a prompt</h1>
        <p className="max-w-2xl text-ivory/75">
          Pick a lane, audition {variationCountHint}, and download the WAV zip you keep.{" "}
          <a href="/daw-inbox" className="text-gold underline">
            DAW inbox
          </a>{" "}
          covers folder export.
        </p>
        {billing?.unlimited ? (
          <p className="font-mono text-xs text-gold">Pro — unlimited generations</p>
        ) : billing ? (
          <p className="font-mono text-xs text-ivory/60">
            Signed in: {billing.creditsBalance} credits left (
            {creditCostForSpec(spec)} per kit batch, {PRODUCT_PACK_CREDIT_COST} for product pack).{" "}
            <a href="/account" className="text-gold underline">
              Account
            </a>
          </p>
        ) : (
          remaining !== null && (
            <p className="font-mono text-xs text-ivory/60">
              Guest: {remaining} of {dailyLimit} batches left today.{" "}
              <a href="/login" className="text-gold underline">
                Sign in
              </a>{" "}
              for credits.
            </p>
          )
        )}
      </header>

      <section className="space-y-4 border border-gold/30 bg-charcoal p-5">
        <label className="block font-condensed text-sm tracking-[0.14em] text-gold uppercase" htmlFor="prompt">
          Prompt
        </label>
        <textarea
          id="prompt"
          rows={3}
          className="w-full border border-gold/30 bg-obsidian px-3 py-2 text-ivory placeholder:text-ivory/40"
          value={prompt}
          onChange={(e) => setPrompt(e.target.value)}
          placeholder="smoky Memphis rim with a long 808"
        />

        <div className="flex flex-wrap gap-2">
          <button
            type="button"
            disabled={ideaLoading || loading}
            onClick={() => applyRagSuggestion("random")}
            className="min-h-11 border border-gold/40 px-3 font-condensed text-sm font-semibold tracking-wide text-ivory uppercase disabled:opacity-50"
          >
            {ideaLoading ? "…" : "Random idea"}
          </button>
          <button
            type="button"
            disabled={ideaLoading || loading}
            onClick={() => applyRagSuggestion("suggest")}
            className="min-h-11 border border-gold/40 px-3 font-condensed text-sm font-semibold tracking-wide text-ivory uppercase disabled:opacity-50"
          >
            Enhance prompt
          </button>
        </div>
        {ragNote && <p className="text-xs text-ivory/55">Lane context: {ragNote}</p>}

        <div className="flex flex-col gap-2 sm:flex-row sm:items-end sm:justify-between">
          <p className="font-condensed text-sm tracking-[0.14em] text-gold uppercase">Lanes</p>
          <label className="block text-sm text-ivory/70">
            <span className="sr-only">Search lanes</span>
            <input
              type="search"
              value={laneQuery}
              onChange={(e) => setLaneQuery(e.target.value)}
              placeholder="Search other lanes"
              className="w-full border border-gold/30 bg-obsidian px-3 py-2 text-ivory placeholder:text-ivory/40 sm:w-64"
            />
          </label>
        </div>
        <div className="grid gap-2 sm:grid-cols-2">
          {visiblePresets.map((preset: StylePreset) => (
            <button
              key={preset.id}
              type="button"
              onClick={() => selectPreset(preset.id)}
              className={`border px-3 py-3 text-left ${
                presetId === preset.id
                  ? "border-gold bg-burgundy/50"
                  : "border-gold/25 bg-obsidian hover:border-gold/60"
              }`}
            >
              <span className="block font-condensed text-sm font-semibold tracking-wide text-ivory uppercase">
                {preset.label}
              </span>
              <span className="mt-1 block font-mono text-xs text-gold">{preset.bpmHint} BPM</span>
              <span className="mt-2 block text-xs text-ivory/70">{preset.description}</span>
            </button>
          ))}
        </div>
        {laneNeedle.length >= 2 && laneMatches.length === 0 && (
          <p className="text-sm text-ivory/60">No lane matches that search.</p>
        )}

        {selectedPreset && (
          <p className="text-xs text-ivory/55">Tags: {selectedPreset.tags.join(" · ")}</p>
        )}

        <button
          type="button"
          className="min-h-11 text-left font-condensed text-sm font-semibold tracking-wide text-gold uppercase"
          onClick={() => setSpecOpen((o) => !o)}
        >
          {specOpen ? "Hide key, BPM, and engine" : "Key, BPM, and engine"}
        </button>
        {specOpen && (
          <GenerationSpecFields spec={spec} onChange={setSpec} disabled={loading} />
        )}

        <button
          type="button"
          disabled={loading}
          onClick={generate}
          className="mt-2 min-h-11 w-full bg-gold px-4 font-condensed text-base font-semibold tracking-wide text-obsidian uppercase disabled:opacity-50"
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
        <section className="space-y-4 border border-gold/30 bg-charcoal p-5">
          <div className="flex flex-wrap items-center justify-between gap-2">
            <h2 className="font-display text-2xl text-ivory">Candidates</h2>
            <div className="flex flex-col items-end gap-1 font-mono text-xs text-ivory/55">
              {batchId && <span>batch {batchId.slice(0, 8)}</span>}
              {savedToAccount && <span className="text-gold">Saved to your account</span>}
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
            <p className="font-mono text-xs text-ivory/55">
              Frozen spec: {activeManifest.generationSpec.engine} ·{" "}
              {activeManifest.generationSpec.key} · {activeManifest.generationSpec.bpm} BPM
            </p>
          )}

          <div className="flex flex-col gap-2 sm:flex-row sm:flex-wrap">
            <button
              type="button"
              disabled={loading || !frozen}
              onClick={generateMore}
              className="min-h-11 min-w-[10rem] flex-1 border border-gold/40 px-4 font-condensed font-semibold tracking-wide text-ivory uppercase disabled:opacity-50"
            >
              Generate more (same spec)
            </button>
            <button
              type="button"
              disabled={loading || !activeManifest}
              onClick={downloadZip}
              className="min-h-11 min-w-[10rem] flex-1 bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase disabled:opacity-50"
            >
              Download ZIP (variation {activeManifest?.variationLabel ?? "A"})
            </button>
            <button
              type="button"
              disabled={loading || !activeManifest}
              onClick={saveToFolder}
              className="min-h-11 min-w-[10rem] flex-1 border border-gold/40 px-4 font-condensed font-semibold tracking-wide text-ivory uppercase disabled:opacity-50"
              title="Chrome / Edge — pick a DAW samples folder"
            >
              Save WAVs to folder
            </button>
          </div>

          <p className="font-mono text-xs text-ivory/55">
            Storage: {storageBackend ?? "local"} · SHA-256 in manifest.json
          </p>
        </section>
      )}

      <section className="space-y-3 border border-gold/20 bg-charcoal p-5">
        <h2 className="font-display text-2xl text-ivory">Product pack</h2>
        <p className="text-sm text-ivory/75">
          Same brief, folder layout ({PRODUCT_PACK_CREDIT_COST} credits):{" "}
          <span className="font-mono text-gold">01_KICKS … 04_PERC</span> with{" "}
          {productPack?.samples.length ?? 17} WAVs. This is a generated pack, not a catalog SKU.
        </p>
        <div className="flex flex-col gap-2 sm:flex-row">
          <button
            type="button"
            disabled={packLoading || loading}
            onClick={runProductPack}
            className="min-h-11 flex-1 border border-gold/40 px-4 font-condensed font-semibold tracking-wide text-ivory uppercase disabled:opacity-50"
          >
            {packLoading ? "Building pack…" : `Build product pack (${PRODUCT_PACK_CREDIT_COST} cr)`}
          </button>
          <button
            type="button"
            disabled={packLoading || !productPack}
            onClick={downloadProductPack}
            className="min-h-11 flex-1 bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase disabled:opacity-50"
          >
            Download pack ZIP
          </button>
        </div>
        {productPack && (
          <p className="font-mono text-xs text-ivory/55">
            pack {productPack.packId.slice(0, 8)} · {productPack.folders.map((f) => f.id).join(" · ")}
          </p>
        )}
      </section>
    </div>
  );
}
