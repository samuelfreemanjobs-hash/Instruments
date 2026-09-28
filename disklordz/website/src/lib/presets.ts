export type StylePreset = {
  id: string;
  label: string;
  artistLane: string;
  bpmHint: string;
  description: string;
  tags: string[];
};

/** v0 presets aligned to four YouTube / factory artist lanes + neutral MPC pack. */
export const STYLE_PRESETS: StylePreset[] = [
  {
    id: "boulevard-86",
    label: "Boulevard 86",
    artistLane: "DL001",
    bpmHint: "118–124",
    description: "French touch filter-house punch — 909/707, sidechain-friendly.",
    tags: ["french touch", "filter house", "909"],
  },
  {
    id: "midnight-circuit",
    label: "Midnight Circuit",
    artistLane: "DL002",
    bpmHint: "140–160",
    description: "Drift / wave phonk weight — 808 glide, Memphis snap, cowbell-ready.",
    tags: ["drift phonk", "808", "memphis"],
  },
  {
    id: "disklordz-screw",
    label: "Disklordz Screw",
    artistLane: "DL006",
    bpmHint: "60–95",
    description: "Slow tape one-shots for chop & screw sessions — thick low-mids.",
    tags: ["screw", "lo-fi", "tape"],
  },
  {
    id: "terminal-mirage",
    label: "Terminal Mirage",
    artistLane: "DL004",
    bpmHint: "112–128",
    description: "Cyber funk transients — tight, coding-session clean.",
    tags: ["cyber funk", "digital", "mpc"],
  },
  {
    id: "mpc-ready-808",
    label: "MPC-ready 808",
    artistLane: "SUPPLY",
    bpmHint: "any",
    description: "Neutral 44.1 kHz one-shots with consistent peaks for pad workflow.",
    tags: ["808-style", "one-shots", "44.1k"],
  },
];

export function getPreset(id: string): StylePreset | undefined {
  return STYLE_PRESETS.find((p) => p.id === id);
}
