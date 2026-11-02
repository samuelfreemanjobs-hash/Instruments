"use client";

import type {
  GenerationEngine,
  GenerationMode,
  GenerationSpec,
  LoopLength,
} from "@/lib/generation/generation-spec";
import { creditCostForSpec } from "@/lib/generation/mode-utils";

type Props = {
  spec: GenerationSpec;
  onChange: (spec: GenerationSpec) => void;
  disabled?: boolean;
};

const MODE_OPTIONS: { value: GenerationMode; label: string }[] = [
  { value: "one_shot", label: "One shot kit" },
  { value: "loop", label: "Drum loop" },
  { value: "sfx", label: "SFX impact" },
];

const ENGINE_OPTIONS: { value: GenerationEngine; label: string }[] = [
  { value: "studio", label: "Studio — tighter" },
  { value: "creative", label: "Creative — wilder" },
];

const LENGTH_OPTIONS: { value: LoopLength; label: string }[] = [
  { value: "short", label: "Short (2–4 bars)" },
  { value: "medium", label: "Medium (4–8 bars)" },
  { value: "long", label: "Long (8–16 bars)" },
];

export function GenerationSpecFields({ spec, onChange, disabled }: Props) {
  const set = <K extends keyof GenerationSpec>(key: K, value: GenerationSpec[K]) => {
    onChange({ ...spec, [key]: value });
  };

  return (
    <div className="grid gap-3 border-t border-zinc-800 pt-4 sm:grid-cols-2">
      <label className="block text-sm">
        <span className="text-zinc-400">Mode</span>
        <select
          className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
          value={spec.mode}
          disabled={disabled}
          onChange={(e) => set("mode", e.target.value as GenerationMode)}
        >
          {MODE_OPTIONS.map((o) => (
            <option key={o.value} value={o.value}>{o.label}</option>
          ))}
        </select>
      </label>

      <label className="block text-sm">
        <span className="text-zinc-400">Engine</span>
        <select
          className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
          value={spec.engine}
          disabled={disabled}
          onChange={(e) => set("engine", e.target.value as GenerationEngine)}
        >
          {ENGINE_OPTIONS.map((o) => (
            <option key={o.value} value={o.value}>{o.label}</option>
          ))}
        </select>
      </label>

      <label className="block text-sm sm:col-span-2">
        <span className="text-zinc-400">Key</span>
        <input
          type="text"
          className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
          value={spec.key}
          disabled={disabled}
          onChange={(e) => set("key", e.target.value)}
          placeholder="F minor"
        />
      </label>

      <label className="block text-sm">
        <span className="text-zinc-400">BPM</span>
        <input
          type="number"
          min={60}
          max={200}
          className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
          value={spec.bpm}
          disabled={disabled}
          onChange={(e) => set("bpm", Number(e.target.value))}
        />
      </label>

      <label className="block text-sm">
        <span className="text-zinc-400">Bars</span>
        <input
          type="number"
          min={1}
          max={16}
          className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
          value={spec.bars}
          disabled={disabled}
          onChange={(e) => set("bars", Number(e.target.value))}
        />
      </label>

      {(spec.mode === "loop" || spec.mode === "sfx") && (
        <label className="block text-sm sm:col-span-2">
          <span className="text-zinc-400">Length</span>
          <select
            className="mt-1 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-2 py-2 text-zinc-100"
            value={spec.length}
            disabled={disabled}
            onChange={(e) => set("length", e.target.value as LoopLength)}
          >
            {LENGTH_OPTIONS.map((o) => (
              <option key={o.value} value={o.value}>{o.label}</option>
            ))}
          </select>
        </label>
      )}

      <label className="block text-sm">
        <span className="text-zinc-400">Stereo width ({Math.round(spec.stereo * 100)}%)</span>
        <input
          type="range"
          min={0}
          max={100}
          className="mt-2 w-full accent-emerald-500"
          value={Math.round(spec.stereo * 100)}
          disabled={disabled}
          onChange={(e) => set("stereo", Number(e.target.value) / 100)}
        />
      </label>

      <label className="block text-sm">
        <span className="text-zinc-400">Wildness ({Math.round(spec.wildness * 100)}%)</span>
        <input
          type="range"
          min={0}
          max={100}
          className="mt-2 w-full accent-emerald-500"
          value={Math.round(spec.wildness * 100)}
          disabled={disabled}
          onChange={(e) => set("wildness", Number(e.target.value) / 100)}
        />
      </label>

      <p className="text-xs text-zinc-500 sm:col-span-2">
        Batch cost: <span className="text-emerald-400/90">{creditCostForSpec(spec)} credits</span>{" "}
        (loop/SFX tiers + studio surcharge). Preset changes reset defaults from the lane BPM hint.
      </p>
    </div>
  );
}
