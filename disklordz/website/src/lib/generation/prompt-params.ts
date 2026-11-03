import { createHash } from "crypto";

import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { wildnessJitterScale } from "@/lib/generation/generation-spec";
import { getPreset } from "@/lib/presets";
import {
  applySparkLaneNudges,
  resolveSparkPresetId,
  sparkLanePresetBases,
} from "@/lib/spark-le/lane-registry";

export type DrumParams = {
  kickPitch: number;
  kickDecay: number;
  snareBody: number;
  snareSnap: number;
  hatDecay: number;
  hatBright: number;
  rimMetal: number;
  clapWide: number;
  seed: number;
};

const PRESET_BASE: Record<string, Partial<DrumParams>> = {
  "boulevard-86": {
    kickPitch: 52,
    kickDecay: 0.22,
    snareBody: 0.35,
    snareSnap: 0.25,
    hatDecay: 0.025,
    hatBright: 0.35,
    rimMetal: 0.45,
    clapWide: 0.35,
  },
  "midnight-circuit": {
    kickPitch: 38,
    kickDecay: 0.35,
    snareBody: 0.2,
    snareSnap: 0.55,
    hatDecay: 0.018,
    hatBright: 0.55,
    rimMetal: 0.65,
    clapWide: 0.5,
  },
  "disklordz-screw": {
    kickPitch: 34,
    kickDecay: 0.55,
    snareBody: 0.55,
    snareSnap: 0.15,
    hatDecay: 0.04,
    hatBright: 0.2,
    rimMetal: 0.3,
    clapWide: 0.25,
  },
  "terminal-mirage": {
    kickPitch: 58,
    kickDecay: 0.18,
    snareBody: 0.3,
    snareSnap: 0.35,
    hatDecay: 0.015,
    hatBright: 0.45,
    rimMetal: 0.55,
    clapWide: 0.4,
  },
  "mpc-ready-808": {
    kickPitch: 45,
    kickDecay: 0.28,
    snareBody: 0.25,
    snareSnap: 0.4,
    hatDecay: 0.022,
    hatBright: 0.45,
    rimMetal: 0.5,
    clapWide: 0.42,
  },
  ...sparkLanePresetBases(),
};

function hashSeed(prompt: string, presetId: string): number {
  const hex = createHash("sha256").update(`${presetId}::${prompt}`).digest("hex");
  return parseInt(hex.slice(0, 8), 16);
}

function clamp(n: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, n));
}

export type VariationContext = {
  variationIndex: number;
  batchId: string;
};

/** Map natural-language prompt tokens to parameter nudges (deterministic + seeded jitter). */
export function resolveDrumParams(
  prompt: string,
  presetId: string,
  spec?: GenerationSpec,
  variation?: VariationContext,
): DrumParams {
  const resolvedPresetId = resolveSparkPresetId(presetId);
  const base = PRESET_BASE[resolvedPresetId] ?? PRESET_BASE["mpc-ready-808"];
  const nudgePresetId = resolvedPresetId;
  let seedInput = spec
    ? `${resolvedPresetId}::${prompt}::${spec.bpm}::${spec.key}::${spec.wildness}::${spec.engine}`
    : prompt;
  if (variation) {
    seedInput += `::batch=${variation.batchId}::v=${variation.variationIndex}`;
  }
  const seed = hashSeed(seedInput, resolvedPresetId);
  const jitterScale = spec ? wildnessJitterScale(spec) : 1;
  const jitter = (i: number) =>
    (((seed >> (i * 4)) & 0xf) / 0xf - 0.5) * jitterScale;

  const p = prompt.toLowerCase();
  let kickPitch = base.kickPitch ?? 45;
  let kickDecay = base.kickDecay ?? 0.28;
  let snareBody = base.snareBody ?? 0.3;
  let snareSnap = base.snareSnap ?? 0.35;
  let hatBright = base.hatBright ?? 0.4;
  let clapWide = base.clapWide ?? 0.4;
  let hatDecay = base.hatDecay ?? 0.025;
  let rimMetal = base.rimMetal ?? 0.5;

  if (/\b(808|sub|deep|low)\b/.test(p)) kickPitch -= 6;
  if (/\b(tight|punch|knock)\b/.test(p)) kickDecay *= 0.85;
  if (/\b(screw|slow|tape|dusty|lo-?fi)\b/.test(p)) {
    kickDecay *= 1.25;
    snareSnap *= 0.7;
    hatBright *= 0.75;
  }
  if (/\b(dirty|grit|memphis|phonk|distort)\b/.test(p)) snareSnap *= 1.2;
  if (/\b(jeezy|snowman|anthem|march)\b/.test(p)) {
    kickDecay *= 1.1;
    snareBody *= 1.08;
  }
  if (/\b(gucci|bounce|quirky|atl)\b/.test(p)) clapWide *= 1.1;
  if (/\b(mike will|mike will made it|minimal hat|hard trap)\b/.test(p)) {
    hatDecay *= 0.9;
    snareSnap *= 1.08;
  }
  if (/\b(drumma|anthem|crack snare|orchestral)\b/.test(p)) {
    snareSnap *= 1.12;
    rimMetal *= 1.1;
  }
  if (/\b(zaytoven|flutter|shaker|atl trap)\b/.test(p)) {
    hatDecay *= 0.85;
    hatBright *= 1.05;
  }
  if (/\b(t\.?i\.?|king of the south|swagger)\b/.test(p)) {
    hatBright *= 1.08;
    clapWide *= 1.05;
  }
  if (/\b(toomp|dj toomp|trunk|memphis crunch|strip)\b/.test(p)) {
    kickDecay *= 1.12;
    hatBright *= 0.82;
    snareSnap *= 1.08;
  }
  if (/\b(dj paul|three 6|36 mafia|hypnotize)\b/.test(p)) {
    snareSnap *= 1.12;
    hatBright *= 0.85;
  }
  if (/\b(juicy j|triplet|dark memphis)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(shawty pimp|underground memphis)\b/.test(p)) kickDecay *= 1.12;
  if (/\b(shawty redd|dirty south loop|swing loop)\b/.test(p)) {
    snareBody *= 1.08;
    clapWide *= 1.06;
  }
  if (/\b(young chop|chicago drill|drill)\b/.test(p)) {
    snareSnap *= 1.1;
    hatDecay *= 0.88;
  }
  if (/\b(est g|est gee|memphis street)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(d-rich|d rich|trunk bounce)\b/.test(p)) {
    kickDecay *= 1.05;
    snareSnap *= 1.05;
  }
  if (/\b(ugk|houston|pimp c|texas trunk)\b/.test(p)) {
    rimMetal *= 1.08;
    kickDecay *= 1.05;
  }
  if (/\b(young dolph|dolph|paper route)\b/.test(p)) snareSnap *= 1.05;
  if (/\b(dr-660|dr660|drum machine)\b/.test(p)) kickDecay *= 0.9;
  if (/\b(tr-808|808 clone|real 808)\b/.test(p)) kickPitch -= 2;
  if (/\b(bay area|hyphy|oakland|sf trap)\b/.test(p)) hatBright *= 1.1;
  if (/\b(detroit trap|detroit street|industrial trap)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(rage|rage trap|hyper trap)\b/.test(p)) {
    hatDecay *= 0.85;
    kickDecay *= 0.92;
  }
  if (/\b(mannie fresh|mannie|cash money|triggerman|nola bounce|orleans bounce)\b/.test(p)) {
    clapWide *= 1.1;
    snareBody *= 1.05;
  }
  if (/\b(master p|no limit|make em say ugh|90s south)\b/.test(p)) {
    snareBody *= 1.06;
    hatBright *= 0.95;
  }
  if (/\b(beats by the pound|bbtp|beats by pound)\b/.test(p)) {
    rimMetal *= 1.08;
    clapWide *= 1.06;
  }
  if (/\b(drake type|drake beat|ovo|toronto|charts|hook ready|melodic trap)\b/.test(p)) {
    hatBright *= 1.06;
    snareSnap *= 0.96;
    kickDecay *= 1.02;
  }
  if (/\b(trophy|nonstop|hard drake|aggressive drake)\b/.test(p)) {
    snareSnap *= 1.1;
    kickDecay *= 0.95;
  }
  if (/\b(808 mafia|southside|dark trap producer)\b/.test(p)) {
    kickPitch -= 3;
    snareSnap *= 1.1;
  }
  if (/\b(purpz|purpz on the beat)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(johnny juliano|juliano)\b/.test(p)) kickDecay *= 1.05;
  if (/\b(cardo|cardo got wings|cinematic)\b/.test(p)) {
    kickDecay *= 1.06;
    clapWide *= 1.05;
  }
  if (/\b(wiz khalifa|wiz|sledgren|jetlife)\b/.test(p)) {
    snareSnap *= 0.94;
    hatDecay *= 1.04;
  }
  if (/\b(tr-909|909 machine|roland 909)\b/.test(p)) kickDecay *= 0.88;
  if (/\b(dom kennedy|dom kenndy)\b/.test(p)) snareBody *= 1.05;
  if (/\b(polyester the saint|polyester)\b/.test(p)) hatBright *= 0.92;
  if (/\b(larry june|larry)\b/.test(p)) hatBright *= 1.04;
  if (/\b(investment rap|boss talk|portfolio)\b/.test(p)) {
    snareSnap *= 0.94;
    hatBright *= 1.05;
  }
  if (/\b(bay hustler|hustler rap|bay street)\b/.test(p)) hatBright *= 1.08;
  if (/\b(le\$|les houston)\b/.test(p)) kickDecay *= 1.06;
  if (/\b(currensy|curren\$y|jazz rap|smoke smooth)\b/.test(p)) {
    rimMetal *= 1.08;
    snareSnap *= 0.92;
  }
  if (/\b(dj quik|quik funk)\b/.test(p)) rimMetal *= 1.1;
  if (/\b(nipsey|marathon)\b/.test(p)) {
    clapWide *= 1.05;
    snareBody *= 1.04;
  }
  if (/\b(strip club|spend that shit|club anthem)\b/.test(p)) clapWide *= 1.12;
  if (/\b(big krit|krit|mississippi soul)\b/.test(p)) snareBody *= 1.06;
  if (/\b(90s houston|h-town classic)\b/.test(p)) {
    kickDecay *= 1.08;
    rimMetal *= 1.05;
  }
  if (/\b(1990s houston|houston 1990s|early h-?town)\b/.test(p)) {
    kickDecay *= 1.1;
    rimMetal *= 1.08;
  }
  if (/\b(2000s houston|houston 2000s)\b/.test(p)) {
    snareSnap *= 1.06;
    clapWide *= 1.05;
  }
  if (/\b(modern houston|houston trap|new h-?town)\b/.test(p)) {
    snareSnap *= 1.08;
    hatDecay *= 0.9;
  }
  if (/\b(fat pat|fat phat|pat type)\b/.test(p)) {
    rimMetal *= 1.1;
    snareBody *= 1.06;
  }
  if (/\b(lex luger|luger|brass trap)\b/.test(p)) snareSnap *= 1.12;
  if (/\b(dj screw|chop screw|screwed)\b/.test(p)) {
    kickDecay *= 1.15;
    hatBright *= 0.75;
    snareSnap *= 0.85;
  }
  if (/\b(mike dean|n\.o\. joe|nojoe|texas psych)\b/.test(p)) kickDecay *= 1.08;
  if (/\b(cloud trap|cloud rap|airy trap)\b/.test(p)) {
    hatBright *= 1.1;
    snareSnap *= 0.92;
  }
  if (/\b(super smoker|super smoke)\b/.test(p)) {
    snareSnap *= 0.88;
    hatDecay *= 1.1;
  }
  if (/\b(helluva)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(doughboyz|cashout)\b/.test(p)) snareBody *= 1.08;
  if (/\b(dej loaf|dej)\b/.test(p)) {
    snareSnap *= 0.94;
    hatBright *= 1.05;
  }
  if (/\b(top side|topside)\b/.test(p)) snareSnap *= 1.04;
  if (/\b(angry beats|engry beats|aggressive trap)\b/.test(p)) snareSnap *= 1.12;
  if (/\b(big germz|germz)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(rick ross|ross|boss trap|luxury trap)\b/.test(p)) {
    kickDecay *= 1.08;
    snareBody *= 1.05;
  }
  if (/\b(big meech|bmf|larry hoover|hoover)\b/.test(p)) {
    snareSnap *= 1.06;
    kickDecay *= 1.04;
  }
  if (/\b(trap or die)\b/.test(p)) kickDecay *= 1.1;
  if (/\b(dj cannon|cannon bounce)\b/.test(p)) clapWide *= 1.08;
  if (/\b(jeezy mixtape|raw jeezy)\b/.test(p)) {
    snareSnap *= 1.06;
    hatBright *= 0.9;
  }
  if (/\b(king louie|king louie chicago)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(king logie|logie)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(skinny pimp|kingpin skinny)\b/.test(p)) {
    kickDecay *= 1.1;
    hatBright *= 0.85;
  }
  if (/\b(og phonk)\b/.test(p)) snareSnap *= 1.1;
  if (/\b(drift phonk|drift)\b/.test(p)) snareSnap *= 1.05;
  if (/\b(wave phonk|wavephonk)\b/.test(p)) {
    hatBright *= 1.08;
    snareSnap *= 1.04;
  }
  if (/\b(^wave\b|wave music)\b/.test(p)) hatBright *= 1.06;
  if (/\b(playboi carti|carti)\b/.test(p)) {
    kickDecay *= 0.92;
    hatDecay *= 0.88;
  }
  if (/\b(pierre bourne|pierre)\b/.test(p)) {
    snareSnap *= 0.94;
    hatBright *= 1.06;
  }
  if (/\b(metro boomin|metro)\b/.test(p)) {
    snareSnap *= 1.08;
    kickDecay *= 1.04;
  }
  if (/\b(future type|future beat|toxic trap)\b/.test(p)) kickDecay *= 1.05;
  if (/\b(organized noize|organised noize|outkast drums)\b/.test(p)) snareBody *= 1.08;
  if (/\b(london on da track|london on the track)\b/.test(p)) clapWide *= 1.06;
  if (/\b(tm88|nightmare trap)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(plugg|opium sound)\b/.test(p)) {
    kickDecay *= 0.9;
    hatDecay *= 0.88;
  }
  if (/\b(pluggnb|plugg nb)\b/.test(p)) {
    snareSnap *= 0.9;
    hatBright *= 1.06;
  }
  if (/\b(modern bounce|nola modern)\b/.test(p)) clapWide *= 1.08;
  if (/\b(sp1200|sp-1200|12-bit chain)\b/.test(p)) {
    snareBody *= 1.06;
    hatBright *= 0.92;
  }
  if (/\b(90s rnb phonk|smooth phonk rnb)\b/.test(p)) snareSnap *= 0.9;
  if (/\b(playa pimp|pimp rap)\b/.test(p)) snareSnap *= 0.92;
  if (/\b(spaceghost purrp|purrp)\b/.test(p)) {
    hatBright *= 0.88;
    snareSnap *= 1.06;
  }
  if (/\b(dj shadow|sample chop)\b/.test(p)) kickDecay *= 0.92;
  if (/\b(judgement g|judgment g|judge g)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(vaporwave)\b/.test(p)) {
    hatBright *= 1.08;
    snareSnap *= 0.88;
  }
  if (/\b(vaportape)\b/.test(p)) kickDecay *= 1.1;
  if (/\b(capsun|origin sounds|splice trap|stock trap)\b/.test(p)) hatBright *= 1.05;
  if (/\b(dj zeek|zeek)\b/.test(p)) clapWide *= 1.07;
  if (/\b(celly cel|celly|heat in the bay)\b/.test(p)) snareBody *= 1.06;
  if (/\b(tone capone|capone mob)\b/.test(p)) rimMetal *= 1.06;
  if (/\b(wheezy)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(tay keith|first take)\b/.test(p)) snareSnap *= 1.12;
  if (/\b(atl jacob|atl jacob type)\b/.test(p)) kickDecay *= 1.05;
  if (/\b(mustard on the beat|mustard)\b/.test(p)) clapWide *= 1.08;
  if (/\b(hit-boy|hit boy)\b/.test(p)) snareBody *= 1.05;
  if (/\b(alchemist|the alchemist)\b/.test(p)) rimMetal *= 1.08;
  if (/\b(cash cobain|sample drill)\b/.test(p)) snareSnap *= 1.05;
  if (/\b(wondagurl|wonda gurl)\b/.test(p)) snareSnap *= 1.04;
  if (/\b(decap|drums that knock|dtk)\b/.test(p)) snareBody *= 1.04;
  if (/\b(pvlace)\b/.test(p)) rimMetal *= 1.06;
  if (/\b(outtatown|rage top)\b/.test(p)) hatDecay *= 0.88;
  if (/\b(gezin)\b/.test(p)) hatBright *= 0.92;
  if (/\b(griselda|craven|daringer)\b/.test(p)) {
    hatBright *= 0.9;
    rimMetal *= 1.08;
  }
  if (/\b(southside|808 mafia)\b/.test(p)) snareSnap *= 1.06;
  if (/\b(playa fly)\b/.test(p)) {
    snareSnap *= 1.08;
    hatBright *= 0.88;
  }
  if (/\b(project pat|choppa pat)\b/.test(p)) snareSnap *= 1.1;
  if (/\b(8ball|mjg|8ball and mjg)\b/.test(p)) {
    snareBody *= 1.06;
    kickDecay *= 1.04;
  }
  if (/\b(trae tha truth|^trae\b)\b/.test(p)) kickDecay *= 1.06;
  if (/\b(dj squeaky|squeaky)\b/.test(p)) {
    hatBright *= 1.1;
    clapWide *= 1.05;
  }
  if (/\b(three 6 mafia|36 mafia|three six)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(dogg pound|dpg gangsta)\b/.test(p)) {
    clapWide *= 1.06;
    snareBody *= 1.05;
  }
  if (/\b(90s snoop|snoop dogg type|snoop pocket)\b/.test(p)) {
    hatDecay *= 1.06;
    snareSnap *= 0.92;
  }
  if (/\b(dre day|90s dre|chronic beat)\b/.test(p)) {
    kickDecay *= 0.94;
    snareBody *= 1.05;
  }
  if (/\b(battlecat|dj battlecat|battle cat)\b/.test(p)) rimMetal *= 1.08;
  if (/\b(cool and dre|cool & dre|cool dre)\b/.test(p)) {
    clapWide *= 1.06;
    snareBody *= 1.04;
  }
  if (/\b(sza type|\bsza\b)\b/.test(p)) {
    snareSnap *= 0.88;
    hatBright *= 1.06;
  }
  if (/\b(alina baraz|alina type)\b/.test(p)) {
    hatBright *= 1.08;
    snareSnap *= 0.9;
  }
  if (/\b(bryson tiller|tiller type|bryson)\b/.test(p)) {
    rimMetal *= 1.08;
    kickDecay *= 1.04;
  }
  if (/\b(trap soul|soul trap)\b/.test(p)) snareBody *= 1.05;
  if (/\b(chill trap|mellow trap|soft trap)\b/.test(p)) {
    snareSnap *= 0.9;
    hatBright *= 1.05;
  }
  if (/\b(chillwave|chill wave)\b/.test(p)) {
    kickDecay *= 1.05;
    snareSnap *= 0.9;
  }
  if (/\b(majestic casual|majestic type|youtube chill hip hop)\b/.test(p)) {
    rimMetal *= 1.05;
    hatDecay *= 1.04;
  }
  if (/\b(timbaland|timbo|syncopated knock)\b/.test(p)) rimMetal *= 1.1;
  if (/\b(neptunes|pharrell drums|chad hugo)\b/.test(p)) {
    hatBright *= 1.06;
    snareSnap *= 0.9;
  }
  if (/\b(crunk|crunk music|lil jon era)\b/.test(p)) {
    clapWide *= 1.1;
    snareBody *= 1.05;
  }
  if (/\b(snap music|atl snap|d4l snap)\b/.test(p)) {
    clapWide *= 1.08;
    snareSnap *= 1.05;
  }
  if (/\b(boogie funk|808 boogie|boogie)\b/.test(p)) rimMetal *= 1.08;
  if (/\b(slow three 6|three 6 slow drag|horrorcore slow)\b/.test(p)) {
    kickDecay *= 1.12;
    hatBright *= 0.85;
  }
  if (/\b(slowed rnb|slowed r&b|screw soul)\b/.test(p)) kickDecay *= 1.1;
  if (/\b(neo soul|neo-soul live)\b/.test(p)) rimMetal *= 1.1;
  if (/\b(lofi jazz trap|lo-fi jazz trap|jazz trap beat)\b/.test(p)) {
    hatBright *= 0.92;
    rimMetal *= 1.06;
  }
  if (/\b(knxwledge|knowledge type)\b/.test(p)) hatBright *= 0.9;
  if (/\b(jazz funk fusion|jazz-funk|fusion rap)\b/.test(p)) rimMetal *= 1.1;
  if (/\b(classic nola bounce|nola bounce classic|triggerman)\b/.test(p)) {
    rimMetal *= 1.08;
    clapWide *= 1.06;
  }
  if (/\b(flint|michigan rap|flint michigan)\b/.test(p)) snareSnap *= 1.08;
  if (/\b(bay area trap)\b/.test(p)) hatBright *= 1.06;
  if (/\b(clean|cyber|digital|crisp)\b/.test(p)) {
    snareSnap *= 0.9;
    hatBright *= 1.1;
  }
  if (/\b(bright|open|air)\b/.test(p)) hatBright *= 1.15;

  const preset = getPreset(resolvedPresetId);
  if (preset?.artistLane === "DL002") snareSnap *= 1.05;
  if (preset?.artistLane === "DL006") kickDecay *= 1.1;

  const laneState = {
    kickPitch,
    kickDecay,
    snareBody,
    snareSnap,
    hatDecay,
    hatBright,
    rimMetal,
    clapWide,
  };
  applySparkLaneNudges(nudgePresetId, laneState);
  kickPitch = laneState.kickPitch;
  kickDecay = laneState.kickDecay;
  snareBody = laneState.snareBody;
  snareSnap = laneState.snareSnap;
  hatDecay = laneState.hatDecay;
  hatBright = laneState.hatBright;
  rimMetal = laneState.rimMetal;
  clapWide = laneState.clapWide;

  if (spec) {
    const bpmFactor = spec.bpm / 120;
    kickDecay *= clamp(1 / Math.sqrt(bpmFactor), 0.85, 1.2);
    if (spec.stereo < 0.4) {
      clapWide = (base.clapWide ?? 0.4) * 0.85;
      hatBright *= 0.92;
    } else if (spec.stereo > 0.65) {
      clapWide = clamp((base.clapWide ?? 0.4) * 1.15, 0.15, 0.65);
    }
    if (/\bminor\b/i.test(spec.key)) hatBright *= 0.98;
  }

  return {
    kickPitch: clamp(kickPitch + jitter(0) * 4, 28, 72),
    kickDecay: clamp(kickDecay * (1 + jitter(1) * 0.08), 0.12, 0.7),
    snareBody: clamp(snareBody * (1 + jitter(2) * 0.1), 0.1, 0.7),
    snareSnap: clamp(snareSnap * (1 + jitter(3) * 0.1), 0.08, 0.75),
    hatDecay: clamp(hatDecay * (1 + jitter(4) * 0.1), 0.01, 0.1),
    hatBright: clamp(hatBright * (1 + jitter(5) * 0.1), 0.1, 0.75),
    rimMetal: clamp(rimMetal * (1 + jitter(6) * 0.1), 0.2, 0.85),
    clapWide: clamp(clapWide * (1 + jitter(7) * 0.1), 0.15, 0.65),
    seed,
  };
}
