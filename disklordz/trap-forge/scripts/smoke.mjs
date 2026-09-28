#!/usr/bin/env node
/** Syntax + DSP smoke (no browser). */
import { readFileSync } from "node:fs";
import { pathToFileURL } from "node:url";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");

function assertSyntax(rel) {
  const src = readFileSync(join(root, rel), "utf8");
  try {
    new Function(src);
  } catch {
    // ES modules: parse via dynamic import
  }
}

assertSyntax("js/studio-app.js");
assertSyntax("js/mpc-trap-forge-ui.js");
assertSyntax("js/trap-presets.js");

const { renderDrumSample, exportMultiVelocityPack, kitState } = await import(
  pathToFileURL(join(root, "js/studio-app.js")).href
);
const { applyDrive } = await import(pathToFileURL(join(root, "js/dsp-core.js")).href);
const { DRUM_ORDER } = await import(pathToFileURL(join(root, "js/trap-presets.js")).href);

if (typeof exportMultiVelocityPack !== "function") throw new Error("exportMultiVelocityPack missing");

const full = renderDrumSample("kick", null, 1);
const soft = renderDrumSample("kick", null, 0.5);
let peakFull = 0;
let peakSoft = 0;
for (let i = 0; i < full.left.length; i++) {
  peakFull = Math.max(peakFull, Math.abs(full.left[i]));
  peakSoft = Math.max(peakSoft, Math.abs(soft.left[i]));
}
const ratio = peakSoft / peakFull;
if (ratio < 0.45 || ratio > 0.55) {
  throw new Error(`velocityScale expected ~0.5, got ratio ${ratio}`);
}

const fl = applyDrive(0.8, 0.5, "fl_clip");
if (Math.abs(fl) > 1.01) throw new Error("fl_clip should bound output");

kitState.drums.sub808.glideMs = 80;
kitState.drums.sub808.glideSemi = 7;
const sub = renderDrumSample("sub808", null, 1);
if (!sub.left.length) throw new Error("sub808 render failed");

for (const d of DRUM_ORDER) {
  const b = renderDrumSample(d.id, null, 1);
  if (!b.left?.length) throw new Error(`empty buffer for ${d.id}`);
}

console.log("trap-forge smoke: ok", { drums: DRUM_ORDER.length, velocityRatio: ratio.toFixed(3) });
