import { SR } from "./dsp-core.js";

export function cardoReverb(mono, { mix = 0.25, decay = 0.55, damp = 0.6, preDelayMs = 12 } = {}) {
  const n = mono.length;
  const pre = Math.floor((preDelayMs / 1000) * SR);
  const left = new Float32Array(n);
  const right = new Float32Array(n);
  const delaysL = [1117, 1423, 1607];
  const delaysR = [1187, 1499, 1657];
  const fb = 0.35 + decay * 0.45;
  const bufsL = delaysL.map((d) => new Float32Array(d + n));
  const bufsR = delaysR.map((d) => new Float32Array(d + n));

  for (let i = 0; i < n; i++) {
    const dry = i >= pre ? mono[i - pre] : 0;
    let wl = dry, wr = dry;
    delaysL.forEach((d, k) => {
      const idx = i + d;
      const tap = bufsL[k][idx - d] || 0;
      const damped = tap * (1 - damp * 0.35);
      bufsL[k][idx] = dry + damped * fb;
      wl += damped * 0.22;
    });
    delaysR.forEach((d, k) => {
      const idx = i + d;
      const tap = bufsR[k][idx - d] || 0;
      const damped = tap * (1 - damp * 0.35);
      bufsR[k][idx] = dry + damped * fb;
      wr += damped * 0.22;
    });
    left[i] = dry * (1 - mix) + wl * mix;
    right[i] = dry * (1 - mix) + wr * mix * 1.04;
  }
  return { left, right, mono: left };
}
