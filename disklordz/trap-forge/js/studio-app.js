import {
  SR,
  encodeStereoWav24,
  encodeMonoWav24,
  downloadBuffer,
  noteHz,
  normalizePeak,
  applyTrunkMasterBus,
} from "./dsp-core.js";
import { SYNTHS } from "./synth-trap.js";
import { renderHatRoll } from "./hat-roll.js";
import { refreshDndShelf } from "./export-dnd.js";
import {
  DRUM_ORDER,
  buildInitialDrums,
  PRODUCER_PRESETS,
  NOTE_FREQS,
  defaultParamsFor,
} from "./trap-presets.js";

export { DRUM_ORDER, NOTE_FREQS, PRODUCER_PRESETS };

export const kitState = {
  currentDrum: "kick",
  bpm: 140,
  playing: false,
  step: 0,
  drums: buildInitialDrums(),
  pattern: Object.fromEntries(DRUM_ORDER.map((d) => [d.id, new Array(16).fill(false)])),
  mutes: Object.fromEntries(DRUM_ORDER.map((d) => [d.id, false])),
  master: { ceilingDb: -0.3, drive: 0.15, trunkAmount: 0.38, distType: "fl_clip" },
  renderCache: {},
  hatRollCache: null,
};

let audioCtx = null;
const activeSources = new Set();
let seqTimer = null;
let lastWaveform = null;
let analyserNode = null;

function scaleStereoBuffer(buf, scale) {
  if (!scale || scale === 1) return buf;
  const left = buf.left.map((x) => x * scale);
  const right = buf.right.map((x) => x * scale);
  const mono = buf.mono ? buf.mono.map((x) => x * scale) : left;
  return { left, right, mono };
}

function resolveSub808Glide(p) {
  const out = { ...p };
  const f0 = out.rootHz ?? 55;
  if (out.glideTargetHz != null) return out;
  if (out.glideSemi) out.glideTargetHz = f0 * Math.pow(2, out.glideSemi / 12);
  else if (out.glideTarget != null) out.glideTargetHz = out.glideTarget;
  return out;
}

/** Render one drum hit; `velocityScale` scales output amplitude (0–1). */
export function renderDrumSample(drumType, stateOverride = null, velocityScale = 1) {
  const synth = SYNTHS[drumType];
  if (!synth) throw new Error(`Unknown drum ${drumType}`);
  let p = { ...(stateOverride || kitState.drums[drumType]) };
  if (drumType === "sub808") p = resolveSub808Glide(p);
  let buf = synth(p);
  if (velocityScale !== 1) buf = scaleStereoBuffer(buf, velocityScale);
  return buf;
}

function applyMasterStereo(left, right) {
  return applyTrunkMasterBus(left, right, kitState.master);
}

function ensureAudio() {
  if (!audioCtx) {
    audioCtx = new AudioContext({ sampleRate: SR });
    analyserNode = audioCtx.createAnalyser();
    analyserNode.fftSize = 2048;
    analyserNode.connect(audioCtx.destination);
  }
  if (audioCtx.state === "suspended") audioCtx.resume();
  return audioCtx;
}

if (typeof window !== "undefined") {
  ["click", "keydown", "touchstart"].forEach((ev) => {
    window.addEventListener(ev, () => ensureAudio(), { once: false, passive: true });
  });
}

export function playStereo(buffer, velocity = 1) {
  const ctx = ensureAudio();
  const n = Math.min(buffer.left.length, buffer.right.length);
  const ab = ctx.createBuffer(2, n, SR);
  ab.copyToChannel(buffer.left, 0);
  ab.copyToChannel(buffer.right, 1);
  const src = ctx.createBufferSource();
  src.buffer = ab;
  const gain = ctx.createGain();
  gain.gain.value = velocity;
  src.connect(gain);
  gain.connect(analyserNode);
  src.start();
  activeSources.add(src);
  src.onended = () => activeSources.delete(src);
  updatePeakMeter(buffer);
  return src;
}

function updatePeakMeter(buffer) {
  let peak = 0;
  for (let i = 0; i < buffer.left.length; i++) {
    peak = Math.max(peak, Math.abs(buffer.left[i]), Math.abs(buffer.right[i]));
  }
  const el = document.getElementById("peak-meter");
  if (!el) return;
  const pct = Math.min(100, peak * 100);
  el.style.width = `${pct}%`;
  el.classList.toggle("clip", peak > 0.99);
}

export function selectAndAuditionDrum(id, { play = true } = {}) {
  if (!SYNTHS[id]) return;
  kitState.currentDrum = id;
  document.querySelectorAll("[data-type]").forEach((btn) => {
    btn.classList.toggle("ring-2", btn.dataset.type === id);
    btn.classList.toggle("ring-emerald-400", btn.dataset.type === id);
  });
  document.querySelectorAll("[data-drum-panel]").forEach((el) => {
    el.classList.toggle("hidden", el.dataset.drumPanel !== id);
  });
  renderDynamicControls(id);
  syncUIFromState();
  updateAndRenderCurrent(play);
}

export function updateAndRenderCurrent(play = false) {
  const id = kitState.currentDrum;
  let buf = renderDrumSample(id, null, 1);
  buf = applyMasterStereo(buf.left, buf.right);
  kitState.renderCache[id] = buf;
  lastWaveform = buf.mono || buf.left;
  drawVisualizer(lastWaveform);
  refreshExportShelfUI();
  if (play) playStereo(buf);
}

export function syncUIFromState() {
  const id = kitState.currentDrum;
  const p = kitState.drums[id];
  document.querySelectorAll("[data-param]").forEach((el) => {
    const key = el.dataset.param;
    if (p[key] === undefined) return;
    if (el.type === "range" || el.type === "number") el.value = p[key];
    else if (el.tagName === "SELECT") el.value = String(p[key]);
    const label = el.parentElement?.querySelector("[data-val]");
    if (label) label.textContent = String(el.value);
  });
  const valEl = document.getElementById("param-seq-tempo-val");
  if (valEl) valEl.textContent = String(kitState.bpm);
  const bpmSlider = document.getElementById("param-seq-tempo");
  if (bpmSlider) bpmSlider.value = kitState.bpm;
}

export function renderDynamicControls(drumId) {
  const host = document.getElementById("dynamic-controls");
  if (!host) return;
  host.innerHTML = "";
  if (drumId !== "sub808") return;
  const p = kitState.drums.sub808;
  const wrap = document.createElement("div");
  wrap.className = "grid grid-cols-2 gap-3 mt-3";
  wrap.innerHTML = `
    <label class="text-xs text-slate-300">Glide ms
      <input type="range" data-param="glideMs" min="0" max="450" step="5" value="${p.glideMs ?? 0}" class="w-full" />
      <span data-val class="text-emerald-400">${p.glideMs ?? 0}</span>
    </label>
    <label class="text-xs text-slate-300">Glide semi
      <input type="range" data-param="glideSemi" min="-12" max="12" step="1" value="${p.glideSemi ?? 0}" class="w-full" />
      <span data-val class="text-emerald-400">${p.glideSemi ?? 0}</span>
    </label>
    <label class="text-xs text-slate-300 col-span-2">Target Hz (optional)
      <input type="number" data-param="glideTargetHz" min="30" max="200" step="1" value="${p.glideTargetHz ?? ""}" placeholder="auto from semi" class="w-full bg-slate-900 border border-slate-700 rounded px-2 py-1" />
    </label>`;
  host.appendChild(wrap);
  wrap.querySelectorAll("[data-param]").forEach(bindParam);
}

function bindParam(el) {
  el.addEventListener("input", () => {
    const id = kitState.currentDrum;
    const key = el.dataset.param;
    let val = el.type === "range" || el.type === "number" ? parseFloat(el.value) : el.value;
    if (key === "filterMode" || key === "distType" || key === "rootNote") val = el.value;
    if (key === "rootNote") {
      kitState.drums[id].rootHz = NOTE_FREQS[el.value] ?? noteHz(el.value, 1);
      kitState.renderCache = {};
      updateAndRenderCurrent(false);
      return;
    }
    if (key === "glideTargetHz" && (el.value === "" || Number.isNaN(val))) {
      kitState.drums[id].glideTargetHz = null;
      kitState.renderCache = {};
      updateAndRenderCurrent(false);
      return;
    }
    kitState.drums[id][key] = val;
    kitState.renderCache = {};
    kitState.hatRollCache = null;
    const label = el.parentElement?.querySelector("[data-val]");
    if (label) label.textContent = el.value;
    updateAndRenderCurrent(false);
  });
}

export function drawVisualizer(mono) {
  const scope = document.getElementById("canvas-visualizer");
  const fftCanvas = document.getElementById("canvas-fft");
  if (!scope || !mono) return;
  const w = scope.width;
  const h = scope.height;
  const ctx = scope.getContext("2d");
  ctx.fillStyle = "#050d1f";
  ctx.fillRect(0, 0, w, h);
  ctx.strokeStyle = "#7dff9a";
  ctx.lineWidth = 1;
  ctx.beginPath();
  const step = Math.max(1, Math.floor(mono.length / w));
  for (let x = 0; x < w; x++) {
    const i = x * step;
    const y = h / 2 - mono[i] * (h * 0.42);
    if (x === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();

  if (fftCanvas) {
    const fctx = fftCanvas.getContext("2d");
    fctx.fillStyle = "#050d1f";
    fctx.fillRect(0, 0, fftCanvas.width, fftCanvas.height);
    const bins = 256;
    for (let b = 0; b < bins; b++) {
      const start = Math.floor((b / bins) * mono.length);
      const end = Math.floor(((b + 1) / bins) * mono.length);
      let e = 0;
      for (let i = start; i < end; i++) e += mono[i] * mono[i];
      e = Math.sqrt(e / (end - start + 1));
      const barH = Math.min(fftCanvas.height, e * fftCanvas.height * 8);
      fctx.fillStyle = `rgba(91,140,255,${0.35 + b / bins})`;
      fctx.fillRect(b * (fftCanvas.width / bins), fftCanvas.height - barH, fftCanvas.width / bins - 1, barH);
    }
  }
}

function resizeCanvas() {
  ["canvas-visualizer", "canvas-fft"].forEach((id) => {
    const c = document.getElementById(id);
    if (!c) return;
    const rect = c.parentElement.getBoundingClientRect();
    c.width = Math.floor(rect.width * devicePixelRatio);
    c.height = Math.floor(90 * devicePixelRatio);
  });
  if (lastWaveform) drawVisualizer(lastWaveform);
}

function buildSequencerUI() {
  const root = document.getElementById("sequencer-rows");
  if (!root) return;
  root.innerHTML = "";
  DRUM_ORDER.forEach((d) => {
    const row = document.createElement("div");
    row.className = "flex gap-1 items-center mb-1";
    const lab = document.createElement("div");
    lab.className = "w-14 text-[10px] text-slate-400 flex gap-1";
    lab.innerHTML = `<span>${d.short}</span>`;
    const mute = document.createElement("button");
    mute.type = "button";
    mute.className = "text-[10px] px-1 rounded bg-slate-800";
    mute.textContent = "M";
    mute.addEventListener("click", () => {
      kitState.mutes[d.id] = !kitState.mutes[d.id];
      mute.classList.toggle("bg-red-900", kitState.mutes[d.id]);
    });
    const solo = document.createElement("button");
    solo.type = "button";
    solo.className = "text-[10px] px-1 rounded bg-slate-800";
    solo.textContent = "▶";
    solo.addEventListener("click", () => selectAndAuditionDrum(d.id));
    lab.append(mute, solo);
    row.appendChild(lab);
    for (let s = 0; s < 16; s++) {
      const btn = document.createElement("button");
      btn.type = "button";
      btn.className = `step w-5 h-5 rounded-sm border border-slate-700 ${s % 4 === 0 ? "bg-slate-900" : "bg-slate-950"}`;
      btn.dataset.row = d.id;
      btn.dataset.step = s;
      btn.addEventListener("click", () => {
        kitState.pattern[d.id][s] = !kitState.pattern[d.id][s];
        btn.classList.toggle("bg-emerald-500", kitState.pattern[d.id][s]);
      });
      row.appendChild(btn);
    }
    root.appendChild(row);
  });
}

function refreshSequencerSteps() {
  document.querySelectorAll(".step[data-row]").forEach((btn) => {
    const id = btn.dataset.row;
    const s = parseInt(btn.dataset.step, 10);
    btn.classList.toggle("bg-emerald-500", kitState.pattern[id][s]);
    btn.classList.toggle("ring-1", kitState.playing && kitState.step === s);
    btn.classList.toggle("ring-yellow-400", kitState.playing && kitState.step === s);
  });
}

function triggerStep(step) {
  DRUM_ORDER.forEach((d) => {
    if (!kitState.pattern[d.id][step] || kitState.mutes[d.id]) return;
    let buf = kitState.renderCache[d.id];
    if (!buf) {
      const raw = renderDrumSample(d.id, null, 1);
      buf = applyMasterStereo(raw.left, raw.right);
      kitState.renderCache[d.id] = buf;
    }
    playStereo(buf, 0.92);
  });
}

function stopSequencer() {
  kitState.playing = false;
  if (seqTimer) {
    clearInterval(seqTimer);
    seqTimer = null;
  }
  refreshSequencerSteps();
}

function startSequencer() {
  ensureAudio();
  kitState.playing = true;
  kitState.step = 0;
  const stepMs = () => (60 / kitState.bpm / 4) * 1000;
  triggerStep(0);
  refreshSequencerSteps();
  seqTimer = setInterval(() => {
    kitState.step = (kitState.step + 1) % 16;
    triggerStep(kitState.step);
    refreshSequencerSteps();
  }, stepMs());
}

export function encodeDrumWav(id, buf) {
  const b = buf || kitState.renderCache[id];
  if (!b) return null;
  if (id === "kick" || id === "sub808") return encodeMonoWav24(b.mono || b.left);
  return encodeStereoWav24(b.left, b.right);
}

function exportDrumWav(id, suffix = "") {
  let buf = kitState.renderCache[id] || renderDrumSample(id, null, 1);
  buf = applyMasterStereo(buf.left, buf.right);
  const wav = encodeDrumWav(id, buf);
  downloadBuffer(wav, `trap-forge_${id}${suffix}.wav`);
}

export function exportMultiVelocityPack() {
  const velocities = [
    { v: 1, tag: "_hard" },
    { v: 0.72, tag: "_med" },
    { v: 0.42, tag: "_soft" },
  ];
  DRUM_ORDER.forEach((d) => {
    velocities.forEach(({ v, tag }) => {
      const raw = renderDrumSample(d.id, null, v);
      const m = applyMasterStereo(raw.left, raw.right);
      const wav = encodeDrumWav(d.id, m);
      downloadBuffer(wav, `trap-forge_${d.id}${tag}.wav`);
    });
  });
}

function exportFullKit() {
  DRUM_ORDER.forEach((d) => exportDrumWav(d.id));
}

function getHatRollOptions() {
  const divEl = document.getElementById("hat-roll-div");
  const div = divEl?.value || "16";
  return {
    bpm: kitState.bpm,
    division: div,
    count: parseInt(document.getElementById("hat-roll-count")?.value || "8", 10),
    ramp: document.getElementById("hat-roll-ramp")?.value || "crescendo",
    pitchDrift: parseFloat(document.getElementById("hat-roll-drift")?.value || "0.006"),
    timingJitter: parseFloat(document.getElementById("hat-roll-jitter")?.value || "0.018"),
    pitchSlide: parseFloat(document.getElementById("hat-roll-slide")?.value || "0"),
  };
}

function buildHatRollBuffer() {
  const opts = getHatRollOptions();
  const p = kitState.drums.closedhat;
  let roll = renderHatRoll((params, vel) => renderDrumSample("closedhat", params, vel), p, opts);
  roll.left = normalizePeak(roll.left, -0.3);
  roll.right = normalizePeak(roll.right, -0.3);
  roll.mono = normalizePeak(roll.mono, -0.3);
  roll = applyMasterStereo(roll.left, roll.right);
  roll.mono = roll.left;
  kitState.hatRollCache = roll;
  return roll;
}

function wavForDrum(id) {
  let buf = kitState.renderCache[id];
  if (!buf) {
    const raw = renderDrumSample(id, null, 1);
    buf = applyMasterStereo(raw.left, raw.right);
  }
  return encodeDrumWav(id, buf);
}

function refreshExportShelfUI() {
  const shelf = document.getElementById("dnd-shelf");
  if (!shelf) return;
  const items = DRUM_ORDER.map((d) => ({
    id: d.id,
    label: d.short,
    getBuffer: () => wavForDrum(d.id),
    filename: () => `trap-forge_${d.id}.wav`,
  }));
  items.push({
    id: "hatRoll",
    label: "HAT ROLL",
    getBuffer: () => {
      const roll = kitState.hatRollCache || buildHatRollBuffer();
      return encodeStereoWav24(roll.left, roll.right);
    },
    filename: () => `trap-forge_hat_roll_${document.getElementById("hat-roll-div")?.value || "16"}.wav`,
  });
  refreshDndShelf(shelf, items);
}

function auditionHatRoll() {
  selectAndAuditionDrum("closedhat", { play: false });
  const roll = buildHatRollBuffer();
  lastWaveform = roll.mono;
  drawVisualizer(lastWaveform);
  playStereo(roll);
  refreshExportShelfUI();
}

function exportHatRollWav() {
  const roll = buildHatRollBuffer();
  downloadBuffer(encodeStereoWav24(roll.left, roll.right), `trap-forge_hat_roll_${getHatRollOptions().division}.wav`);
}

function applyPreset(key) {
  const preset = PRODUCER_PRESETS[key];
  if (!preset) return;
  const id = preset.drum || kitState.currentDrum;
  selectAndAuditionDrum(id, { play: false });
  const { drum, label, ...params } = preset;
  Object.assign(kitState.drums[id], params);
  kitState.renderCache = {};
  syncUIFromState();
  renderDynamicControls(id);
  updateAndRenderCurrent(true);
}

function fillPresetSelector() {
  const sel = document.getElementById("preset-selector");
  if (!sel || sel.options.length > 1) return;
  Object.entries(PRODUCER_PRESETS).forEach(([key, p]) => {
    const opt = document.createElement("option");
    opt.value = key;
    opt.textContent = p.label || key;
    sel.appendChild(opt);
  });
}

function initUI() {
  const tabNav = document.getElementById("drum-tabs");
  DRUM_ORDER.forEach((d, idx) => {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.dataset.type = d.id;
    btn.className =
      "px-3 py-2 text-sm rounded-lg bg-slate-800 hover:bg-slate-700 text-left";
    btn.textContent = d.label;
    btn.addEventListener("click", () => selectAndAuditionDrum(d.id));
    tabNav?.appendChild(btn);
    const pads = document.getElementById("pads");
    if (pads) {
      const pad = document.createElement("button");
      pad.type = "button";
      pad.className = "w-10 h-10 rounded bg-slate-800 hover:bg-emerald-700 text-sm";
      pad.textContent = String(idx + 1);
      pad.addEventListener("click", () => selectAndAuditionDrum(d.id));
      pads.appendChild(pad);
    }
  });

  document.querySelectorAll("[data-param]").forEach(bindParam);
  fillPresetSelector();

  document.getElementById("btn-trigger")?.addEventListener("click", () => updateAndRenderCurrent(true));
  document.getElementById("canvas-visualizer")?.addEventListener("click", () => updateAndRenderCurrent(true));
  document.getElementById("btn-download")?.addEventListener("click", () => exportDrumWav(kitState.currentDrum));
  document.getElementById("btn-export-all")?.addEventListener("click", exportFullKit);
  document.getElementById("btn-export-vel")?.addEventListener("click", exportMultiVelocityPack);
  document.getElementById("btn-seq-play")?.addEventListener("click", () => {
    if (kitState.playing) stopSequencer();
    else startSequencer();
  });
  document.getElementById("btn-seq-clear")?.addEventListener("click", () => {
    DRUM_ORDER.forEach((d) => kitState.pattern[d.id].fill(false));
    refreshSequencerSteps();
  });
  document.getElementById("param-seq-tempo")?.addEventListener("input", (e) => {
    kitState.bpm = parseInt(e.target.value, 10);
    const val = document.getElementById("param-seq-tempo-val");
    if (val) val.textContent = String(kitState.bpm);
    if (kitState.playing) {
      stopSequencer();
      startSequencer();
    }
  });
  document.getElementById("preset-selector")?.addEventListener("change", (e) => {
    if (e.target.value) applyPreset(e.target.value);
  });
  document.getElementById("btn-hat-roll")?.addEventListener("click", auditionHatRoll);
  document.getElementById("btn-hat-roll-export")?.addEventListener("click", exportHatRollWav);
  document.getElementById("master-ceiling")?.addEventListener("input", (e) => {
    kitState.master.ceilingDb = parseFloat(e.target.value);
    kitState.renderCache = {};
  });
  document.getElementById("master-drive")?.addEventListener("input", (e) => {
    kitState.master.drive = parseFloat(e.target.value);
    kitState.renderCache = {};
  });
  document.getElementById("master-trunk")?.addEventListener("input", (e) => {
    kitState.master.trunkAmount = parseFloat(e.target.value);
    kitState.renderCache = {};
  });

  window.addEventListener("keydown", (e) => {
    if (e.code === "Space") {
      e.preventDefault();
      updateAndRenderCurrent(true);
    }
    const num = parseInt(e.key, 10);
    if (num >= 1 && num <= 7) selectAndAuditionDrum(DRUM_ORDER[num - 1].id);
  });

  buildSequencerUI();
  resizeCanvas();
  window.addEventListener("resize", resizeCanvas);
  selectAndAuditionDrum("kick", { play: false });
  refreshExportShelfUI();
}

if (typeof document !== "undefined") {
  if (document.readyState === "loading") document.addEventListener("DOMContentLoaded", initUI);
  else initUI();
}
