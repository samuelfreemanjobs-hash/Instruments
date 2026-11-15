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
            className={`min-h-11 border px-4 font-condensed text-sm font-semibold tracking-wide uppercase ${
              selectedIndex === i
                ? "border-gold bg-burgundy/50 text-ivory"
                : "border-gold/30 bg-obsidian text-ivory/80"
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
    <div className="space-y-3 border border-gold/20 bg-obsidian p-4">
      <p className="font-mono text-xs text-ivory/50">{manifest.kitId}</p>
      <p className="text-sm text-ivory/70">{auditionLabel}</p>
      <ul className="flex flex-wrap gap-2">
        {audition.map((s) => {
          const key = `${manifest.kitId}-${s.name}`;
          return (
            <li key={s.name}>
              <button
                type="button"
                onClick={() => onPlay(s.url, key)}
                className="min-h-11 border border-gold/40 px-3 font-condensed text-sm font-semibold tracking-wide text-gold uppercase"
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
              className="flex min-h-11 items-center justify-between border border-gold/20 px-3"
            >
              <span className="font-mono text-sm text-ivory">{s.name}</span>
              <button
                type="button"
                onClick={() => onPlay(s.url, key)}
                className="font-condensed text-xs font-semibold tracking-wide text-gold uppercase"
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
