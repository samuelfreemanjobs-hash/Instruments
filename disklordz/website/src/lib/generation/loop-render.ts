import type { GenerationEngine } from "@/lib/generation/generation-spec";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { barsForSpec } from "@/lib/generation/mode-utils";
import { mulberry32 } from "@/lib/generation/dsp-core";
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

function velocity(seed: number, beat: number, base: number): number {
  const rng = mulberry32(seed + beat * 131);
  return base * (0.84 + rng() * 0.28);
}

/** WO-SAAS-014 + v2: humanized 4/4 with swing and optional trap hat rolls. */
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
  const hatOpen =
    spec.wildness >= 0.55
      ? renderSampleForEngine("hat_open", params, engine)
      : null;

  const spb = seconds / beats;
  const beatSamples = Math.floor(spb * SAMPLE_RATE);
  const sixteenth = Math.max(1, Math.floor(beatSamples / 4));
  const swingSamples =
    engine === "creative" ? Math.floor(sixteenth * (0.08 + spec.wildness * 0.06)) : 0;

  for (let beat = 0; beat < beats; beat++) {
    const offset = Math.floor(beat * beatSamples);
    const barBeat = beat % 4;

    if (barBeat === 0 || barBeat === 2) {
      mixAt(out, offset, kick, velocity(params.seed, beat, 0.96));
    } else if (spec.wildness >= 0.72 && barBeat === 1) {
      mixAt(out, offset, kick, velocity(params.seed, beat, 0.32));
    }

    if (barBeat === 1 || barBeat === 3) {
      mixAt(out, offset, snare, velocity(params.seed, beat + 100, 0.78));
    }

    const hatOffset = offset + (barBeat % 2 === 1 ? swingSamples : 0);
    mixAt(out, hatOffset, hat, velocity(params.seed, beat + 200, 0.34));

    if (beat % 2 === 1) {
      mixAt(
        out,
        offset + Math.floor(beatSamples / 2),
        hat,
        velocity(params.seed, beat + 201, 0.22),
      );
    }

    if (spec.wildness >= 0.48 && barBeat === 3) {
      for (let s = 0; s < 4; s++) {
        mixAt(
          out,
          offset + sixteenth * (2 + s),
          hat,
          0.14 + mulberry32(params.seed + beat + s)() * 0.14,
        );
      }
    }

    if (hatOpen && spec.wildness >= 0.62 && barBeat === 3) {
      mixAt(out, offset + sixteenth, hatOpen, velocity(params.seed, beat + 300, 0.42));
    }
  }

  return out;
}
