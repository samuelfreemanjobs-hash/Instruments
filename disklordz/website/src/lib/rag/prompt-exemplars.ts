import type { GenerationEngine } from "@/lib/generation/generation-spec";

export type PromptExemplar = {
  prompt: string;
  presetId?: string;
  engine?: GenerationEngine;
  key?: string;
  bpm?: number;
  tags: string[];
};

/** Curated discovery prompts (lane-aligned). WO-SAAS-012 v1 — embeddings later. */
export const PROMPT_EXEMPLARS: PromptExemplar[] = [
  {
    prompt: "French touch filtered disco kick, sidechain pump, 122 BPM",
    presetId: "boulevard-86",
    engine: "studio",
    key: "A minor",
    bpm: 122,
    tags: ["french touch", "filter", "house"],
  },
  {
    prompt: "crispy 909 snare with vinyl hiss, late 90s filter house",
    presetId: "boulevard-86",
    engine: "creative",
    key: "D minor",
    bpm: 120,
    tags: ["909", "vinyl", "snare"],
  },
  {
    prompt: "drift phonk 808 with long glide, cowbell ghost notes",
    presetId: "midnight-circuit",
    engine: "creative",
    key: "F minor",
    bpm: 150,
    tags: ["phonk", "808", "cowbell"],
  },
  {
    prompt: "Memphis snap clap, distorted hat, midnight highway vibe",
    presetId: "midnight-circuit",
    engine: "studio",
    key: "G minor",
    bpm: 145,
    tags: ["memphis", "snap", "distorted"],
  },
  {
    prompt: "screwed tape kick, thick low-mid mud, chop-ready one-shot",
    presetId: "disklordz-screw",
    engine: "creative",
    key: "F minor",
    bpm: 72,
    tags: ["screw", "tape", "lo-fi"],
  },
  {
    prompt: "slowed rim shot with wow flutter, 90s Houston character",
    presetId: "disklordz-screw",
    engine: "studio",
    key: "C minor",
    bpm: 68,
    tags: ["rim", "houston", "slow"],
  },
  {
    prompt: "cyber funk digital clap, tight transient, neon grid",
    presetId: "terminal-mirage",
    engine: "studio",
    key: "E minor",
    bpm: 118,
    tags: ["cyber", "funk", "digital"],
  },
  {
    prompt: "vapor-adjacent open hat, bitcrush tail, terminal mirage",
    presetId: "terminal-mirage",
    engine: "creative",
    key: "B minor",
    bpm: 124,
    tags: ["vapor", "hat", "bitcrush"],
  },
  {
    prompt: "MPC-ready 808 kick, even peaks, neutral studio mix",
    presetId: "mpc-ready-808",
    engine: "studio",
    key: "C minor",
    bpm: 96,
    tags: ["808", "mpc", "neutral"],
  },
  {
    prompt: "dirty boom bap kick with tape grit and dusty top",
    presetId: "mpc-ready-808",
    engine: "creative",
    key: "F# minor",
    bpm: 92,
    tags: ["boom bap", "tape", "grit"],
  },
  {
    prompt: "1997 Memphis phonk cowbell, SP-1200 grit, dark minor",
    presetId: "midnight-circuit",
    engine: "creative",
    key: "F# minor",
    bpm: 92,
    tags: ["memphis", "phonk", "cowbell"],
  },
  {
    prompt: "filter sweep house percussion, Boulevard 86 sparkle",
    presetId: "boulevard-86",
    engine: "creative",
    bpm: 124,
    tags: ["filter", "percussion"],
  },
];
