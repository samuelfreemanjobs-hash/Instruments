import { synthKick } from "./synth-trap.js";

function mapDist(txType) {
  if (txType === "hard") return "fl_clip";
  if (txType === "fold") return "fold";
  return "triode";
}

/** Map MPC preset object → studio `synthKick` params (layered beater/body/sub). */
export function mpcKickParams(soundParams) {
  const base = soundParams.transient.baseFreq ?? 50;
  const start = soundParams.transient.pitchStart ?? base * 4;
  const pitchMod = Math.min(96, Math.max(0, Math.abs(12 * Math.log2(Math.max(start, 20) / Math.max(base, 20)))));
  return {
    rootHz: base,
    pitchMod,
    pitchDecay: soundParams.transient.pitchDecay ?? 0.022,
    duration: Math.max(0.28, soundParams.amp.decay + soundParams.amp.release),
    aA: soundParams.amp.attack,
    aD: soundParams.amp.decay,
    aS: soundParams.amp.sustain,
    aR: soundParams.amp.release,
    drive: (soundParams.tx.drive ?? 40) / 100,
    distType: mapDist(soundParams.tx.type),
    peakDb: soundParams.tx.ceiling ?? -0.3,
  };
}

/** Render layered kick matching modular studio (`synth-trap.js`). */
export async function renderKickAudioBuffer(soundParams, sampleRate = 44100) {
  const hit = synthKick(mpcKickParams(soundParams));
  const mono = hit.mono ?? hit.left;
  const ctx = new OfflineAudioContext(1, mono.length, sampleRate);
  const buf = ctx.createBuffer(1, mono.length, sampleRate);
  buf.getChannelData(0).set(mono);
  return buf;
}
