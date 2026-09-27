#!/usr/bin/env node
/** Offline metrics for a generated kit kick WAV (requires running server). */
const base = process.env.DISKLORDZ_URL || "http://127.0.0.1:3000";

const res = await fetch(`${base.replace(/\/$/, "")}/api/generate`, {
  method: "POST",
  headers: { "Content-Type": "application/json" },
  body: JSON.stringify({
    prompt: "memphis phonk trap 808",
    presetId: "boulevard-86",
    spec: { mode: "one_shot", engine: "creative", wildness: 0.7, stereo: 0.2 },
  }),
});
const json = await res.json();
const kick = json.variations[0].manifest.samples.find((s) => s.name === "kick");
const wavRes = await fetch(kick.url);
const buf = Buffer.from(await wavRes.arrayBuffer());
const dataOffset = 44;
const samples = [];
for (let i = dataOffset; i < buf.length; i += 2) {
  samples.push(buf.readInt16LE(i) / 32768);
}
let peak = 0;
let sumSq = 0;
for (const s of samples) {
  peak = Math.max(peak, Math.abs(s));
  sumSq += s * s;
}
const rms = Math.sqrt(sumSq / samples.length);
console.log(JSON.stringify({ provenance: kick.provenance, peak, rms, frames: samples.length }, null, 2));
