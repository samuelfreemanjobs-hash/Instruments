import type { GenerationEngine } from "@/lib/generation/generation-spec";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { barsForSpec } from "@/lib/generation/mode-utils";
import type { DrumParams } from "@/lib/generation/prompt-params";
import { renderSampleForEngine } from "@/lib/generation/engine-render";

const SAMPLE_RATE = 44100;

function mixAt(buf: Float32Array, offset: number, sample: Float32Array, gain = 1): void {
  for (let i = 0; i < sample.length; i++) {
    const idx = offset + i;
    if (idx >= buf.length) break;
    buf[idx] += sample[i] * gain;
  }
}

/** WO-SAAS-014: simple 4/4 drum loop at spec BPM/bars. */
export function renderDrumLoop(
  params: DrumParams,
  spec: GenerationSpec,
  engine: GenerationEngine,
): Float32Array {
  const bars = barsForSpec(spec);
  const beats = bars * 4;
  const seconds = (60 / spec.bpm) * beats;
  const n = Math.floor(SAMPLE_RATE * seconds);
  const out = new Float32Array(n);

  const kick = renderSampleForEngine("kick", params, engine);
  const snare = renderSampleForEngine("snare", params, engine);
  const hat = renderSampleForEngine("hat_closed", params, engine);

  const spb = seconds / beats;
  const kickSamples = Math.floor(spb * SAMPLE_RATE);

  for (let beat = 0; beat < beats; beat++) {
    const offset = Math.floor(beat * kickSamples);
    if (beat % 4 === 0 || beat % 4 === 2) {
      mixAt(out, offset, kick, 0.95);
    }
    if (beat % 4 === 1 || beat % 4 === 3) {
      mixAt(out, offset, snare, 0.75);
    }
    mixAt(out, offset, hat, 0.35);
    if (beat % 2 === 1) {
      mixAt(out, offset + Math.floor(kickSamples / 2), hat, 0.25);
    }
  }

  let peak = 0;
  for (const s of out) peak = Math.max(peak, Math.abs(s));
  const scale = peak > 0 ? 0.89 / peak : 1;
  for (let i = 0; i < out.length; i++) out[i] *= scale;

  return out;
}
