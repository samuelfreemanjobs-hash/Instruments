/**
 * Client-only pad audition (Tone.js) for Drum Kit Factory before WAV export.
 * Dynamic import keeps Tone off the server bundle.
 */

export type PadFamily = "kick" | "snare" | "hat" | "perc" | "808" | "fx";

export function padFamily(padIndex: number): PadFamily {
  const families: PadFamily[] = [
    "kick",
    "perc",
    "snare",
    "snare",
    "808",
    "perc",
    "hat",
    "808",
    "hat",
    "kick",
    "hat",
    "fx",
    "kick",
    "hat",
    "snare",
    "fx",
  ];
  return families[padIndex % families.length] ?? "kick";
}

export async function auditionPad(
  padIndex: number,
  opts?: { formulaIndex?: number; velocity?: number },
): Promise<void> {
  if (typeof window === "undefined") return;

  const Tone = await import("tone");
  await Tone.start();

  const velocity = opts?.velocity ?? 0.85;
  const formulaBias = ((opts?.formulaIndex ?? 0) % 8) * 0.04;
  const family = padFamily(padIndex);
  const now = Tone.now();

  switch (family) {
    case "kick":
    case "808": {
      const synth = new Tone.MembraneSynth({
        pitchDecay: family === "808" ? 0.08 : 0.02,
        octaves: family === "808" ? 5 : 4,
        oscillator: { type: "sine" },
        envelope: {
          attack: 0.001,
          decay: family === "808" ? 0.55 : 0.28,
          sustain: 0,
          release: family === "808" ? 0.35 : 0.12,
        },
      }).toDestination();
      synth.volume.value = -8 + formulaBias * 10;
      const note = family === "808" ? "A1" : "C2";
      synth.triggerAttackRelease(note, family === "808" ? "4n" : "8n", now, velocity);
      window.setTimeout(() => synth.dispose(), 1200);
      break;
    }
    case "snare": {
      const noise = new Tone.NoiseSynth({
        noise: { type: "white" },
        envelope: { attack: 0.001, decay: 0.18, sustain: 0, release: 0.08 },
      }).toDestination();
      const body = new Tone.MembraneSynth({
        pitchDecay: 0.01,
        octaves: 2,
        envelope: { attack: 0.001, decay: 0.12, sustain: 0, release: 0.05 },
      }).toDestination();
      noise.volume.value = -10;
      body.volume.value = -14;
      noise.triggerAttackRelease("16n", now, velocity);
      body.triggerAttackRelease("G3", "16n", now, velocity * 0.7);
      window.setTimeout(() => {
        noise.dispose();
        body.dispose();
      }, 800);
      break;
    }
    case "hat": {
      const metal = new Tone.MetalSynth({
        envelope: { attack: 0.001, decay: padIndex === 10 ? 0.35 : 0.08, release: 0.02 },
        harmonicity: 5.1,
        modulationIndex: 32,
        resonance: 3200,
        octaves: 1.2,
      }).toDestination();
      metal.frequency.value = 280 + padIndex * 12;
      metal.volume.value = -18;
      metal.triggerAttackRelease("32n", now, velocity);
      window.setTimeout(() => metal.dispose(), 700);
      break;
    }
    case "perc": {
      const synth = new Tone.MembraneSynth({
        pitchDecay: 0.008,
        octaves: 2,
        envelope: { attack: 0.001, decay: 0.09, sustain: 0, release: 0.04 },
      }).toDestination();
      synth.volume.value = -12;
      synth.triggerAttackRelease(padIndex % 2 === 0 ? "D4" : "A4", "32n", now, velocity);
      window.setTimeout(() => synth.dispose(), 500);
      break;
    }
    case "fx":
    default: {
      const noise = new Tone.NoiseSynth({
        noise: { type: "brown" },
        envelope: { attack: 0.01, decay: 0.4, sustain: 0, release: 0.2 },
      }).toDestination();
      noise.volume.value = -16;
      noise.triggerAttackRelease("8n", now, velocity);
      window.setTimeout(() => noise.dispose(), 900);
      break;
    }
  }
}
