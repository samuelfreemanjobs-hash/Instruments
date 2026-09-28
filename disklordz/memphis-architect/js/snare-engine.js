import {
  SAMPLE_RATE,
  adsrEnvelope,
  applyPitchEnv,
  integrateFreqToPhase,
  sp1200Decimate,
  onePoleLowpass,
  onePoleHighpass,
  biquadBandpass,
  softClip,
  normalizePeak,
  whiteNoise,
} from "./dsp-utils.js";

export function renderSnare(params) {
  const sr = SAMPLE_RATE;
  const duration = params.duration ?? 0.35;
  const n = Math.floor(sr * duration);
  const t = new Float32Array(n);
  for (let i = 0; i < n; i++) t[i] = i / sr;

  const membraneRoot = params.membraneHz ?? 220;
  const pitchMod = params.pitchModSt ?? 24;
  const pitchDecay = params.pitchDecay ?? 0.018;

  const instFreq = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    instFreq[i] = applyPitchEnv(t[i], membraneRoot, pitchMod, pitchDecay);
  }
  const phase = integrateFreqToPhase(instFreq, sr);
  const membrane = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const s = Math.sin(phase[i]);
    const tri = (2 / Math.PI) * Math.asin(Math.sin(phase[i]));
    membrane[i] = 0.8 * s + 0.2 * tri;
  }

  const bodyAmp = adsrEnvelope(
    t,
    {
      attack: params.bodyA ?? 0.003,
      decay: params.bodyD ?? 0.085,
      sustain: params.bodyS ?? 0,
      release: params.bodyR ?? 0.05,
    },
    duration,
  );
  for (let i = 0; i < n; i++) membrane[i] *= bodyAmp[i] * 0.75;

  const noiseRaw = whiteNoise(n);
  const noiseCenter = params.noiseCenter ?? 2400;
  let noiseBp = biquadBandpass(noiseRaw, sr, noiseCenter, params.noiseQ ?? 1.2);
  const noiseAmp = new Float32Array(n);
  const att = params.noiseAttack ?? 0.0025;
  const noiseDecay = params.noiseDecay ?? 0.19;
  for (let i = 0; i < n; i++) {
    if (t[i] < att) noiseAmp[i] = t[i] / att;
    else noiseAmp[i] = Math.exp(-(t[i] - att) / noiseDecay);
  }
  for (let i = 0; i < n; i++) noiseBp[i] *= noiseAmp[i] * 0.55;

  const flamMs = params.flamDelayMs ?? 4;
  const clap = whiteNoise(n);
  const clapAmp = new Float32Array(n);
  const hits = [
    [0, 0.6],
    [2, 0.8],
    [flamMs, 1.0],
  ];
  for (const [ms, gain] of hits) {
    const off = Math.floor((ms / 1000) * sr);
    for (let i = off; i < n; i++) {
      clapAmp[i] += gain * Math.exp(-(t[i] - t[off]) / 0.035);
    }
  }
  const delaySamples = Math.floor((flamMs / 1000) * sr);
  const clapLayer = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const src = i - delaySamples;
    clapLayer[i] = src >= 0 ? clap[src] * clapAmp[i] * 0.45 : 0;
  }

  let composite = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    composite[i] = membrane[i] + noiseBp[i] + clapLayer[i];
  }

  let lofi = sp1200Decimate(composite, sr, params.lofiSr ?? 26040, params.bitDepth ?? 12);
  lofi = onePoleHighpass(lofi, sr, params.hpHz ?? 105);
  lofi = onePoleLowpass(lofi, sr, params.tapeLpHz ?? 11500);
  lofi = softClip(lofi, params.clipDriveDb ?? 5.5);
  return normalizePeak(lofi, -0.3);
}

export const SNARE_STYLES = {
  doomshop: { label: "Doomshop / DR-660", membraneHz: 220, pitchModSt: 28, clipDriveDb: 6, flamDelayMs: 5 },
  drift: { label: "Drift phonk", membraneHz: 185, pitchModSt: 22, clipDriveDb: 5, noiseCenter: 3200 },
  dusty: { label: "Dusty 90s cassette", membraneHz: 196, pitchModSt: 30, tapeLpHz: 10500, bitDepth: 12 },
};
