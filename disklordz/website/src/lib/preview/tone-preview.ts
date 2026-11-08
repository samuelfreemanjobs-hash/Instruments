/**
 * Tone.js helpers for richer in-browser audition (https://github.com/Tonejs/Tone.js).
 * Import dynamically in client components to avoid SSR bundling Tone on the server.
 */

export type TonePreviewConfig = {
  bpm: number;
  volumeDb?: number;
};

export async function playClickPreview(config: TonePreviewConfig): Promise<void> {
  if (typeof window === "undefined") {
    return;
  }
  const Tone = await import("tone");
  await Tone.start();
  const synth = new Tone.MembraneSynth().toDestination();
  synth.volume.value = config.volumeDb ?? -12;
  const interval = 60 / Math.max(40, config.bpm);
  synth.triggerAttackRelease("C2", "8n", Tone.now());
  synth.triggerAttackRelease("C2", "8n", Tone.now() + interval);
  synth.triggerAttackRelease("C2", "8n", Tone.now() + interval * 2);
  synth.triggerAttackRelease("C2", "8n", Tone.now() + interval * 3);
}
