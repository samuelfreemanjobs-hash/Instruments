"use client";

import type { KitManifest } from "@/lib/manifest";

type Props = {
  variations: KitManifest[];
  selectedIndex: number;
  onSelect: (index: number) => void;
  playingKey: string | null;
  onPlay: (url: string, playKey: string) => void;
};

const AUDITION_SAMPLES = ["kick", "snare", "hat_closed"] as const;

export function VariationPreview({
  variations,
  selectedIndex,
  onSelect,
  playingKey,
  onPlay,
}: Props) {
  return (
    <div className="space-y-4">
      <div className="flex flex-wrap gap-2">
        {variations.map((v, i) => (
          <button
            key={v.kitId}
            type="button"
            onClick={() => onSelect(i)}
            className={`rounded-lg border px-4 py-2 text-sm font-semibold transition ${
              selectedIndex === i
                ? "border-emerald-500 bg-emerald-500/15 text-emerald-200"
                : "border-zinc-700 bg-zinc-950 text-zinc-300 hover:border-zinc-500"
            }`}
          >
            Variation {v.variationLabel ?? String.fromCharCode(65 + i)}
          </button>
        ))}
      </div>

      {variations[selectedIndex] && (
        <VariationDetail
          manifest={variations[selectedIndex]}
          playingKey={playingKey}
          onPlay={onPlay}
        />
      )}
    </div>
  );
}

function VariationDetail({
  manifest,
  playingKey,
  onPlay,
}: {
  manifest: KitManifest;
  playingKey: string | null;
  onPlay: (url: string, playKey: string) => void;
}) {
  const mode = manifest.generationSpec?.mode ?? "one_shot";
  const audition =
    mode === "loop"
      ? manifest.samples.filter((s) => s.name === "loop_main")
      : mode === "sfx"
        ? manifest.samples.filter((s) => s.name === "sfx")
        : manifest.samples.filter((s) =>
            AUDITION_SAMPLES.includes(s.name as (typeof AUDITION_SAMPLES)[number]),
          );

  const auditionLabel =
    mode === "loop"
      ? "Quick audition (loop)"
      : mode === "sfx"
        ? "Quick audition (SFX)"
        : "Quick audition (kick / snare / hat)";

  return (
    <div className="space-y-3 rounded-xl border border-zinc-800 bg-zinc-950/80 p-4">
      <p className="font-mono text-xs text-zinc-500">{manifest.kitId}</p>
      <p className="text-sm text-zinc-400">{auditionLabel}</p>
      <ul className="flex flex-wrap gap-2">
        {audition.map((s) => {
          const key = `${manifest.kitId}-${s.name}`;
          return (
            <li key={s.name}>
              <button
                type="button"
                onClick={() => onPlay(s.url, key)}
                className="rounded-lg border border-zinc-700 px-3 py-2 text-sm text-emerald-400 hover:border-emerald-600"
              >
                {playingKey === key ? "Playing…" : `Play ${s.name}`}
              </button>
            </li>
          );
        })}
      </ul>
      <ul className="grid gap-2 sm:grid-cols-2">
        {manifest.samples.map((s) => {
          const key = `${manifest.kitId}-${s.name}`;
          return (
            <li
              key={s.name}
              className="flex items-center justify-between rounded-lg border border-zinc-800 px-3 py-2"
            >
              <span className="font-mono text-sm text-zinc-200">{s.name}</span>
              <button
                type="button"
                onClick={() => onPlay(s.url, key)}
                className="text-xs font-medium text-zinc-400 hover:text-emerald-300"
              >
                {playingKey === key ? "…" : "Play"}
              </button>
            </li>
          );
        })}
      </ul>
    </div>
  );
}
