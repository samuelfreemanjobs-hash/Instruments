/** DNA kit formulas — mirror of DrumDesignLab GenreBankLibrary (32 banks). */

export type DnaLine = "Bang" | "Hitmaker" | "MelodyMotion" | "SouthernTriumph";

export type DnaFormula = {
  index: number;
  id: string;
  displayName: string;
  line: DnaLine;
  bankIndex: number;
  /** Maps onto STYLE_PRESETS for server-side render. */
  presetId: string;
  promptSeed: string;
};

export const PAD_NAMES = [
  "Kick",
  "Rim",
  "Snare",
  "Clap",
  "808",
  "Perc",
  "CH",
  "808B",
  "CH2",
  "Tom",
  "OH",
  "FX",
  "Kick2",
  "Ride",
  "Snare2",
  "Impact",
] as const;

export type PadName = (typeof PAD_NAMES)[number];

const LINE_PRESET: Record<DnaLine, string> = {
  Bang: "midnight-circuit",
  Hitmaker: "mpc-ready-808",
  MelodyMotion: "boulevard-86",
  SouthernTriumph: "disklordz-screw",
};

function formula(
  index: number,
  line: DnaLine,
  bankIndex: number,
  id: string,
  displayName: string,
  promptSeed: string,
): DnaFormula {
  return {
    index,
    id,
    displayName,
    line,
    bankIndex,
    presetId: LINE_PRESET[line],
    promptSeed,
  };
}

export const DNA_FORMULAS: DnaFormula[] = [
  formula(0, "Bang", 0, "bang_milli_pressure", "Bang — Milli Pressure", "milli pressure bang trap kick 808"),
  formula(1, "Bang", 1, "bang_dirty_digital", "Bang — Dirty Digital", "dirty digital bang trap drums grit"),
  formula(2, "Bang", 2, "bang_atlanta_eccentric", "Bang — Atlanta Eccentric", "atlanta eccentric bang drums"),
  formula(3, "Bang", 3, "bang_boss_drums", "Bang — Boss Drums", "boss drums bang hard trap"),
  formula(4, "Bang", 4, "bang_street_experimental", "Bang — Street Experimental", "street experimental bang perc"),
  formula(5, "Bang", 5, "bang_southern_futurism", "Bang — Southern Futurism", "southern futurism bang 808"),
  formula(6, "Bang", 6, "bang_raw_machine", "Bang — Raw Machine", "raw machine bang industrial trap"),
  formula(7, "Bang", 7, "bang_minimal_monster", "Bang — Minimal Monster", "minimal monster bang sparse punch"),
  formula(8, "Hitmaker", 0, "hitmaker_earthquake", "Hitmaker — Earthquake", "earthquake hitmaker 808 trunk"),
  formula(9, "Hitmaker", 1, "hitmaker_atlanta_heat", "Hitmaker — Atlanta Heat", "atlanta heat hitmaker trap"),
  formula(10, "Hitmaker", 2, "hitmaker_trunk_rattle", "Hitmaker — Trunk Rattle", "trunk rattle hitmaker bass"),
  formula(11, "Hitmaker", 3, "hitmaker_hit_record", "Hitmaker — Hit Record", "hit record hitmaker radio drums"),
  formula(12, "Hitmaker", 4, "hitmaker_dark_experiment", "Hitmaker — Dark Experiment", "dark experiment hitmaker"),
  formula(13, "Hitmaker", 5, "hitmaker_club_anthem", "Hitmaker — Club Anthem", "club anthem hitmaker dance trap"),
  formula(14, "Hitmaker", 6, "hitmaker_melodic_pressure", "Hitmaker — Melodic Pressure", "melodic pressure hitmaker"),
  formula(15, "Hitmaker", 7, "hitmaker_future_machine", "Hitmaker — Future Machine", "future machine hitmaker digital"),
  formula(16, "MelodyMotion", 0, "mm_rich_bounce", "MelodyMotion — Rich Bounce", "rich bounce melodic trap drums"),
  formula(17, "MelodyMotion", 1, "mm_slime_melody", "MelodyMotion — Slime Melody", "slime melody soft 808 clap"),
  formula(18, "MelodyMotion", 2, "mm_church_to_trap", "MelodyMotion — Church To Trap", "church to trap gospel drums"),
  formula(19, "MelodyMotion", 3, "mm_atlanta_lifestyle", "MelodyMotion — Atlanta Lifestyle", "atlanta lifestyle bounce drums"),
  formula(20, "MelodyMotion", 4, "mm_keys_and_bells", "MelodyMotion — Keys And Bells", "keys bells melodic percussion"),
  formula(21, "MelodyMotion", 5, "mm_street_anthem", "MelodyMotion — Street Anthem", "street anthem melodic trap"),
  formula(22, "MelodyMotion", 6, "mm_melodic_luxury", "MelodyMotion — Melodic Luxury", "melodic luxury polished drums"),
  formula(23, "MelodyMotion", 7, "mm_late_night_london", "MelodyMotion — Late Night London", "late night london UK trap drums"),
  formula(24, "SouthernTriumph", 0, "st_southern_triumph", "SouthernTriumph — Southern Triumph", "southern triumph cinematic trap"),
  formula(25, "SouthernTriumph", 1, "st_memphis_roots", "SouthernTriumph — Memphis Roots", "memphis roots phonk cowbell"),
  formula(26, "SouthernTriumph", 2, "st_atlanta_anthem", "SouthernTriumph — Atlanta Anthem", "atlanta anthem southern trap"),
  formula(27, "SouthernTriumph", 3, "st_orchestral_soul", "SouthernTriumph — Orchestral Soul", "orchestral soul trap drums"),
  formula(28, "SouthernTriumph", 4, "st_triple_kick_warfare", "SouthernTriumph — Triple Kick Warfare", "triple kick warfare hard trap"),
  formula(29, "SouthernTriumph", 5, "st_drum_squad_classics", "SouthernTriumph — Drum Squad Classics", "drum squad classics southern"),
  formula(30, "SouthernTriumph", 6, "st_blues_and_trap", "SouthernTriumph — Blues And Trap", "blues and trap southern drums"),
  formula(31, "SouthernTriumph", 7, "st_epic_street_cinema", "SouthernTriumph — Epic Street Cinema", "epic street cinema trap drums"),
];

export function formulaAt(index: number): DnaFormula {
  const i = ((index % DNA_FORMULAS.length) + DNA_FORMULAS.length) % DNA_FORMULAS.length;
  return DNA_FORMULAS[i]!;
}

/** Map a pad index to the closest product-pack / kit sample role. */
export function padSampleHint(padIndex: number): string {
  const map = [
    "kick",
    "rim",
    "snare",
    "clap",
    "kick",
    "rim",
    "hat_closed",
    "kick",
    "hat_closed",
    "snare",
    "hat_open",
    "clap",
    "kick",
    "hat_open",
    "snare",
    "clap",
  ];
  return map[padIndex % map.length] ?? "kick";
}
