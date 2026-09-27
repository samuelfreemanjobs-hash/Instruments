/**
 * Lofi-12 4×16 step sequencer — Web MIDI output.
 */

const STEPS = 16;
const TRACK_NAMES = ["Kick", "Snare", "Hat", "Cowbell"];
const DEFAULT_NOTES = [36, 39, 42, 45]; // slots 1,4,7,10
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

const gridEl = document.getElementById("grid");
const statusEl = document.getElementById("status");
const bpmEl = document.getElementById("bpm");
const midiSelect = document.getElementById("midiOut");

function slotToNote(slot) {
  return SLOT_BASE + slot - 1;
}

function noteToSlot(note) {
  return note - SLOT_BASE + 1;
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
    row.appendChild(label);

    for (let si = 0; si < STEPS; si++) {
      const btn = document.createElement("button");
      btn.type = "button";
      btn.className = "step";
      btn.dataset.track = String(ti);
      btn.dataset.step = String(si);
      btn.title = "Click toggle · Shift+click sound lock (slot)";
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
    let tag = el.querySelector(".slot-tag");
    if (ti === 0 && cell.on && cell.note !== DEFAULT_NOTES[0]) {
      if (!tag) {
        tag = document.createElement("span");
        tag.className = "slot-tag";
        el.appendChild(tag);
      }
      tag.textContent = String(noteToSlot(cell.note));
    } else if (tag) tag.remove();
  });
}

function onStepClick(ti, si, ev) {
  const cell = pattern[ti][si];
  if (ev.shiftKey && ti === 0) {
    const slot = prompt("Sample slot 1–16 on Lofi-12 bank:", String(noteToSlot(cell.note)));
    if (slot) {
      const n = Math.min(16, Math.max(1, parseInt(slot, 10) || 1));
      cell.note = slotToNote(n);
      cell.on = true;
    }
  } else {
    cell.on = !cell.on;
    if (cell.on && cell.note < SLOT_BASE) cell.note = DEFAULT_NOTES[ti];
  }
  syncGridUi();
}

function setStatus(msg) {
  statusEl.textContent = msg;
}

async function refreshMidi() {
  if (!navigator.requestMIDIAccess) {
    setStatus("Web MIDI not supported — use Chrome/Edge or midi_play.py");
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
    const id = midiSelect.value;
    midiOut = id ? access.outputs.get(id) : null;
    setStatus(midiOut ? `MIDI: ${midiOut.name}` : "No MIDI port selected");
  };
  setStatus(`Found ${access.outputs.size} MIDI output(s)`);
}

function sendNote(channel, note, velocity, durationMs) {
  if (!midiOut) return;
  midiOut.send([0x90 + channel, note, velocity]);
  setTimeout(() => midiOut.send([0x80 + channel, note, 0]), durationMs);
}

function sendClockStart() {
  if (!midiOut) return;
  midiOut.send([0xfa]);
}

function sendClockStop() {
  if (!midiOut) return;
  midiOut.send([0xfc]);
}

function sendClockTick() {
  if (!midiOut) return;
  midiOut.send([0xf8]);
}

function stepDurationMs() {
  const bpm = Number(bpmEl.value) || 84;
  return (60000 / bpm / 4);
}

function playStep(si) {
  pattern.forEach((track, ti) => {
    const cell = track[si];
    if (!cell.on) return;
    const ch = ti; // channels 0–3; map to Lofi track auto-channel if configured
    sendNote(ch, cell.note, cell.velocity, Math.min(120, stepDurationMs() * 0.8));
  });
}

function startTransport() {
  if (playing) return;
  playing = true;
  currentStep = -1;
  const sendClock = document.getElementById("sendClock").checked;
  if (sendClock && midiOut) {
    sendClockStart();
    const bpm = Number(bpmEl.value) || 84;
    const tickMs = 60000 / bpm / 24;
    clockTimer = setInterval(sendClockTick, tickMs);
  }
  const tick = () => {
    currentStep = (currentStep + 1) % STEPS;
    playStep(currentStep);
    syncGridUi();
  };
  tick();
  timerId = setInterval(tick, stepDurationMs());
  setStatus("Playing…");
}

function stopTransport() {
  playing = false;
  if (timerId) clearInterval(timerId);
  timerId = null;
  if (clockTimer) clearInterval(clockTimer);
  clockTimer = null;
  sendClockStop();
  currentStep = -1;
  syncGridUi();
  setStatus("Stopped");
}

function patternToJson() {
  return {
    format: "LOFI12_STEP_PATTERN",
    version: 1,
    bpm: Number(bpmEl.value) || 84,
    steps: STEPS,
    sendClock: document.getElementById("sendClock").checked,
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
  document.getElementById("sendClock").checked = !!obj.sendClock;
  obj.tracks.forEach((tr, ti) => {
    if (ti >= TRACK_NAMES.length) return;
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

/** Demo phonk groove (matches factory kick/snare/hat/cowbell steps). */
function loadPhonkDemo() {
  bpmEl.value = 84;
  pattern = TRACK_NAMES.map((_, ti) =>
    Array.from({ length: STEPS }, () => ({
      on: false,
      note: DEFAULT_NOTES[ti],
      velocity: 100,
    }))
  );
  const on = (ti, steps, note, vel = 100) => {
    steps.forEach((s) => {
      pattern[ti][s].on = true;
      if (note) pattern[ti][s].note = note;
      pattern[ti][s].velocity = vel;
    });
  };
  on(0, [0, 6, 8], slotToNote(1));
  on(0, [6], slotToNote(2), 70);
  on(1, [4, 12], slotToNote(4), 110);
  on(2, [0, 2, 4, 6, 8, 10, 12, 14], slotToNote(7), 75);
  on(3, [2, 9], slotToNote(10), 90);
  syncGridUi();
  setStatus("Loaded DJ Paul–style demo pattern");
}

document.getElementById("play").onclick = startTransport;
document.getElementById("stop").onclick = stopTransport;
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
  setStatus("Cleared");
};
document.getElementById("loadPhonk").onclick = loadPhonkDemo;
document.getElementById("saveJson").onclick = () => {
  const blob = new Blob([JSON.stringify(patternToJson(), null, 2)], {
    type: "application/json",
  });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = "lofi12_pattern.json";
  a.click();
};
document.getElementById("loadJson").onchange = async (ev) => {
  const file = ev.target.files?.[0];
  if (!file) return;
  const text = await file.text();
  loadFromJson(JSON.parse(text));
  setStatus(`Loaded ${file.name}`);
};

buildGrid();
refreshMidi();
