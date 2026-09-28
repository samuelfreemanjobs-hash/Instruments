/** Shared defaults, note map, and producer presets for TRAP-FORGE studio-app. */

export const DRUM_ORDER = [
  { id: "kick", label: "Punch Kick", short: "KICK" },
  { id: "sub808", label: "Trap 808", short: "808" },
  { id: "snare", label: "Crack Snare", short: "SNR" },
  { id: "clap", label: "Dirty Clap", short: "CLAP" },
  { id: "closedhat", label: "Closed Hat", short: "CH" },
  { id: "openhat", label: "Open Hat", short: "OH" },
  { id: "perc", label: "Perc / Cowbell", short: "PERC" },
];

const NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];

/** Root frequencies (Hz) at octave 1 for note pickers. */
export const NOTE_FREQS = Object.fromEntries(
  NOTE_NAMES.map((name, i) => {
    const midi = (1 + 1) * 12 + i;
    return [name, 440 * Math.pow(2, (midi - 69) / 12)];
  }),
);

export function baseEnvelopeParams() {
  return {
    aA: 0.001,
    aD: 0.1,
    aS: 0,
    aR: 0.05,
    fA: 0.002,
    fD: 0.08,
    fS: 0.15,
    fR: 0.06,
    fCut: 7500,
    fEnvAmt: 18,
    fQ: 0.85,
    filterMode: "lp12",
    drive: 0.42,
    distType: "triode",
    bits: 12,
    lofiSr: 26040,
    ceiling: 0.92,
    transAttack: 0,
    transSustain: 0,
    transAttackDb: 0,
    transSustainDb: 0,
    oversample: true,
    peakDb: -0.3,
  };
}

export const DRUM_DEFAULTS = {
  kick: () => ({ duration: 0.28, rootHz: 50, pitchMod: 38, pitchDecay: 0.02, aD: 0.14, fCut: 4200 }),
  sub808: () => ({
    duration: 2,
    rootHz: 55,
    rootNote: "G",
    aA: 0.002,
    aD: 0.45,
    aS: 0.88,
    aR: 0.4,
    fCut: 280,
    harm2: 0.38,
    harm3: 0.24,
    glideMs: 0,
    glideSemi: 0,
    glideTargetHz: null,
    glideExponent: 2.2,
  }),
  snare: () => ({
    duration: 0.32,
    pitchSemi: 0,
    snap: 0.68,
    snapBite: 0.55,
    fCut: 6200,
    reverbMix: 0.28,
    reverbDecay: 0.58,
    reverbDamp: 0.52,
    reverbPre: 14,
  }),
  clap: () => ({
    duration: 0.34,
    flamMs: 0.022,
    reverbMix: 0.24,
    reverbDecay: 0.55,
    reverbDamp: 0.5,
    reverbPre: 18,
  }),
  closedhat: () => ({ duration: 0.055, aD: 0.022, fCut: 7800, dispersion: 0.01 }),
  openhat: () => ({ duration: 0.4, aD: 0.2, aS: 0.1, fCut: 5200 }),
  perc: () => ({ duration: 0.14, rootHz: 540, fCut: 4000 }),
};

export function defaultParamsFor(drumId) {
  const base = baseEnvelopeParams();
  const spec = DRUM_DEFAULTS[drumId];
  return spec ? { ...base, ...spec() } : { ...base };
}

export function buildInitialDrums() {
  return Object.fromEntries(DRUM_ORDER.map((d) => [d.id, defaultParamsFor(d.id)]));
}

export const PRODUCER_PRESETS = {
  jeezyKick: { rootHz: 50, pitchMod: 40, pitchDecay: 0.018, drive: 0.55, label: "Jeezy Snowman Kick", drum: "kick" },
  drummaKick: { rootHz: 52, pitchMod: 48, pitchDecay: 0.015, transAttackDb: 6, label: "Drumma Boy Punch", drum: "kick" },
  shawty808: { rootHz: 42, harm2: 0.45, harm3: 0.3, aS: 0.9, duration: 2.2, label: "Shawty Redd 808", drum: "sub808" },
  mike808: { rootHz: 47, glideMs: 120, glideSemi: 7, label: "Mike Will Trunk Glide", drum: "sub808" },
  gucciSnare: { pitchSemi: 2, snap: 0.75, fCut: 6500, label: "Gucci So Icy Snap", drum: "snare" },
  cardoSnare: { pitchSemi: -1, snap: 0.5, reverbMix: 0.32, reverbDecay: 0.62, label: "Cardo Space Snare", drum: "snare" },
  drummaClap: { flamMs: 0.024, reverbMix: 0.22, label: "Drumma Trap Clap", drum: "clap" },
  cardoHat: { dispersion: 0.012, fCut: 8200, label: "Cardo Silk Hat", drum: "closedhat" },
  sledgrenHat: { dispersion: 0.006, fCut: 6800, label: "Sledgren Roll Hat", drum: "closedhat" },
  estPerc: { rootHz: 620, drive: 0.5, label: "EST Gee Rim Block", drum: "perc" },
};
