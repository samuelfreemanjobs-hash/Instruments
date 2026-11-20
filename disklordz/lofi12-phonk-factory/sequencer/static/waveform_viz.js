/** Draw peak waveform from WAV ArrayBuffer onto canvas. */

export async function drawWaveformFromBuffer(canvas, arrayBuffer) {
  if (!canvas || !arrayBuffer) return;
  const ctx = canvas.getContext("2d");
  if (!ctx) return;
  const w = canvas.width;
  const h = canvas.height;
  ctx.fillStyle = "#0a0c10";
  ctx.fillRect(0, 0, w, h);

  try {
    const ac = new (window.AudioContext || window.webkitAudioContext)();
    const audioBuffer = await ac.decodeAudioData(arrayBuffer.slice(0));
    await ac.close();
    const data = audioBuffer.getChannelData(0);
    const step = Math.max(1, Math.floor(data.length / w));
    ctx.strokeStyle = "#00e5ff";
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    for (let x = 0; x < w; x++) {
      const start = x * step;
      let min = 0;
      let max = 0;
      for (let i = 0; i < step && start + i < data.length; i++) {
        const v = data[start + i];
        if (v < min) min = v;
        if (v > max) max = v;
      }
      const y1 = ((1 - max) * 0.5 + 0.5) * h;
      const y2 = ((1 - min) * 0.5 + 0.5) * h;
      ctx.moveTo(x, y1);
      ctx.lineTo(x, y2);
    }
    ctx.stroke();
    ctx.strokeStyle = "rgba(125, 255, 179, 0.35)";
    ctx.beginPath();
    ctx.moveTo(0, h / 2);
    ctx.lineTo(w, h / 2);
    ctx.stroke();
  } catch (_) {
    ctx.fillStyle = "#6b7280";
    ctx.font = "11px monospace";
    ctx.fillText("waveform unavailable", 8, h / 2);
  }
}
