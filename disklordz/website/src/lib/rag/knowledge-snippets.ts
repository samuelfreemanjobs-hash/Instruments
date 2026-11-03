/** Short retrieval chunks (from repo docs). Replace with pgvector in WO-SAAS-012b. */

export type KnowledgeSnippet = {
  id: string;
  text: string;
  tags: string[];
};

export const KNOWLEDGE_SNIPPETS: KnowledgeSnippet[] = [
  {
    id: "lane-dl001",
    text: "DL001 Boulevard 86: French touch filter-house 118–124 BPM, 909/707 punch, sidechain-friendly kicks and crisp hats.",
    tags: ["boulevard-86", "french touch", "dl001", "house"],
  },
  {
    id: "lane-dl002",
    text: "DL002 Midnight Circuit: drift/wave phonk 140–160 BPM, 808 glide, Memphis snap, cowbell layers.",
    tags: ["midnight-circuit", "phonk", "dl002", "808"],
  },
  {
    id: "lane-dl006",
    text: "DL006 Disklordz screw: 60–95 BPM slow tape one-shots, thick low-mids, chop and screw sessions.",
    tags: ["disklordz-screw", "screw", "dl006", "tape"],
  },
  {
    id: "lane-dl004",
    text: "DL004 Terminal Mirage: cyber funk 112–128 BPM, tight digital transients, clean MPC-style peaks.",
    tags: ["terminal-mirage", "cyber", "dl004", "funk"],
  },
  {
    id: "spec-studio",
    text: "Studio engine: tighter variations (2 candidates), production-ready prompts with key and BPM.",
    tags: ["studio", "engine", "spec"],
  },
  {
    id: "spec-creative",
    text: "Creative engine: wilder variations (3 candidates), experimental sound design prompts.",
    tags: ["creative", "engine", "spec"],
  },
  {
    id: "memphis-stack",
    text: "Memphis phonk stack: distorted snare snap, narrow stereo cowbell, dusty lo-fi hats, 90s sampler character.",
    tags: ["memphis", "phonk", "distorted", "cowbell"],
  },
  {
    id: "lane-jeezy",
    text: "Jeezy lane (archetype): trap-or-die 808 weight 68–76 or double-time, dark knock, anthem snare body, preset lane-jeezy.",
    tags: ["lane-jeezy", "jeezy", "dl-jeezy", "trap", "808"],
  },
  {
    id: "lane-gucci",
    text: "Gucci Mane lane (archetype): bouncy ATL sub, wide clap, quirky perc on pad 8, preset lane-gucci-mane.",
    tags: ["lane-gucci-mane", "gucci", "atl", "bounce"],
  },
  {
    id: "lane-mike-will",
    text: "Mike Will lane (archetype): hard modern trap 130–160, tight knock, minimal hats, FX pad slot, preset lane-mike-will.",
    tags: ["lane-mike-will", "mike will", "trap", "hard"],
  },
  {
    id: "lane-drumma",
    text: "Drumma Boy lane (archetype): southern anthem snare crack, rim, big FX hit, 72–88 or double-time, preset lane-drumma-boy.",
    tags: ["lane-drumma-boy", "drumma", "south", "anthem"],
  },
  {
    id: "lane-zaytoven",
    text: "Zaytoven lane (archetype): ATL flutter hats, lighter knock, shaker perc, preset lane-zaytoven.",
    tags: ["lane-zaytoven", "zaytoven", "atl", "hats"],
  },
  {
    id: "lane-ti",
    text: "T.I. lane (archetype): swagger bounce, clean clap stack, crisp southern hats, preset lane-ti.",
    tags: ["lane-ti", "ti", "south", "bounce"],
  },
  {
    id: "lane-dj-doomp",
    text: "DJ Doomp lane (archetype): slow Memphis crunch, crushed snare, dark hats, preset lane-dj-doomp.",
    tags: ["lane-dj-doomp", "dj doomp", "memphis", "crunch"],
  },
];
