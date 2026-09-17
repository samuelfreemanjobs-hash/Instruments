"use client";

import { useCallback, useEffect, useMemo, useRef, useState } from "react";

import { GenerationSpecFields } from "@/components/GenerationSpecFields";
import { defaultGenerationSpec, type GenerationSpec } from "@/lib/generation/generation-spec";
import type { KitManifest } from "@/lib/manifest";
import { STYLE_PRESETS, type StylePreset } from "@/lib/presets";

type GenerateResponse = {
  manifest: KitManifest;
  savedToAccount?: boolean;
  rateLimit?: { remaining: number; limit: number };
};

export function KitGenerator() {
  const [prompt, setPrompt] = useState("dirty 90s boom bap kick with tape grit");
  const [presetId, setPresetId] = useState<string>(STYLE_PRESETS[0].id);
  const [spec, setSpec] = useState<GenerationSpec>(() =>
    defaultGenerationSpec(STYLE_PRESETS[0].id),
  );
  const [specOpen, setSpecOpen] = useState(false);
  const [manifest, setManifest] = useState<KitManifest | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [playing, setPlaying] = useState<string | null>(null);
  const [savedToAccount, setSavedToAccount] = useState(false);
  const [remaining, setRemaining] = useState<number | null>(null);
  const [dailyLimit, setDailyLimit] = useState(20);
  const audioRef = useRef<HTMLAudioElement | null>(null);

  useEffect(() => {
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

  const selectPreset = useCallback((id: string) => {
    setPresetId(id);
    setSpec(defaultGenerationSpec(id));
  }, []);

  const generate = useCallback(async () => {
    setLoading(true);
    setError(null);
    setManifest(null);
    setSavedToAccount(false);
    try {
      const res = await fetch("/api/generate", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ prompt, presetId, spec }),
      });
      const data = await res.json();
      if (!res.ok) {
        setError(data.message ?? data.error ?? "Generation failed");
        return;
      }
      const payload = data as GenerateResponse;
      setManifest(payload.manifest);
      setSavedToAccount(Boolean(payload.savedToAccount));
      if (payload.rateLimit) {
        setRemaining(payload.rateLimit.remaining);
        setDailyLimit(payload.rateLimit.limit);
      }
    } catch {
      setError("Network error — try again.");
    } finally {
      setLoading(false);
    }
  }, [prompt, presetId, spec]);

  const playSample = useCallback((url: string, name: string) => {
    if (audioRef.current) {
      audioRef.current.pause();
    }
    const audio = new Audio(url);
    audioRef.current = audio;
    setPlaying(name);
    audio.play().catch(() => setPlaying(null));
    audio.onended = () => setPlaying(null);
  }, []);

  const downloadZip = useCallback(async () => {
    if (!manifest) return;
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/download", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ manifest }),
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
      a.download = `disklordz-${manifest.presetId}.zip`;
      a.click();
      URL.revokeObjectURL(url);
    } catch {
      setError("Download failed.");
    } finally {
      setLoading(false);
    }
  }, [manifest]);

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
          Type a prompt, pick an artist lane preset, preview one-shots, download an MPC-ready ZIP
          with provenance manifest.
        </p>
        {remaining !== null && (
          <p className="text-xs text-zinc-500">
            Free tier: {remaining} of {dailyLimit} kits left today (per IP).
          </p>
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
          {loading ? "Generating…" : "Generate preview kit"}
        </button>

        {error && (
          <p className="rounded-lg border border-red-900/50 bg-red-950/40 px-3 py-2 text-sm text-red-200">
            {error}
          </p>
        )}
      </section>

      {manifest && (
        <section className="space-y-4 rounded-2xl border border-zinc-800 bg-zinc-900/40 p-5">
          <div className="flex flex-wrap items-center justify-between gap-2">
            <h2 className="text-lg font-semibold text-zinc-100">Preview</h2>
            <div className="flex flex-col items-end gap-1">
              <span className="font-mono text-xs text-zinc-500">{manifest.kitId}</span>
              {savedToAccount && (
                <span className="text-xs text-emerald-400">Saved to your account</span>
              )}
            </div>
          </div>
          <ul className="grid gap-2 sm:grid-cols-2">
            {manifest.samples.map((s) => (
              <li
                key={s.name}
                className="flex items-center justify-between rounded-lg border border-zinc-800 bg-zinc-950 px-3 py-2"
              >
                <span className="font-mono text-sm text-zinc-200">{s.name}</span>
                <button
                  type="button"
                  onClick={() => playSample(s.url, s.name)}
                  className="text-sm font-medium text-emerald-400 hover:text-emerald-300"
                >
                  {playing === s.name ? "Playing…" : "Play"}
                </button>
              </li>
            ))}
          </ul>
          {manifest.generationSpec && (
            <p className="font-mono text-xs text-zinc-500">
              Spec: {manifest.generationSpec.mode} · {manifest.generationSpec.engine} ·{" "}
              {manifest.generationSpec.key} · {manifest.generationSpec.bpm} BPM · wildness{" "}
              {Math.round(manifest.generationSpec.wildness * 100)}%
            </p>
          )}
          <p className="text-xs text-zinc-500">
            Provenance: {manifest.samples[0]?.provenance} · SHA-256 per file in manifest.json
          </p>
          <button
            type="button"
            disabled={loading}
            onClick={downloadZip}
            className="w-full rounded-xl border border-zinc-600 px-4 py-3 font-semibold text-zinc-100 transition hover:border-emerald-500 hover:text-emerald-300 disabled:opacity-50"
          >
            Download ZIP (WAV + manifest)
          </button>
        </section>
      )}
    </div>
  );
}
