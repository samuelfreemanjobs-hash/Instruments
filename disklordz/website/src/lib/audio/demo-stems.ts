/** Procedural demo stems for /launch until real WAVs ship in public/launch/stems/. */

export type DemoStemId = "vocal" | "808" | "bell";

const STEM_FILES: Partial<Record<DemoStemId, string>> = {
  // vocal: "/launch/stems/vocal.wav",
};

export async function loadDemoStem(
  ctx: AudioContext,
  id: DemoStemId,
): Promise<AudioBuffer> {
  const url = STEM_FILES[id];
  if (url) {
    const res = await fetch(url);
    const ab = await res.arrayBuffer();
    return ctx.decodeAudioData(ab);
  }
  return synthesizeStem(ctx, id);
}

function synthesizeStem(ctx: AudioContext, id: DemoStemId): AudioBuffer {
  const sampleRate = ctx.sampleRate;
  const durationSec = id === "808" ? 1.2 : 2.4;
  const length = Math.floor(sampleRate * durationSec);
  const buffer = ctx.createBuffer(1, length, sampleRate);
  const data = buffer.getChannelData(0);

  if (id === "808") {
    let phase = 0;
    const f0 = 55;
    for (let i = 0; i < length; i += 1) {
      const t = i / sampleRate;
      const env = Math.exp(-t * 4.2) * (1 - Math.exp(-t * 80));
      phase += (2 * Math.PI * f0) / sampleRate;
      const click = i < 80 ? (1 - i / 80) * 0.35 : 0;
      data[i] = (Math.sin(phase) * 0.85 + click) * env * 0.9;
    }
    return buffer;
  }

  if (id === "bell") {
    for (let i = 0; i < length; i += 1) {
      const t = i / sampleRate;
      const env = Math.exp(-t * 2.8);
      const f = 880 + Math.sin(t * 12) * 40;
      data[i] =
        env *
        0.35 *
        (Math.sin(2 * Math.PI * f * t) + 0.25 * Math.sin(2 * Math.PI * f * 2.01 * t));
    }
    return buffer;
  }

  // vocal-ish: bandpassed noise bursts + formant tone
  let noiseSeed = 12345;
  const rand = () => {
    noiseSeed = (noiseSeed * 16807) % 2147483647;
    return (noiseSeed & 0xffff) / 0xffff - 0.5;
  };
  let phase = 0;
  const fFormant = 320;
  for (let i = 0; i < length; i += 1) {
    const t = i / sampleRate;
    const gate = 0.5 + 0.5 * Math.sin(2 * Math.PI * 2.1 * t);
    phase += (2 * Math.PI * fFormant) / sampleRate;
    const tone = Math.sin(phase) * 0.25;
    const noise = rand() * 0.4 * gate;
    const env = 0.55 + 0.45 * Math.sin(2 * Math.PI * 0.35 * t);
    data[i] = (tone + noise) * env * 0.55;
  }
  return buffer;
}
