import { SAMPLE_RATE, encodeWav24Mono, downloadBlob, FILTER_PRESETS, noteToHz } from "./dsp-utils.js";
import { renderKick, KICK_STYLES } from "./kick-engine.js";
import { renderSnare, SNARE_STYLES } from "./snare-engine.js";

let audioCtx = null;

function getCtx() {
  if (!audioCtx) audioCtx = new AudioContext();
  return audioCtx;
}

export async function playBuffer(samples) {
  const ctx = getCtx();
  if (ctx.state === "suspended") await ctx.resume();
  const buf = ctx.createBuffer(1, samples.length, SAMPLE_RATE);
  buf.copyToChannel(samples, 0);
  const src = ctx.createBufferSource();
  src.buffer = buf;
  src.connect(ctx.destination);
  src.start();
}

export function drawWaveform(canvas, samples) {
  const ctx = canvas.getContext("2d");
  const w = canvas.width;
  const h = canvas.height;
  ctx.fillStyle = "#0a0a0c";
  ctx.fillRect(0, 0, w, h);
  ctx.strokeStyle = "#c4f542";
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  const step = Math.max(1, Math.floor(samples.length / w));
  for (let x = 0; x < w; x++) {
    const idx = x * step;
    const y = h / 2 - samples[idx] * (h * 0.42);
    if (x === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();
  ctx.fillStyle = "rgba(196,245,66,0.08)";
  ctx.fillRect(0, 0, w, h);
}

function bindRange(id, labelEl, fmt = (v) => v.toFixed(3)) {
  const el = document.getElementById(id);
  const label = document.getElementById(labelEl);
  const update = () => {
    if (label) label.textContent = fmt(parseFloat(el.value));
  };
  el.addEventListener("input", update);
  update();
  return el;
}

function readKickParams() {
  return {
    rootHz: parseFloat(document.getElementById("kick-root-hz").value),
    pitchModSt: parseFloat(document.getElementById("kick-pitch-mod").value),
    pitchDecay: parseFloat(document.getElementById("kick-pitch-decay").value) / 1000,
    duration: parseFloat(document.getElementById("kick-duration").value) / 1000,
    filterPreset: document.getElementById("kick-filter-type").value,
    ampA: parseFloat(document.getElementById("kick-amp-a").value) / 1000,
    ampD: parseFloat(document.getElementById("kick-amp-d").value) / 1000,
    ampS: parseFloat(document.getElementById("kick-amp-s").value),
    ampR: parseFloat(document.getElementById("kick-amp-r").value) / 1000,
    filA: parseFloat(document.getElementById("kick-fil-a").value) / 1000,
    filD: parseFloat(document.getElementById("kick-fil-d").value) / 1000,
    filS: parseFloat(document.getElementById("kick-fil-s").value),
    filR: parseFloat(document.getElementById("kick-fil-r").value) / 1000,
    lofiSr: parseFloat(document.getElementById("kick-lofi-sr").value),
    bitDepth: parseInt(document.getElementById("kick-bit-depth").value, 10),
    clipDriveDb: parseFloat(document.getElementById("kick-clip").value),
    tapeLpHz: parseFloat(document.getElementById("kick-tape-lp").value),
  };
}

function readSnareParams() {
  return {
    membraneHz: parseFloat(document.getElementById("snare-membrane").value),
    pitchModSt: parseFloat(document.getElementById("snare-pitch-mod").value),
    pitchDecay: parseFloat(document.getElementById("snare-pitch-decay").value) / 1000,
    duration: parseFloat(document.getElementById("snare-duration").value) / 1000,
    noiseCenter: parseFloat(document.getElementById("snare-noise-center").value),
    noiseDecay: parseFloat(document.getElementById("snare-noise-decay").value) / 1000,
    flamDelayMs: parseFloat(document.getElementById("snare-flam").value),
    bodyA: parseFloat(document.getElementById("snare-body-a").value) / 1000,
    bodyD: parseFloat(document.getElementById("snare-body-d").value) / 1000,
    bodyS: parseFloat(document.getElementById("snare-body-s").value),
    bodyR: parseFloat(document.getElementById("snare-body-r").value) / 1000,
    lofiSr: parseFloat(document.getElementById("snare-lofi-sr").value),
    bitDepth: parseInt(document.getElementById("snare-bit-depth").value, 10),
    clipDriveDb: parseFloat(document.getElementById("snare-clip").value),
    tapeLpHz: parseFloat(document.getElementById("snare-tape-lp").value),
  };
}

let lastKick = null;
let lastSnare = null;

function renderKickPage() {
  lastKick = renderKick(readKickParams());
  const canvas = document.getElementById("kick-canvas");
  drawWaveform(canvas, lastKick);
}

function renderSnarePage() {
  lastSnare = renderSnare(readSnareParams());
  const canvas = document.getElementById("snare-canvas");
  drawWaveform(canvas, lastSnare);
}

function setupNav() {
  document.querySelectorAll("[data-page]").forEach((btn) => {
    btn.addEventListener("click", () => {
      const page = btn.dataset.page;
      document.querySelectorAll("[data-page]").forEach((b) => b.classList.toggle("active", b === btn));
      document.querySelectorAll(".page").forEach((p) => p.classList.toggle("visible", p.id === `page-${page}`));
    });
  });
}

function populateFilterSelect(id) {
  const sel = document.getElementById(id);
  Object.values(FILTER_PRESETS).forEach((p) => {
    const opt = document.createElement("option");
    opt.value = p.id;
    opt.textContent = p.label;
    sel.appendChild(opt);
  });
}

function applyKickStyle(key) {
  const style = KICK_STYLES[key];
  if (!style) return;
  document.getElementById("kick-root-hz").value = style.rootHz;
  document.getElementById("kick-pitch-mod").value = style.pitchModSt;
  document.getElementById("kick-clip").value = style.clipDriveDb;
  if (style.tapeLpHz) document.getElementById("kick-tape-lp").value = style.tapeLpHz;
  renderKickPage();
}

function applySnareStyle(key) {
  const style = SNARE_STYLES[key];
  if (!style) return;
  document.getElementById("snare-membrane").value = style.membraneHz ?? 220;
  document.getElementById("snare-pitch-mod").value = style.pitchModSt ?? 24;
  document.getElementById("snare-clip").value = style.clipDriveDb ?? 5.5;
  if (style.flamDelayMs) document.getElementById("snare-flam").value = style.flamDelayMs;
  if (style.noiseCenter) document.getElementById("snare-noise-center").value = style.noiseCenter;
  if (style.tapeLpHz) document.getElementById("snare-tape-lp").value = style.tapeLpHz;
  renderSnarePage();
}

function wireKick() {
  populateFilterSelect("kick-filter-type");
  document.querySelectorAll("#page-kick input, #page-kick select").forEach((el) => {
    el.addEventListener("input", renderKickPage);
  });
  document.getElementById("kick-note").addEventListener("change", (e) => {
    const [note, oct] = e.target.value.split("-");
    document.getElementById("kick-root-hz").value = noteToHz(note, parseInt(oct, 10)).toFixed(2);
    renderKickPage();
  });
  document.querySelectorAll("[data-kick-style]").forEach((btn) => {
    btn.addEventListener("click", () => applyKickStyle(btn.dataset.kickStyle));
  });
  document.getElementById("kick-preview-btn").addEventListener("click", async () => {
    renderKickPage();
    await playBuffer(lastKick);
  });
  document.getElementById("kick-canvas").addEventListener("click", async () => {
    renderKickPage();
    await playBuffer(lastKick);
  });
  document.getElementById("kick-export").addEventListener("click", () => {
    renderKickPage();
    const wav = encodeWav24Mono(lastKick);
    downloadBlob(wav, "memphis_phonk_kick.wav");
  });
  renderKickPage();
}

function wireSnare() {
  document.querySelectorAll("#page-snare input").forEach((el) => {
    el.addEventListener("input", renderSnarePage);
  });
  document.querySelectorAll("[data-snare-style]").forEach((btn) => {
    btn.addEventListener("click", () => applySnareStyle(btn.dataset.snareStyle));
  });
  document.getElementById("snare-preview-btn").addEventListener("click", async () => {
    renderSnarePage();
    await playBuffer(lastSnare);
  });
  document.getElementById("snare-canvas").addEventListener("click", async () => {
    renderSnarePage();
    await playBuffer(lastSnare);
  });
  document.getElementById("snare-export").addEventListener("click", () => {
    renderSnarePage();
    const wav = encodeWav24Mono(lastSnare);
    downloadBlob(wav, "memphis_phonk_snare.wav");
  });
  renderSnarePage();
}

document.addEventListener("DOMContentLoaded", () => {
  setupNav();
  wireKick();
  wireSnare();
  [
    ["kick-root-hz", "kick-root-hz-val", (v) => `${v} Hz`],
    ["kick-pitch-mod", "kick-pitch-mod-val", (v) => `${v} st`],
  ].forEach(([a, b, f]) => bindRange(a, b, f));
});
