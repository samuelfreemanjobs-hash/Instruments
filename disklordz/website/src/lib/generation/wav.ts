/** Encode float32 PCM to WAV (mono or interleaved stereo). WO-SAAS-017 stereo width. */
export function encodeWav(
  samples: Float32Array,
  sampleRate = 44100,
  channels: 1 | 2 = 1,
): Buffer {
  const frameCount = channels === 2 ? samples.length / 2 : samples.length;
  let peak = 0;
  for (const s of samples) peak = Math.max(peak, Math.abs(s));
  const scale = peak > 0 ? 0.89 / peak : 1;

  const dataSize = frameCount * channels * 2;
  const buffer = Buffer.alloc(44 + dataSize);
  buffer.write("RIFF", 0);
  buffer.writeUInt32LE(36 + dataSize, 4);
  buffer.write("WAVE", 8);
  buffer.write("fmt ", 12);
  buffer.writeUInt32LE(16, 16);
  buffer.writeUInt16LE(1, 20);
  buffer.writeUInt16LE(channels, 22);
  buffer.writeUInt32LE(sampleRate, 24);
  buffer.writeUInt32LE(sampleRate * channels * 2, 28);
  buffer.writeUInt16LE(channels * 2, 32);
  buffer.writeUInt16LE(16, 34);
  buffer.write("data", 36);
  buffer.writeUInt32LE(dataSize, 40);

  let offset = 44;
  if (channels === 1) {
    for (let i = 0; i < frameCount; i++) {
      const v = Math.max(-1, Math.min(1, samples[i] * scale));
      buffer.writeInt16LE(Math.round(v * 32767), offset);
      offset += 2;
    }
  } else {
    for (let i = 0; i < frameCount; i++) {
      for (let c = 0; c < 2; c++) {
        const v = Math.max(-1, Math.min(1, samples[i * 2 + c] * scale));
        buffer.writeInt16LE(Math.round(v * 32767), offset);
        offset += 2;
      }
    }
  }
  return buffer;
}
