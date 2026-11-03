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
  {
    id: "lane-jeezy",
    label: "Jeezy lane",
    artistLane: "DL-JEEZY",
    bpmHint: "68–76 / 136–152",
    description: "Style archetype: trap-or-die 808 weight, dark knock, anthem snare — original one-shots.",
    tags: ["jeezy", "trap", "808", "south", "archetype"],
  },
  {
    id: "lane-gucci-mane",
    label: "Gucci Mane lane",
    artistLane: "DL-GUCCI",
    bpmHint: "70–78 / 140–156",
    description: "Style archetype: bouncy ATL sub, wide clap, quirky perc — original one-shots.",
    tags: ["gucci", "atl", "bounce", "trap", "archetype"],
  },
  {
    id: "lane-mike-will",
    label: "Mike Will Made It lane",
    artistLane: "DL-MIKEWILL",
    bpmHint: "130–160",
    description: "Style archetype: hard modern trap — tight knock, minimal hats, FX pad slot.",
    tags: ["mike will", "trap", "hard", "archetype"],
  },
  {
    id: "lane-drumma-boy",
    label: "Drumma Boy lane",
    artistLane: "DL-DRUMMA",
    bpmHint: "72–88 / 144–176",
    description: "Style archetype: southern anthem snare crack, rim, big FX hit slot.",
    tags: ["drumma boy", "south", "anthem", "archetype"],
  },
  {
    id: "lane-zaytoven",
    label: "Zaytoven lane",
    artistLane: "DL-ZAYTOVEN",
    bpmHint: "130–155",
    description: "Style archetype: ATL pocket — flutter hats, lighter knock, shaker perc.",
    tags: ["zaytoven", "atl", "hats", "archetype"],
  },
  {
    id: "lane-ti",
    label: "T.I. lane",
    artistLane: "DL-TI",
    bpmHint: "80–100 / 160–200",
    description: "Style archetype: swagger bounce, clean clap stack, crisp southern hats.",
    tags: ["ti", "south", "bounce", "archetype"],
  },
  {
    id: "lane-dj-doomp",
    label: "DJ Doomp lane",
    artistLane: "DL-DOOMP",
    bpmHint: "65–90 / 130–180",
    description: "Style archetype: slow Memphis crunch, crushed snare, dark tops — original one-shots.",
    tags: ["dj doomp", "memphis", "crunch", "archetype"],
  },
];

export function getPreset(id: string): StylePreset | undefined {
  return STYLE_PRESETS.find((p) => p.id === id);
}
