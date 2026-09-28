import { readFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";
import { describe, expect, it } from "vitest";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");

describe("trap-forge DSP smoke", () => {
  it("loads core modules", async () => {
    const { applyDrive } = await import(pathToFileURL(join(root, "js/dsp-core.js")).href);
    expect(Math.abs(applyDrive(0.8, 0.5, "fl_clip"))).toBeLessThanOrEqual(1.01);
  });

  it("layered kick matches velocity scaling contract", async () => {
    const { renderDrumSample } = await import(pathToFileURL(join(root, "js/studio-app.js")).href);
    const full = renderDrumSample("kick", null, 1);
    const soft = renderDrumSample("kick", null, 0.5);
    let peakFull = 0;
    let peakSoft = 0;
    for (let i = 0; i < full.left.length; i++) {
      peakFull = Math.max(peakFull, Math.abs(full.left[i]));
      peakSoft = Math.max(peakSoft, Math.abs(soft.left[i]));
    }
    const ratio = peakSoft / peakFull;
    expect(ratio).toBeGreaterThan(0.45);
    expect(ratio).toBeLessThan(0.55);
  });

  it("mpc kick bridge maps METRO preset", async () => {
    const uiSrc = readFileSync(join(root, "js/mpc-trap-forge-ui.js"), "utf8");
    expect(uiSrc).toContain("kick-layered-mpc.mjs");
    const { mpcKickParams } = await import(pathToFileURL(join(root, "js/kick-layered-mpc.mjs")).href);
    const params = mpcKickParams({
      transient: { baseFreq: 52, pitchStart: 320, pitchDecay: 0.035 },
      amp: { attack: 0.001, decay: 0.28, sustain: 0, release: 0.3 },
      tx: { drive: 40, type: "tube", ceiling: -0.1 },
    });
    expect(params.rootHz).toBe(52);
    expect(params.pitchMod).toBeGreaterThan(10);
  });
});
