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
    id: "spark-le-index",
    text: "SparkLE elite lanes (archetypes): Memphis, Atlanta, South, Chicago, Cash Money NO (Mannie Fresh, Master P, Beats by the Pound), Drake-type polished beats, hardware DR-660/TR-808 clone, Bay/Detroit trap, rage. Registry: lane-registry.ts. Gangsta trap = hard street trap not trap house.",
    tags: ["spark-le", "lanes", "gangsta trap", "memphis", "atlanta", "registry"],
  },
  {
    id: "lane-dj-toomp",
    text: "DJ Toomp lane preset lane-dj-toomp: trunk-rattling ATL 808, thick sub, hard knock, 65–80 or double-time.",
    tags: ["lane-dj-toomp", "dj toomp", "toomp", "atl", "trunk"],
  },
  {
    id: "lane-shawty-redd",
    text: "Shawty Redd lane preset lane-shawty-redd: dirty south drum loops, thick kick, clap stack, swing — prioritize loop stems.",
    tags: ["lane-shawty-redd", "shawty redd", "dirty south", "loops"],
  },
  {
    id: "lane-tr-808-clone",
    text: "TR-808 clone lane preset lane-tr-808-clone: canonical repo 808 voice for SparkLE pads — align with Instruments 808 clone path.",
    tags: ["lane-tr-808-clone", "tr-808", "808 clone", "hardware"],
  },
  {
    id: "lane-zaytoven-drich",
    text: "Zaytoven preset lane-zaytoven (flutter hats); D-Rich preset lane-d-rich (trunk bounce, club weight). Both Atlanta family.",
    tags: ["lane-zaytoven", "lane-d-rich", "zaytoven", "d-rich", "atl"],
  },
  {
    id: "lane-regional-rage",
    text: "Bay Area trap lane-bay-area-trap (bright hyphy-adjacent); Detroit trap lane-detroit-trap (industrial snap); rage lane-rage-trap (short 808, frantic hats).",
    tags: ["lane-bay-area-trap", "lane-detroit-trap", "lane-rage-trap", "rage", "detroit", "bay area"],
  },
  {
    id: "lane-cash-money-no",
    text: "Cash Money / NO old school: Mannie Fresh lane-mannie-fresh (bounce, Triggerman clap); Master P lane-master-p (90s No Limit); Beats by the Pound lane-beats-by-the-pound (BBTP swing). Archetypes only.",
    tags: ["lane-mannie-fresh", "lane-master-p", "lane-beats-by-the-pound", "cash money", "mannie fresh", "new orleans"],
  },
  {
    id: "lane-drake-type",
    text: "Drake-type beat preset lane-drake-type: polished 808, crisp hats, hook-ready space 70–95 or 140–150. Prompt hard drake or trophy for aggressive variant. Style archetype not affiliated.",
    tags: ["lane-drake-type", "drake type", "drake beat", "ovo", "polished trap"],
  },
  {
    id: "lane-trap-producers-ext",
    text: "Trap producer lanes: 808 Mafia/Southside lane-808-mafia-southside; Purpz lane-purpz; Johnny Juliano lane-johnny-juliano; Cardo lane-cardo. Archetypes only.",
    tags: ["lane-808-mafia-southside", "lane-purpz", "lane-johnny-juliano", "lane-cardo", "808 mafia", "southside", "cardo"],
  },
  {
    id: "lane-jetlife-smooth-ext",
    text: "Jetlife/smooth lanes: Wiz lane-wiz-khalifa, Sledgren lane-sledgren, Dom Kennedy lane-dom-kennedy, Polyester lane-polyester-the-saint, Larry June lane-larry-june, investment rap lane-investment-rap, Le$ lane-le-dollar, Jetlife lane-jetlife, Curren$y jazz lane-currensy-jazz-rap. Archetypes only.",
    tags: ["lane-wiz-khalifa", "lane-jetlife", "lane-currensy-jazz-rap", "lane-larry-june", "jetlife", "smooth"],
  },
  {
    id: "lane-west-strip-909",
    text: "West/Bay/strip/hardware: DJ Quik lane-dj-quik, Nipsey lane-nipsey-hussle, Bay hustler lane-bay-hustler-rap, strip anthem lane-strip-club-anthem, TR-909 lane-tr-909. Archetypes only.",
    tags: ["lane-dj-quik", "lane-nipsey-hussle", "lane-strip-club-anthem", "lane-tr-909", "strip club"],
  },
  {
    id: "lane-houston-screw-ext",
    text: "Houston: 90s lane-houston-90s, DJ Screw lane-dj-screw (matches disklordz-screw), Mike Dean/N.O. Joe lane-mike-dean-nojoe. Archetypes only.",
    tags: ["lane-houston-90s", "lane-dj-screw", "lane-mike-dean-nojoe", "dj screw", "houston"],
  },
  {
    id: "lane-detroit-cloud-ext",
    text: "Detroit: Helluva lane-helluva, Doughboyz lane-doughboyz-cashout, Dej Loaf lane-dej-loaf, Top side lane-top-side, Big Germz lane-big-germz. Cloud/lex/angry: lane-cloud-trap, lane-lex-luger, lane-angry-beats (alias lane-engry-beats), Big Krit lane-big-krit, Super smoker lane-super-smoker. Archetypes only.",
    tags: ["lane-helluva", "lane-doughboyz-cashout", "lane-dej-loaf", "lane-cloud-trap", "lane-lex-luger", "lane-big-krit", "detroit"],
  },
];
