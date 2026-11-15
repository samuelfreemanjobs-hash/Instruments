"use client";

import { useEffect, useRef, useState } from "react";

import type { VaultPreview } from "@/lib/catalog/memphis-vault";

export function PreviewList({ tracks }: { tracks: VaultPreview[] }) {
  const audioRef = useRef<HTMLAudioElement | null>(null);
  const [current, setCurrent] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const audio = new Audio();
    audioRef.current = audio;
    audio.addEventListener("ended", () => setCurrent(null));
    return () => {
      audio.pause();
      audio.src = "";
    };
  }, []);

  async function toggle(file: string) {
    const audio = audioRef.current;
    if (!audio) return;
    setError(null);
    if (current === file && !audio.paused) {
      audio.pause();
      setCurrent(null);
      return;
    }
    audio.src = `/api/catalog/memphis-vault-vol-1?file=${encodeURIComponent(file)}`;
    try {
      await audio.play();
      setCurrent(file);
    } catch {
      setCurrent(null);
      setError("Playback needs a click and a browser that allows audio.");
    }
  }

  return (
    <div>
      <ul className="divide-y divide-gold/20 border border-gold/30 bg-charcoal">
        {tracks.map((track) => {
          const playing = current === track.file;
          return (
            <li key={track.file} className="flex items-center gap-3 px-3 py-2">
              <button
                type="button"
                onClick={() => void toggle(track.file)}
                className="min-h-11 min-w-11 bg-gold px-3 font-condensed text-sm font-semibold tracking-wide text-obsidian"
                aria-pressed={playing}
              >
                {playing ? "Stop" : "Play"}
              </button>
              <span className="font-mono text-sm text-ivory">{track.label}</span>
              <span className="ml-auto font-mono text-xs text-ivory/70">
                {track.key ? track.key : "untuned"} · {track.seconds.toFixed(2)}s
              </span>
            </li>
          );
        })}
      </ul>
      {error ? (
        <p className="mt-2 text-sm text-gold" role="status">
          {error}
        </p>
      ) : null}
    </div>
  );
}
