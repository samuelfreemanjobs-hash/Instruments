import {
  SR,
  adsr,
  decimate,
  onePoleLP,
  biquadLP,
  normalizePeak,
  applyDrive,
  applySaturationBuffer,
  attackTransientGain,
  sustainBodyGain,
  svfLowPass,
} from "./dsp-core.js";
import { cardoReverb } from "./reverb-cardo.js";

const HAT808 = [263, 400, 421, 474, 587, 845];

function softSquare(phase) {
  return Math.sin(phase) + 0.28 * Math.sin(phase * 3) + 0.08 * Math.sin(phase * 5);
}

function applyFilterEnv(t, p, gate) {
  const fe = adsr(t, p.fA, p.fD, p.fS, p.fR, gate);
  const cut = p.fCut * Math.pow(2, (p.fEnvAmt * fe) / 12);
  return Math.max(80, Math.min(cut, 16000));
}

function processChain(mono, p, stereoFn) {
  let x = decimate(mono, p.lofiSr ?? 26040, p.bits ?? 12);
  let lp = 0;
  const st = { x1: 0, x2: 0, y1: 0, y2: 0 };
  const svf = { ic1eq: 0, ic2eq: 0 };
  const gate = p.duration ?? 0.2;
  const n = x.length;
  const out = new Float32Array(n);
  const attackDb =
    p.transAttackDb ??
    (p.transAttack != null ? (p.transAttack - 0.5) * 24 : 0);
  const sustainDb = p.transSustainDb ?? (p.transSustain != null ? (p.transSustain - 0.5) * 24 : 0);
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const cut = applyFilterEnv(t, p, gate);
    let s = x[i];
    if (p.filterMode === "lp24") s = biquadLP(s, st, cut, p.fQ ?? 0.8);
    else if (p.filterMode === "svf") s = svfLowPass(s, svf, cut, p.fQ ?? 0.85);
    else {
      lp = onePoleLP(s, lp, cut);
      s = lp;
    }
    s *= attackTransientGain(t, attackDb, 0.015);
    s *= sustainBodyGain(t, sustainDb, 0.015);
    out[i] = s;
  }
  let shaped = out;
  const dist = p.distType ?? "triode";
  if (p.oversample !== false && dist !== "fl_clip") shaped = applySaturationBuffer(out, 1 + (p.drive ?? 0.4));
  else {
    shaped = new Float32Array(n);
    for (let i = 0; i < n; i++) shaped[i] = applyDrive(out[i], p.drive ?? 0.4, dist);
  }
  const sustainBoost = 1 + (p.transSustain ?? 0) * 0.35;
  if (sustainBoost !== 1 && p.transSustainDb == null) {
    for (let i = 0; i < n; i++) {
      const t = i / SR;
      if (t > 0.02) shaped[i] *= sustainBoost;
    }
  }
  const norm = normalizePeak(shaped, p.peakDb ?? -0.3);
  if (stereoFn) return stereoFn(norm);
  return { left: norm, right: norm, mono: norm };
}

export function synthKick(p) {
  const dur = p.duration ?? 0.28;
  const n = Math.floor(SR * (dur + (p.aR ?? 0.05)));
  const mono = new Float32Array(n);
  let phase = 0;
  let lp = 0;
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const gate = dur;
    const ae = adsr(t, p.aA ?? 0.001, p.aD ?? 0.12, p.aS ?? 0, p.aR ?? 0.05, gate);
    const beater = t < 0.004 ? Math.exp(-t / 0.0008) * Math.sin(2 * Math.PI * 2800 * t) * 0.55 : 0;
    const pitchEnv = Math.exp(-t / (p.pitchDecay ?? 0.022));
    const f0 = (p.rootHz ?? 48) * Math.pow(2, ((p.pitchMod ?? 36) * pitchEnv) / 12);
    phase += (2 * Math.PI * f0) / SR;
    const body = Math.sin(phase);
    const sub = Math.sin(phase * 0.5) * 0.35;
    mono[i] = (beater + body * 0.75 + sub) * ae;
  }
  return processChain(mono, p);
}

export function synth808(p) {
  const dur = p.duration ?? 1.8;
  const n = Math.floor(SR * dur);
  const mono = new Float32Array(n);
  const f0 = p.rootHz ?? 55;
  const glideT = (p.glideMs ?? 0) / 1000;
  const glideExp = p.glideExponent ?? 2.2;
  let phase = 0;
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.002, p.aD ?? 0.4, p.aS ?? 0.85, p.aR ?? 0.35, dur * 0.95);
    let freq = f0;
    const glideTarget =
      p.glideTargetHz ??
      p.glideTarget ??
      (p.glideSemi ? f0 * Math.pow(2, p.glideSemi / 12) : f0);
    if (glideT > 0 && glideTarget !== f0) {
      const u = Math.min(1, t / glideT);
      freq = f0 + (glideTarget - f0) * Math.pow(u, glideExp);
    }
    phase += (2 * Math.PI * freq) / SR;
    const fund = Math.sin(phase);
    const h2 = Math.sin(phase * 2) * (p.harm2 ?? 0.35);
    const h3 = Math.sin(phase * 3) * (p.harm3 ?? 0.22);
    mono[i] = (fund + h2 + h3) * ae;
  }
  return processChain(mono, p);
}

export function synthSnare(p) {
  const dur = p.duration ?? 0.32;
  const n = Math.floor(SR * dur);
  const mono = new Float32Array(n);
  const pitch = Math.pow(2, (p.pitchSemi ?? 0) / 12);
  const f1 = 180 * pitch;
  const f2 = 330 * pitch;
  let p1 = 0, p2 = 0;
  const snapAmt = p.snap ?? 0.65;
  const bite = p.snapBite ?? 0.5;
  const snapFreq = 1800 + snapAmt * 4200 + bite * 900;
  const st = { x1: 0, x2: 0, y1: 0, y2: 0 };
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.001, p.aD ?? 0.09, p.aS ?? 0, p.aR ?? 0.06, dur * 0.85);
    p1 += (2 * Math.PI * f1) / SR;
    p2 += (2 * Math.PI * f2) / SR;
    const shell = 0.55 * Math.sin(p1) + 0.35 * Math.sin(p2 * 1.07);
    const noise = (Math.random() * 2 - 1) * Math.exp(-t / 0.045);
    const snap = biquadLP(noise, st, snapFreq, 0.9 + snapAmt);
    mono[i] = (shell + snap * snapAmt * 0.9) * ae;
  }
  let result = processChain(mono, p);
  if ((p.reverbMix ?? 0) > 0) {
    result = cardoReverb(result.mono, {
      mix: p.reverbMix,
      decay: p.reverbDecay ?? 0.5,
      damp: p.reverbDamp ?? 0.55,
      preDelayMs: p.reverbPre ?? 14,
    });
  }
  return result;
}

export function synthClap(p) {
  const dur = p.duration ?? 0.35;
  const n = Math.floor(SR * dur);
  const left = new Float32Array(n);
  const right = new Float32Array(n);
  const bursts = [0, 0.008, 0.016, p.flamMs ?? 0.022];
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.001, p.aD ?? 0.11, p.aS ?? 0, p.aR ?? 0.08, dur * 0.8);
    let env = 0;
    bursts.forEach((b, k) => {
      if (t >= b) env += (0.5 + k * 0.15) * Math.exp(-(t - b) / 0.028);
    });
    const nL = Math.random() * 2 - 1;
    const nR = Math.random() * 2 - 1;
    left[i] = nL * env * ae;
    right[i] = nR * env * ae * 1.05;
  }
  const mono = new Float32Array(n);
  for (let i = 0; i < n; i++) mono[i] = (left[i] + right[i]) * 0.5;
  let result = processChain(mono, p, () => ({ left, right, mono }));
  if ((p.reverbMix ?? 0) > 0) {
    result = cardoReverb(result.mono, {
      mix: p.reverbMix,
      decay: p.reverbDecay ?? 0.55,
      damp: p.reverbDamp ?? 0.5,
      preDelayMs: p.reverbPre ?? 18,
    });
  }
  return result;
}

export function synthHatClosed(p) {
  const dur = p.duration ?? 0.06;
  const n = Math.floor(SR * dur);
  const mono = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.0005, p.aD ?? 0.025, p.aS ?? 0, p.aR ?? 0.015, dur * 0.9);
    let s = 0;
    HAT808.forEach((f, k) => {
      const det = 1 + (k - 2.5) * (p.dispersion ?? 0.008);
      s += softSquare((2 * Math.PI * f * det * t)) / HAT808.length;
    });
    s += (Math.random() * 2 - 1) * 0.08;
    mono[i] = s * ae;
  }
  p.fCut = p.fCut ?? 7000;
  return processChain(mono, p);
}

export function synthHatOpen(p) {
  const dur = p.duration ?? 0.42;
  const n = Math.floor(SR * dur);
  const mono = new Float32Array(n);
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.002, p.aD ?? 0.18, p.aS ?? 0.12, p.aR ?? 0.2, dur);
    let s = 0;
    HAT808.forEach((f) => { s += softSquare((2 * Math.PI * f * 1.02 * t)) / HAT808.length; });
    s += (Math.random() * 2 - 1) * 0.15 * Math.exp(-t / 0.08);
    mono[i] = s * ae;
  }
  p.fCut = p.fCut ?? 5500;
  return processChain(mono, p);
}

export function synthPerc(p) {
  const dur = p.duration ?? 0.15;
  const n = Math.floor(SR * dur);
  const mono = new Float32Array(n);
  const f0 = p.rootHz ?? 540;
  for (let i = 0; i < n; i++) {
    const t = i / SR;
    const ae = adsr(t, p.aA ?? 0.001, p.aD ?? 0.05, p.aS ?? 0, p.aR ?? 0.04, dur * 0.85);
    const bell = Math.sign(Math.sin((2 * Math.PI * f0 * t))) * 0.6 + Math.sign(Math.sin((2 * Math.PI * f0 * 1.47 * t))) * 0.4;
    mono[i] = bell * ae;
  }
  return processChain(mono, p);
}

export const SYNTHS = {
  kick: synthKick,
  sub808: synth808,
  snare: synthSnare,
  clap: synthClap,
  closedhat: synthHatClosed,
  openhat: synthHatOpen,
  hatClosed: synthHatClosed,
  hatOpen: synthHatOpen,
  perc: synthPerc,
};

export const PRESETS = {
  jeezyKick: { rootHz: 50, pitchMod: 40, pitchDecay: 0.018, drive: 0.55, label: "Jeezy Snowman Kick" },
  drummaKick: { rootHz: 52, pitchMod: 48, pitchDecay: 0.015, transAttackDb: 6, label: "Drumma Boy Punch" },
  shawty808: { rootHz: 42, harm2: 0.45, harm3: 0.3, aS: 0.9, duration: 2.2, label: "Shawty Redd 808" },
  mike808: { rootHz: 47, glideMs: 120, glideTarget: 62, label: "Mike Will Trunk Glide" },
  gucciSnare: { pitchSemi: 2, snap: 0.75, fCut: 6500, label: "Gucci So Icy Snap" },
  cardoSnare: { pitchSemi: -1, snap: 0.5, reverbMix: 0.32, reverbDecay: 0.62, label: "Cardo Space Snare" },
  drummaClap: { flamMs: 0.024, reverbMix: 0.22, label: "Drumma Trap Clap" },
  cardoHat: { dispersion: 0.012, fCut: 8200, label: "Cardo Silk Hat" },
  sledgrenHat: { dispersion: 0.006, fCut: 6800, label: "Sledgren Roll Hat" },
  estPerc: { rootHz: 620, drive: 0.5, label: "EST Gee Rim Block" },
};
