import { SR, encodeStereoWav24, encodeMonoWav24, downloadBuffer, noteHz, normalizePeak, applyTrunkMasterBus } from "./dsp-core.js";
import { SYNTHS, PRESETS, synthHatClosed } from "./synth-trap.js";
import { renderHatRoll } from "./hat-roll.js";
import { refreshDndShelf } from "./export-dnd.js";

export const DRUM_ORDER = [
  { id: "kick", label: "Punch Kick", short: "KICK" },
  { id: "sub808", label: "Trap 808", short: "808" },
  { id: "snare", label: "Crack Snare", short: "SNR" },
  { id: "clap", label: "Dirty Clap", short: "CLAP" },
  { id: "hatClosed", label: "Closed Hat", short: "CH" },
  { id: "hatOpen", label: "Open Hat", short: "OH" },
  { id: "perc", label: "Perc / Cowbell", short: "PERC" },
];

function baseEnvelopeParams() {
  return {
    aA: 0.001,
    aD: 0.1,
    aS: 0,
    aR: 0.05,
    fA: 0.002,
    fD: 0.08,
    fS: 0.15,
    fR: 0.06,
    fCut: 7500,
    fEnvAmt: 18,
    fQ: 0.85,
    filterMode: "lp12",
    drive: 0.42,
    bits: 12,
    lofiSr: 26040,
    ceiling: 0.92,
    transAttack: 0,
    transSustain: 0,
    transAttackDb: 0,
    transSustainDb: 0,
    oversample: true,
    peakDb: -0.3,
  };
}

export function defaultParamsFor(id) {
  const p = { ...baseEnvelopeParams(), ...drumDefaults[id]() };
  return p;
}

const drumDefaults = {
  kick: () => ({ duration: 0.28, rootHz: 50, pitchMod: 38, pitchDecay: 0.02, aD: 0.14, fCut: 4200 }),
  sub808: () => ({ duration: 2, rootHz: 55, aA: 0.002, aD: 0.45, aS: 0.88, aR: 0.4, fCut: 280, harm2: 0.38, harm3: 0.24, glideMs: 0, glideTarget: 55 }),
  snare: () => ({
    duration: 0.32,
    pitchSemi: 0,
    snap: 0.68,
    snapBite: 0.55,
    fCut: 6200,
    reverbMix: 0.28,
    reverbDecay: 0.58,
    reverbDamp: 0.52,
    reverbPre: 14,
  }),
  clap: () => ({
    duration: 0.34,
    flamMs: 0.022,
    reverbMix: 0.24,
    reverbDecay: 0.55,
    reverbDamp: 0.5,
    reverbPre: 18,
  }),
  hatClosed: () => ({ duration: 0.055, aD: 0.022, fCut: 7800, dispersion: 0.01 }),
  hatOpen: () => ({ duration: 0.4, aD: 0.2, aS: 0.1, fCut: 5200 }),
  perc: () => ({ duration: 0.14, rootHz: 540, fCut: 4000 }),
};

export const kitState = {
  currentDrum: "kick",
  bpm: 140,
  playing: false,
  step: 0,
  pattern: Object.fromEntries(DRUM_ORDER.map((d) => [d.id, new Array(16).fill(false)])),
  mutes: Object.fromEntries(DRUM_ORDER.map((d) => [d.id, false])),
  params: Object.fromEntries(DRUM_ORDER.map((d) => [d.id, defaultParamsFor(d.id)])),
  master: { ceilingDb: -0.3, drive: 0.15, trunkAmount: 0.38 },
  renderCache: {},
  hatRollCache: null,
};

let audioCtx = null;
const activeSources = new Set();
let seqTimer = null;
let lastWaveform = null;
let analyserNode = null;

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

["click", "keydown", "touchstart"].forEach((ev) => {
  window.addEventListener(ev, () => ensureAudio(), { once: false, passive: true });
});

export function renderDrum(id, paramOverride = null) {
  const p = { ...(paramOverride || kitState.params[id]) };
  const synth = SYNTHS[id];
  if (!synth) throw new Error(`Unknown drum ${id}`);
  return synth(p);
}

function applyMasterStereo(left, right) {
  return applyTrunkMasterBus(left, right, kitState.master);
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
  document.querySelectorAll("nav.tabs button").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.drum === id);
  });
  document.querySelectorAll(".drum-panel").forEach((el) => {
    el.classList.toggle("hidden", el.dataset.drum !== id);
  });
  syncUIFromState();
  updateAndRenderCurrent(play);
}

export function updateAndRenderCurrent(play = false) {
  const id = kitState.currentDrum;
  let buf = renderDrum(id);
  buf = applyMasterStereo(buf.left, buf.right);
  kitState.renderCache[id] = buf;
  lastWaveform = buf.mono || buf.left;
  drawVisualizer(lastWaveform);
  refreshExportShelfUI();
  if (play) playStereo(buf);
}

export function syncUIFromState() {
  const id = kitState.currentDrum;
  const p = kitState.params[id];
  document.querySelectorAll("[data-param]").forEach((el) => {
    const key = el.dataset.param;
    if (p[key] === undefined) return;
    if (el.type === "range" || el.type === "number") el.value = p[key];
    else if (el.tagName === "SELECT") el.value = String(p[key]);
  });
  const valEl = document.getElementById("bpm-val");
  if (valEl) valEl.textContent = String(kitState.bpm);
  const bpmSlider = document.getElementById("bpm");
  if (bpmSlider) bpmSlider.value = kitState.bpm;
}

function bindParam(el) {
  el.addEventListener("input", () => {
    const id = kitState.currentDrum;
    const key = el.dataset.param;
    let val = el.type === "range" || el.type === "number" ? parseFloat(el.value) : el.value;
    if (key === "filterMode") val = el.value;
    if (key === "rootNote") {
      kitState.params[id].rootHz = noteHz(el.value, 0);
      kitState.renderCache = {};
      updateAndRenderCurrent(false);
      return;
    }
    if (key === "glideInterval") {
      const semi = parseInt(el.value, 10);
      const root = kitState.params[id].rootHz ?? 55;
      kitState.params[id].glideTarget = semi === 0 ? root : root * Math.pow(2, semi / 12);
      kitState.renderCache = {};
      updateAndRenderCurrent(false);
      return;
    }
    kitState.params[id][key] = val;
    kitState.renderCache = {};
    kitState.hatRollCache = null;
    const label = el.parentElement.querySelector(".val");
    if (label) label.textContent = el.value;
    updateAndRenderCurrent(false);
  });
}

export function drawVisualizer(mono) {
  const scope = document.getElementById("scope");
  const fftCanvas = document.getElementById("fft");
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
  ["scope", "fft"].forEach((id) => {
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
  DRUM_ORDER.forEach((d, rowIdx) => {
    const row = document.createElement("div");
    row.className = "seq-row";
    const lab = document.createElement("div");
    lab.className = "seq-label";
    lab.innerHTML = `<span>${d.short}</span>`;
    const mute = document.createElement("button");
    mute.type = "button";
    mute.textContent = "M";
    mute.title = "Mute";
    mute.addEventListener("click", () => {
      kitState.mutes[d.id] = !kitState.mutes[d.id];
      mute.classList.toggle("btn", kitState.mutes[d.id]);
    });
    const solo = document.createElement("button");
    solo.type = "button";
    solo.textContent = "▶";
    solo.title = "Audition";
    solo.addEventListener("click", () => selectAndAuditionDrum(d.id));
    lab.append(mute, solo);
    row.appendChild(lab);
    for (let s = 0; s < 16; s++) {
      const btn = document.createElement("button");
      btn.type = "button";
      btn.className = `step ${s % 8 < 4 ? "beat-a" : "beat-b"}`;
      btn.dataset.row = d.id;
      btn.dataset.step = s;
      btn.addEventListener("click", () => {
        kitState.pattern[d.id][s] = !kitState.pattern[d.id][s];
        btn.classList.toggle("active", kitState.pattern[d.id][s]);
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
    btn.classList.toggle("active", kitState.pattern[id][s]);
    btn.classList.toggle("playhead", kitState.playing && kitState.step === s);
  });
  document.querySelectorAll(".led").forEach((led, i) => {
    led.classList.toggle("on", kitState.pattern[kitState.currentDrum]?.[i]);
    led.classList.toggle("play", kitState.playing && kitState.step === i);
  });
}

function triggerStep(step) {
  DRUM_ORDER.forEach((d) => {
    if (!kitState.pattern[d.id][step] || kitState.mutes[d.id]) return;
    let buf = kitState.renderCache[d.id];
    if (!buf) {
      const raw = renderDrum(d.id);
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

function exportDrumWav(id, suffix = "") {
  let buf = kitState.renderCache[id] || renderDrum(id);
  buf = applyMasterStereo(buf.left, buf.right);
  const wav =
    id === "kick" || id === "sub808"
      ? encodeMonoWav24(buf.mono || buf.left)
      : encodeStereoWav24(buf.left, buf.right);
  downloadBuffer(wav, `trap-forge_${id}${suffix}.wav`);
}

function exportVelocityPack() {
  const velocities = [
    { v: 1, tag: "_hard" },
    { v: 0.72, tag: "_med" },
    { v: 0.42, tag: "_soft" },
  ];
  DRUM_ORDER.forEach((d) => {
    velocities.forEach(({ v, tag }) => {
      const raw = renderDrum(d.id);
      const scaled = {
        left: raw.left.map((x) => x * v),
        right: raw.right.map((x) => x * v),
        mono: raw.mono?.map((x) => x * v),
      };
      const m = applyMasterStereo(scaled.left, scaled.right);
      const wav =
        d.id === "kick" || d.id === "sub808"
          ? encodeMonoWav24(m.left)
          : encodeStereoWav24(m.left, m.right);
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
  const p = kitState.params.hatClosed;
  let roll = renderHatRoll(
    (params, vel) => {
      const hit = synthHatClosed(params);
      return {
        left: hit.left.map((x) => x * vel),
        right: hit.right.map((x) => x * vel),
        mono: hit.mono.map((x) => x * vel),
      };
    },
    p,
    opts,
  );
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
    const raw = renderDrum(id);
    buf = applyMasterStereo(raw.left, raw.right);
  }
  if (id === "kick" || id === "sub808") return encodeMonoWav24(buf.mono || buf.left);
  return encodeStereoWav24(buf.left, buf.right);
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
  selectAndAuditionDrum("hatClosed", { play: false });
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
  const preset = PRESETS[key];
  if (!preset) return;
  const map = {
    jeezyKick: "kick",
    drummaKick: "kick",
    shawty808: "sub808",
    mike808: "sub808",
    gucciSnare: "snare",
    cardoSnare: "snare",
    drummaClap: "clap",
    cardoHat: "hatClosed",
    sledgrenHat: "hatClosed",
    estPerc: "perc",
  };
  const id = map[key] || kitState.currentDrum;
  selectAndAuditionDrum(id, { play: false });
  Object.assign(kitState.params[id], preset);
  kitState.renderCache = {};
  syncUIFromState();
  updateAndRenderCurrent(true);
}

function initUI() {
  const tabNav = document.getElementById("drum-tabs");
  DRUM_ORDER.forEach((d, idx) => {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.dataset.drum = d.id;
    btn.textContent = d.label;
    btn.addEventListener("click", () => selectAndAuditionDrum(d.id));
    tabNav.appendChild(btn);
    const pads = document.getElementById("pads");
    if (pads) {
      const pad = document.createElement("button");
      pad.type = "button";
      pad.className = "pad";
      pad.textContent = String(idx + 1);
      pad.addEventListener("click", () => selectAndAuditionDrum(d.id));
      pads.appendChild(pad);
    }
  });

  document.querySelectorAll("[data-param]").forEach(bindParam);

  document.getElementById("btn-play")?.addEventListener("click", () => updateAndRenderCurrent(true));
  document.getElementById("scope")?.addEventListener("click", () => updateAndRenderCurrent(true));
  document.getElementById("btn-export-one")?.addEventListener("click", () => exportDrumWav(kitState.currentDrum));
  document.getElementById("btn-export-kit")?.addEventListener("click", exportFullKit);
  document.getElementById("btn-export-vel")?.addEventListener("click", exportVelocityPack);
  document.getElementById("btn-seq-play")?.addEventListener("click", () => {
    if (kitState.playing) stopSequencer();
    else startSequencer();
  });
  document.getElementById("btn-seq-clear")?.addEventListener("click", () => {
    DRUM_ORDER.forEach((d) => kitState.pattern[d.id].fill(false));
    refreshSequencerSteps();
  });
  document.getElementById("bpm")?.addEventListener("input", (e) => {
    kitState.bpm = parseInt(e.target.value, 10);
    document.getElementById("bpm-val").textContent = String(kitState.bpm);
    if (kitState.playing) {
      stopSequencer();
      startSequencer();
    }
  });
  document.getElementById("preset-select")?.addEventListener("change", (e) => {
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
  const bar = document.getElementById("led-bar");
  if (bar && !bar.children.length) {
    for (let i = 0; i < 16; i++) {
      const d = document.createElement("div");
      d.className = "led";
      bar.appendChild(d);
    }
  }
  resizeCanvas();
  window.addEventListener("resize", resizeCanvas);
  selectAndAuditionDrum("kick", { play: false });
  refreshExportShelfUI();
}

initUI();
