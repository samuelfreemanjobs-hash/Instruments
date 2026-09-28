/**
 * Lofi-12 step sequencer — 6 tracks, Web Audio preview, Web MIDI, backing loop API.
 */

import {
  previewNote,
  resumeAudio,
  playBackingArrayBuffer,
  stopBacking,
  restartBacking,
  hasBacking,
} from "./audio_engine.js";

const STEPS = 16;
const TRACK_NAMES = ["Kick", "Snare", "Hat", "Cowbell", "Perc", "OpenHat"];
const DEFAULT_NOTES = [36, 39, 42, 45, 48, 46]; // slots 1,4,7,10,13,8
const SLOT_BASE = 36;

/** @type {{ on: boolean, note: number, velocity: number }[][]} */
let pattern = TRACK_NAMES.map((_, ti) =>
  Array.from({ length: STEPS }, () => ({
    on: false,
    note: DEFAULT_NOTES[ti],
    velocity: 100,
  }))
);

let currentStep = -1;
let playing = false;
let timerId = null;
/** @type {MIDIOutput | null} */
let midiOut = null;
let clockTimer = null;
let selected = { ti: 0, si: 0 };
/** @type {object[]} */
let presetCache = [];
/** @type {ArrayBuffer | null} */
let lastBackingBuffer = null;

const gridEl = document.getElementById("grid");
const padRowEl = document.getElementById("padRow");
const statusEl = document.getElementById("status");
const bpmEl = document.getElementById("bpm");
const midiSelect = document.getElementById("midiOut");
const editNoteEl = document.getElementById("editNote");
const editVelEl = document.getElementById("editVel");

function slotToNote(slot) {
  return SLOT_BASE + slot - 1;
}
function noteToSlot(note) {
  return note - SLOT_BASE + 1;
}

function selectedPresetId() {
  return document.getElementById("stylePreset").value || null;
}

function fxValues() {
  return {
    filter: Number(document.getElementById("fxFilter").value),
    reverb: Number(document.getElementById("fxReverb").value),
    tape: Number(document.getElementById("fxTape").value),
    drive: Number(document.getElementById("fxDrive").value),
  };
}

function sendFxCc() {
  if (!midiOut) return;
  const fx = fxValues();
  const ch = 0;
  midiOut.send([0xb0 + ch, 38, Math.floor(fx.filter * 127)]);
  midiOut.send([0xb0 + ch, 36, Math.floor(fx.reverb * 127)]);
}

function buildGrid() {
  gridEl.innerHTML = "";
  const head = document.createElement("div");
  head.className = "row";
  head.innerHTML =
    '<span class="row-label"></span>' +
    Array.from({ length: STEPS }, (_, i) => `<span class="row-label">${i + 1}</span>`).join("");
  gridEl.appendChild(head);

  TRACK_NAMES.forEach((name, ti) => {
    const row = document.createElement("div");
    row.className = "row";
    const label = document.createElement("span");
    label.className = "row-label";
    label.textContent = name;
    label.title = "Double-click row label: set default slot";
    label.addEventListener("dblclick", () => {
      const slot = prompt(`Default slot 1–16 for ${name}:`, String(noteToSlot(DEFAULT_NOTES[ti])));
      if (slot) {
        const n = Math.min(16, Math.max(1, parseInt(slot, 10) || 1));
        DEFAULT_NOTES[ti] = slotToNote(n);
        pattern[ti].forEach((c) => (c.note = slotToNote(n)));
        syncGridUi();
      }
    });
    row.appendChild(label);

    for (let si = 0; si < STEPS; si++) {
      const btn = document.createElement("button");
      btn.type = "button";
      btn.className = "step";
      btn.dataset.track = String(ti);
      btn.dataset.step = String(si);
      btn.title = "Click: toggle · Shift: slot · Alt: velocity · Click selects for editor";
      btn.addEventListener("click", (ev) => onStepClick(ti, si, ev));
      row.appendChild(btn);
    }
    gridEl.appendChild(row);
  });
  syncGridUi();
}

function syncGridUi() {
  document.querySelectorAll(".step").forEach((el) => {
    const ti = Number(el.dataset.track);
    const si = Number(el.dataset.step);
    if (Number.isNaN(ti)) return;
    const cell = pattern[ti][si];
    el.classList.toggle("on", cell.on);
    el.classList.toggle("playhead", playing && si === currentStep);
    el.classList.toggle("selected", ti === selected.ti && si === selected.si);
    let tag = el.querySelector(".slot-tag");
    if (cell.on) {
      if (!tag) {
        tag = document.createElement("span");
        tag.className = "slot-tag";
        el.appendChild(tag);
      }
      tag.textContent = `${noteToSlot(cell.note)}·${cell.velocity}`;
    } else if (tag) tag.remove();
  });
  const c = pattern[selected.ti][selected.si];
  editNoteEl.value = c.note;
  editVelEl.value = c.velocity;
}

function onStepClick(ti, si, ev) {
  selected = { ti, si };
  const cell = pattern[ti][si];
  if (ev.altKey) {
    const v = prompt("Velocity 1–127:", String(cell.velocity));
    if (v) {
      cell.velocity = Math.min(127, Math.max(1, parseInt(v, 10) || 100));
      cell.on = true;
    }
  } else if (ev.shiftKey) {
    const slot = prompt("Sample slot 1–16:", String(noteToSlot(cell.note)));
    if (slot) {
      cell.note = slotToNote(Math.min(16, Math.max(1, parseInt(slot, 10) || 1)));
      cell.on = true;
    }
  } else {
    cell.on = !cell.on;
    if (cell.on) cell.note = cell.note || DEFAULT_NOTES[ti];
    if (cell.on) previewNote(cell.note, cell.velocity);
  }
  syncGridUi();
}

function setStatus(msg) {
  statusEl.textContent = msg;
}

async function refreshMidi() {
  if (!navigator.requestMIDIAccess) {
    setStatus("Web MIDI optional — Web Audio preview always works");
    return;
  }
  const access = await navigator.requestMIDIAccess({ sysex: false });
  midiSelect.innerHTML = '<option value="">— select port —</option>';
  for (const out of access.outputs.values()) {
    const opt = document.createElement("option");
    opt.value = out.id;
    opt.textContent = out.name || out.id;
    midiSelect.appendChild(opt);
  }
  midiSelect.onchange = () => {
    midiOut = midiSelect.value ? access.outputs.get(midiSelect.value) : null;
    setStatus(midiOut ? `MIDI: ${midiOut.name}` : "MIDI off — preview only");
    sendFxCc();
  };
}

function sendNote(channel, note, velocity, durationMs) {
  if (midiOut) {
    midiOut.send([0x90 + channel, note, velocity]);
    setTimeout(() => midiOut.send([0x80 + channel, note, 0]), durationMs);
  }
  previewNote(note, velocity, durationMs / 1000);
}

function stepDurationMs() {
  return (60000 / (Number(bpmEl.value) || 84)) / 4;
}

function playStep(si) {
  pattern.forEach((track, ti) => {
    const cell = track[si];
    if (!cell.on) return;
    sendNote(Math.min(ti, 15), cell.note, cell.velocity, Math.min(120, stepDurationMs() * 0.85));
  });
}

async function startTransport() {
  await resumeAudio();
  if (playing) return;
  if (document.getElementById("backingOn").checked && hasBacking()) {
    restartBacking(true);
  }
  playing = true;
  currentStep = -1;
  sendFxCc();
  const tick = () => {
    currentStep = (currentStep + 1) % STEPS;
    playStep(currentStep);
    syncGridUi();
  };
  tick();
  timerId = setInterval(tick, stepDurationMs());
  setStatus(document.getElementById("backingOn").checked ? "Playing pattern + backing" : "Playing…");
}

function stopTransport() {
  playing = false;
  if (timerId) clearInterval(timerId);
  timerId = null;
  if (clockTimer) clearInterval(clockTimer);
  currentStep = -1;
  syncGridUi();
  setStatus("Stopped");
}

function patternToJson() {
  return {
    format: "LOFI12_STEP_PATTERN",
    version: 2,
    bpm: Number(bpmEl.value) || 84,
    steps: STEPS,
    fx: fxValues(),
    tracks: TRACK_NAMES.map((name, ti) => ({
      name,
      midiChannel: ti + 1,
      defaultNote: DEFAULT_NOTES[ti],
      steps: pattern[ti].map((c) => ({ ...c })),
    })),
  };
}

function loadFromJson(obj) {
  if (!obj.tracks) return;
  bpmEl.value = obj.bpm || 84;
  if (obj.fx) {
    document.getElementById("fxFilter").value = obj.fx.filter ?? 0.65;
    document.getElementById("fxReverb").value = obj.fx.reverb ?? 0.25;
    document.getElementById("fxTape").value = obj.fx.tape ?? 0.2;
    document.getElementById("fxDrive").value = obj.fx.drive ?? 0.15;
  }
  obj.tracks.forEach((tr, ti) => {
    if (ti >= TRACK_NAMES.length) return;
    if (tr.defaultNote) DEFAULT_NOTES[ti] = tr.defaultNote;
    tr.steps.forEach((s, si) => {
      if (si >= STEPS) return;
      pattern[ti][si] = {
        on: !!s.on,
        note: s.note ?? DEFAULT_NOTES[ti],
        velocity: s.velocity ?? 100,
      };
    });
  });
  syncGridUi();
}

async function loadGrooveFromApi() {
  const prompt = document.getElementById("groovePrompt").value || "dj paul memphis 84";
  const body = { prompt, bpm: Number(bpmEl.value) || 84 };
  const preset = selectedPresetId();
  if (preset) body.preset = preset;
  const res = await fetch("/api/groove_pattern", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  const data = await res.json();
  loadFromJson(data);
  setStatus("Loaded groove from factory (6 tracks)");
}

function downloadBackingBuffer(buf) {
  const blob = new Blob([buf], { type: "audio/wav" });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = "backing_loop.wav";
  a.click();
}

async function generateBacking() {
  const prompt = document.getElementById("groovePrompt").value || "juicy j memphis phonk 84";
  setStatus("Rendering backing loop…");
  const body = {
    prompt,
    bpm: Number(bpmEl.value) || 84,
    bars: 2,
    fx: fxValues(),
  };
  const preset = selectedPresetId();
  if (preset) body.preset = preset;
  const res = await fetch("/api/render_loop", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  const metaRaw = res.headers.get("X-Loop-Meta");
  const buf = await res.arrayBuffer();
  lastBackingBuffer = buf;
  if (metaRaw) {
    try {
      const meta = JSON.parse(metaRaw);
      if (meta.bpm) bpmEl.value = meta.bpm;
      if (meta.memphisLaneName) {
        setStatus(`Backing: ${meta.memphisLaneName} · ${meta.drumEngine} · ${meta.bpm} BPM`);
      }
    } catch (_) {}
  }
  if (document.getElementById("backingOn").checked) {
    await playBackingArrayBuffer(buf, true);
  }
  if (document.getElementById("autoDownloadWav").checked) {
    downloadBackingBuffer(buf);
  }
  if (!metaRaw) setStatus("Backing loop ready — hit Play to practice");
}

function applyPresetFromUi() {
  const id = selectedPresetId();
  const p = presetCache.find((x) => x.id === id);
  if (!p) {
    setStatus("Select a style preset first");
    return;
  }
  document.getElementById("groovePrompt").value = p.prompt;
  if (p.bpm) bpmEl.value = p.bpm;
  if (p.fx) {
    document.getElementById("fxFilter").value = p.fx.filter;
    document.getElementById("fxReverb").value = p.fx.reverb;
    document.getElementById("fxTape").value = p.fx.tape;
    document.getElementById("fxDrive").value = p.fx.drive;
    sendFxCc();
  }
  setStatus(`Preset: ${p.label}`);
}

async function loadPresets() {
  const sel = document.getElementById("stylePreset");
  try {
    const res = await fetch("/api/presets");
    presetCache = await res.json();
    sel.innerHTML = '<option value="">— custom prompt —</option>';
    presetCache.forEach((p) => {
      const opt = document.createElement("option");
      opt.value = p.id;
      opt.textContent = p.label;
      sel.appendChild(opt);
    });
    sel.value = "memphis_trinity";
    applyPresetFromUi();
  } catch (_) {
    sel.innerHTML = '<option value="">— presets unavailable —</option>';
  }
}

function padHit(ti) {
  resumeAudio();
  const note = DEFAULT_NOTES[ti];
  sendNote(ti, note, 110, 160);
}

function buildPads() {
  padRowEl.innerHTML = "";
  TRACK_NAMES.forEach((name, ti) => {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className = "pad-btn";
    btn.textContent = name;
    btn.title = `Pad · key ${ti + 1}`;
    btn.addEventListener("mousedown", () => padHit(ti));
    padRowEl.appendChild(btn);
  });
}

document.addEventListener("keydown", (ev) => {
  if (ev.target instanceof HTMLInputElement || ev.target instanceof HTMLTextAreaElement) return;
  const idx = "123456".indexOf(ev.key);
  if (idx >= 0) {
    padHit(idx);
    ev.preventDefault();
  }
});

document.getElementById("play").onclick = () => startTransport();
document.getElementById("stop").onclick = () => {
  stopTransport();
  stopBacking();
};
document.getElementById("refreshMidi").onclick = () => refreshMidi();
document.getElementById("clear").onclick = () => {
  pattern.forEach((t, ti) =>
    t.forEach((c) => {
      c.on = false;
      c.note = DEFAULT_NOTES[ti];
      c.velocity = 100;
    })
  );
  syncGridUi();
};
document.getElementById("loadGroove").onclick = () => loadGrooveFromApi();
document.getElementById("genBacking").onclick = () => generateBacking();
document.getElementById("applyPreset").onclick = () => applyPresetFromUi();
document.getElementById("downloadBacking").onclick = () => {
  if (lastBackingBuffer) downloadBackingBuffer(lastBackingBuffer);
  else setStatus("Generate a backing loop first");
};
document.getElementById("applyEdit").onclick = () => {
  const c = pattern[selected.ti][selected.si];
  c.note = Math.min(127, Math.max(0, parseInt(editNoteEl.value, 10) || c.note));
  c.velocity = Math.min(127, Math.max(1, parseInt(editVelEl.value, 10) || c.velocity));
  c.on = true;
  syncGridUi();
};
["fxFilter", "fxReverb", "fxTape", "fxDrive"].forEach((id) => {
  document.getElementById(id).addEventListener("input", () => sendFxCc());
});
document.getElementById("saveJson").onclick = () => {
  const blob = new Blob([JSON.stringify(patternToJson(), null, 2)], { type: "application/json" });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = "lofi12_pattern.json";
  a.click();
};
document.getElementById("loadJson").onchange = async (ev) => {
  const file = ev.target.files?.[0];
  if (!file) return;
  loadFromJson(JSON.parse(await file.text()));
  setStatus(`Loaded ${file.name}`);
};
document.getElementById("loadBackingFile").onchange = async (ev) => {
  const file = ev.target.files?.[0];
  if (!file) return;
  await playBackingArrayBuffer(await file.arrayBuffer(), true);
  setStatus(`Backing: ${file.name}`);
};

buildGrid();
buildPads();
loadPresets();
refreshMidi();
