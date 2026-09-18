"use client";

import { useCallback, useEffect, useRef, useState } from "react";

import { createCassetteSmashChain } from "@/lib/audio/cassette-smash";
import { loadDemoStem, type DemoStemId } from "@/lib/audio/demo-stems";

const STEMS: { id: DemoStemId; label: string; blurb: string }[] = [
  { id: "vocal", label: "Phonk vocal", blurb: "Dirtied chop lane" },
  { id: "808", label: "808", blurb: "Sub + click" },
  { id: "bell", label: "Bell", blurb: "Memphis stack top" },
];

export function CassetteSmashSoundboard() {
  const [smashOn, setSmashOn] = useState(true);
  const [drive, setDrive] = useState(0.75);
  const [wobble, setWobble] = useState(0.45);
  const [playing, setPlaying] = useState<DemoStemId | null>(null);
  const [ready, setReady] = useState(false);
  const ctxRef = useRef<AudioContext | null>(null);
  const chainRef = useRef<ReturnType<typeof createCassetteSmashChain> | null>(null);
  const sourceRef = useRef<AudioBufferSourceNode | null>(null);
  const buffersRef = useRef<Partial<Record<DemoStemId, AudioBuffer>>>({});

  const ensureAudio = useCallback(async () => {
    if (!ctxRef.current) {
      ctxRef.current = new AudioContext();
      chainRef.current = createCassetteSmashChain(ctxRef.current, { drive, wobble });
      chainRef.current.output.connect(ctxRef.current.destination);
    }
    if (ctxRef.current.state === "suspended") {
      await ctxRef.current.resume();
    }
    const ctx = ctxRef.current;
    for (const stem of STEMS) {
      if (!buffersRef.current[stem.id]) {
        buffersRef.current[stem.id] = await loadDemoStem(ctx, stem.id);
      }
    }
    setReady(true);
  }, [drive, wobble]);

  useEffect(() => {
    chainRef.current?.setParams({ drive, wobble });
  }, [drive, wobble]);

  useEffect(() => {
    return () => {
      sourceRef.current?.stop();
      chainRef.current?.disconnect();
      void ctxRef.current?.close();
    };
  }, []);

  const stopCurrent = () => {
    try {
      sourceRef.current?.stop();
    } catch {
      /* already stopped */
    }
    sourceRef.current = null;
    setPlaying(null);
  };

  const playStem = async (id: DemoStemId) => {
    await ensureAudio();
    const ctx = ctxRef.current!;
    const chain = chainRef.current!;
    stopCurrent();

    const src = ctx.createBufferSource();
    src.buffer = buffersRef.current[id]!;
    if (smashOn) {
      src.connect(chain.input);
    } else {
      src.connect(ctx.destination);
    }
    src.onended = () => {
      if (sourceRef.current === src) {
        setPlaying(null);
      }
    };
    sourceRef.current = src;
    setPlaying(id);
    src.start();
  };

  return (
    <section className="mx-auto max-w-3xl rounded-2xl border border-zinc-800 bg-zinc-900/60 p-6 shadow-xl">
      <div className="flex flex-wrap items-start justify-between gap-4">
        <div>
          <p className="text-xs uppercase tracking-widest text-amber-500/90">Results in advance</p>
          <h2 className="mt-1 text-2xl font-semibold text-zinc-50">Cassette Smash soundboard</h2>
          <p className="mt-2 max-w-xl text-sm text-zinc-400">
            Hear clean vs crushed stems in the browser — same two-knob idea as the $17 plugin.
          </p>
        </div>
        <button
          type="button"
          onClick={() => setSmashOn((v) => !v)}
          className={`rounded-full px-4 py-2 text-sm font-medium transition ${
            smashOn
              ? "bg-amber-500 text-zinc-950 hover:bg-amber-400"
              : "bg-zinc-800 text-zinc-200 hover:bg-zinc-700"
          }`}
        >
          Cassette Smash: {smashOn ? "ON" : "OFF"}
        </button>
      </div>

      <div className="mt-6 grid gap-4 sm:grid-cols-2">
        <label className="block text-sm text-zinc-300">
          Drive
          <input
            type="range"
            min={0}
            max={100}
            value={Math.round(drive * 100)}
            onChange={(e) => setDrive(Number(e.target.value) / 100)}
            className="mt-1 w-full accent-amber-500"
          />
        </label>
        <label className="block text-sm text-zinc-300">
          Wobble
          <input
            type="range"
            min={0}
            max={100}
            value={Math.round(wobble * 100)}
            onChange={(e) => setWobble(Number(e.target.value) / 100)}
            className="mt-1 w-full accent-amber-500"
          />
        </label>
      </div>

      <div className="mt-6 grid gap-3 sm:grid-cols-3">
        {STEMS.map((stem) => (
          <button
            key={stem.id}
            type="button"
            onClick={() => void playStem(stem.id)}
            className={`rounded-xl border px-4 py-4 text-left transition ${
              playing === stem.id
                ? "border-amber-500/60 bg-amber-500/10"
                : "border-zinc-700 bg-zinc-950/50 hover:border-zinc-500"
            }`}
          >
            <span className="block text-sm font-medium text-zinc-100">{stem.label}</span>
            <span className="mt-1 block text-xs text-zinc-500">{stem.blurb}</span>
            <span className="mt-2 block text-xs text-amber-500/80">
              {playing === stem.id ? "Playing…" : "Tap to preview"}
            </span>
          </button>
        ))}
      </div>

      {!ready && (
        <p className="mt-4 text-center text-xs text-zinc-500">
          First play unlocks the audio engine (browser gesture policy).
        </p>
      )}
    </section>
  );
}
