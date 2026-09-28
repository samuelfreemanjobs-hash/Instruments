/**
 * Layered drum passes + phase alignment (Splice-elite style stacking).
 */
import { SR } from "./dsp-core.js";

export function findOnsetIndex(mono, windowSec = 0.02) {
  const maxI = Math.min(mono.length, Math.floor(SR * windowSec));
  let best = 0;
  let peak = 0;
  for (let i = 0; i < maxI; i++) {
    const a = Math.abs(mono[i]);
    if (a > peak) {
      peak = a;
      best = i;
    }
  }
  return best;
}

/** Sum layers with onset alignment to the first layer's transient peak. */
export function phaseAlignMix(layers, weights = null) {
  if (!layers.length) return new Float32Array(0);
  const ref = findOnsetIndex(layers[0]);
  const maxLen = Math.max(...layers.map((l) => l.length));
  const out = new Float32Array(maxLen);
  layers.forEach((layer, li) => {
    const shift = findOnsetIndex(layer) - ref;
    const w = weights?.[li] ?? 1;
    for (let i = 0; i < layer.length; i++) {
      const dest = i - shift;
      if (dest >= 0 && dest < maxLen) out[dest] += layer[i] * w;
    }
  });
  return out;
}

export function applyGlideIntervalParams(p) {
  const out = { ...p };
  const f0 = out.rootHz ?? 55;
  if (out.glideInterval != null && out.glideInterval !== 0) {
    out.glideSemi = out.glideInterval;
    out.glideTargetHz = f0 * Math.pow(2, out.glideInterval / 12);
  } else if (out.glideSemi && !out.glideTargetHz) {
    out.glideTargetHz = f0 * Math.pow(2, out.glideSemi / 12);
  }
  return out;
}
