import {
  SAMPLE_RATE,
  adsrEnvelope,
  applyPitchEnv,
  integrateFreqToPhase,
  sp1200Decimate,
  onePoleLowpass,
  softClip,
  normalizePeak,
  FILTER_PRESETS,
} from "./dsp-utils.js";

export function renderKick(params) {
  const sr = SAMPLE_RATE;
  const duration = params.duration ?? 0.25;
  const n = Math.floor(sr * duration);
  const t = new Float32Array(n);
  for (let i = 0; i < n; i++) t[i] = i / sr;

  const rootHz = params.rootHz ?? 43.65;
  const pitchMod = params.pitchModSt ?? 48;
  const pitchDecay = params.pitchDecay ?? 0.022;

  const instFreq = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    instFreq[i] = applyPitchEnv(t[i], rootHz, pitchMod, pitchDecay);
  }
  const phase = integrateFreqToPhase(instFreq, sr);
  const dry = new Float32Array(n);
  for (let i = 0; i < n; i++) dry[i] = Math.sin(phase[i]);

  const ampEnv = adsrEnvelope(
    t,
    {
      attack: params.ampA ?? 0.001,
      decay: params.ampD ?? 0.14,
      sustain: params.ampS ?? 0,
      release: params.ampR ?? 0.04,
    },
    duration,
  );
  for (let i = 0; i < n; i++) dry[i] *= ampEnv[i];

  const filterEnv = adsrEnvelope(
    t,
    {
      attack: params.filA ?? 0.002,
      decay: params.filD ?? 0.035,
      sustain: params.filS ?? 0.15,
      release: params.filR ?? 0.08,
    },
    duration,
  );

  const preset = FILTER_PRESETS[params.filterPreset ?? "cassette"] ?? FILTER_PRESETS.cassette;
  const filtered = preset.apply(dry, sr);
  const signal = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    signal[i] = dry[i] * (1 - filterEnv[i]) + filtered[i] * filterEnv[i];
  }

  let lofi = sp1200Decimate(signal, sr, params.lofiSr ?? 26040, params.bitDepth ?? 12);
  lofi = onePoleLowpass(lofi, sr, params.tapeLpHz ?? 12000);
  lofi = softClip(lofi, params.clipDriveDb ?? 4);
  return normalizePeak(lofi, -0.3);
}

export const KICK_STYLES = {
  doomshop: { label: "Raw 90s / Doomshop", rootHz: 36.71, pitchModSt: 52, clipDriveDb: 5, tapeLpHz: 11000 },
  drift: { label: "Drift phonk", rootHz: 46.25, pitchModSt: 42, clipDriveDb: 4.5, tapeLpHz: 12500 },
  modern: { label: "Modern clean", rootHz: 43.65, pitchModSt: 38, clipDriveDb: 3, tapeLpHz: 13000 },
};
