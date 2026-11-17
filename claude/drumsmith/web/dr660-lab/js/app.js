import { DR660Sequencer } from './Sequencer.js';
import { DrumEngine } from './DrumEngine.js';
import { MasterChain } from './MasterChain.js';

const TRACKS = ['kick', 'snare', 'rim', 'chh', 'ohh', 'cowbell', 'perc1', 'perc2'];

/** FX rack presets — maps to MasterChain UI params */
const FX_PRESETS = {
  stock12Bit: {
    cassette: 0.25,
    cutoff: 12000,
    reso: 0.7,
    bits: 12,
    downsample: 2,
    crushMix: 0.55,
    delayWet: 0.15,
    delayMs: 280,
    delayFb: 0.25,
  },
  classicSampler: {
    cassette: 0.35,
    cutoff: 7800,
    reso: 1.8,
    bits: 12,
    downsample: 6,
    crushMix: 0.9,
    delayWet: 0.22,
    delayMs: 320,
    delayFb: 0.35,
  },
  warmCassette: {
    cassette: 0.72,
    cutoff: 6200,
    reso: 1.0,
    bits: 14,
    downsample: 3,
    crushMix: 0.45,
    delayWet: 0.4,
    delayMs: 420,
    delayFb: 0.55,
  },
  undergroundMemphis: {
    cassette: 0.55,
    cutoff: 9500,
    reso: 1.2,
    bits: 12,
    downsample: 4,
    crushMix: 0.85,
    delayWet: 0.35,
    delayMs: 375,
    delayFb: 0.45,
  },
};

/** Dummy context so we can edit patterns before a user gesture. */
function makePlaceholderCtx() {
  return {
    currentTime: 0,
    state: 'running',
    resume: async () => {},
  };
}

/** @type {AudioContext | null} */
let audioCtx = null;
/** @type {MasterChain | null} */
let master = null;
/** @type {DrumEngine | null} */
let drums = null;
/** @type {DR660Sequencer} */
let seq = new DR660Sequencer(/** @type {any} */ (makePlaceholderCtx()), () => {});
seq.loadPreset('crawl');

async function ensureAudio() {
  if (audioCtx) {
    if (audioCtx.state === 'suspended') await audioCtx.resume();
    return;
  }
  audioCtx = new (window.AudioContext || window.webkitAudioContext)();
  master = new MasterChain(audioCtx);
  drums = new DrumEngine(audioCtx, master.input);
  try {
    await master.initBitcrush();
  } catch (err) {
    console.warn('Bitcrush worklet unavailable; cassette+LPF+delay only', err);
  }

  const saved = {
    bpm: seq.bpm,
    swing: seq.swing,
    pattern: structuredClone(seq.pattern),
  };

  seq = new DR660Sequencer(
    audioCtx,
    (track, time, velocity) => drums.trigger(track, time, velocity),
    (step) => paintPlayhead(step),
    () => drums.stopAll(),
  );
  seq.bpm = saved.bpm;
  seq.swing = saved.swing;
  seq.pattern = saved.pattern;

  applyFxFromUi();
  master.syncDelayToBpm(seq.bpm);
}

function setRange(id, value) {
  /** @type {HTMLInputElement} */ (document.getElementById(id)).value = String(value);
}

function applyFxFromUi() {
  if (!master) return;
  master.setDrive(Number(/** @type {HTMLInputElement} */ (document.getElementById('cassette')).value));
  master.setFilterCutoff(Number(/** @type {HTMLInputElement} */ (document.getElementById('cutoff')).value));
  master.setFilterQ(Number(/** @type {HTMLInputElement} */ (document.getElementById('reso')).value));
  const bits = Number(/** @type {HTMLInputElement} */ (document.getElementById('bits')).value);
  const down = Number(/** @type {HTMLInputElement} */ (document.getElementById('downsample')).value);
  const mix = Number(/** @type {HTMLInputElement} */ (document.getElementById('crush-mix')).value);
  master.setBitcrush(bits, down, mix);
  document.getElementById('bits-val').textContent = String(bits);
  document.getElementById('down-val').textContent = `${down}×`;
  document.getElementById('cutoff-val').textContent = `${/** @type {HTMLInputElement} */ (document.getElementById('cutoff')).value} Hz`;

  const delayMs = Number(/** @type {HTMLInputElement} */ (document.getElementById('delay-ms')).value);
  const feedback = Number(/** @type {HTMLInputElement} */ (document.getElementById('delay-fb')).value);
  const wet = Number(/** @type {HTMLInputElement} */ (document.getElementById('delay-wet')).value);
  master.setDelayTime(delayMs / 1000);
  master.setDelayFeedback(feedback);
  master.setDelayWet(wet);
  document.getElementById('delay-ms-val').textContent = `${delayMs} ms`;
  document.getElementById('delay-fb-val').textContent = feedback.toFixed(2);
  document.getElementById('delay-wet-val').textContent = wet.toFixed(2);
}

/** @param {keyof typeof FX_PRESETS} name */
function loadFxPreset(name) {
  const p = FX_PRESETS[name];
  if (!p) return;
  setRange('cassette', p.cassette);
  setRange('cutoff', p.cutoff);
  setRange('reso', p.reso);
  setRange('bits', p.bits);
  setRange('downsample', p.downsample);
  setRange('crush-mix', p.crushMix);
  setRange('delay-wet', p.delayWet);
  setRange('delay-ms', p.delayMs);
  setRange('delay-fb', p.delayFb);
  document.querySelectorAll('[data-fx-preset]').forEach((btn) => {
    btn.classList.toggle('active', btn.getAttribute('data-fx-preset') === name);
  });
  applyFxFromUi();
}

function paintPlayhead(step) {
  document.querySelectorAll('.step-btn').forEach((el) => {
    el.classList.toggle('is-playing', Number(el.dataset.step) === step);
  });
}

function syncControlsFromSeq() {
  /** @type {HTMLInputElement} */ (document.getElementById('bpm')).value = String(seq.bpm);
  /** @type {HTMLInputElement} */ (document.getElementById('swing')).value = String(seq.swing);
  document.getElementById('bpm-val').textContent = String(seq.bpm);
  document.getElementById('swing-val').textContent = `${seq.swing}%`;
}

function renderGrid() {
  const grid = document.getElementById('grid');
  grid.innerHTML = '';
  const head = document.createElement('div');
  head.className = 'grid-head';
  head.innerHTML = `<span></span>${Array.from({ length: 16 }, (_, i) => `<span>${i + 1}</span>`).join('')}`;
  grid.appendChild(head);

  for (const track of TRACKS) {
    const row = document.createElement('div');
    row.className = 'grid-row';
    row.dataset.track = track;
    const label = document.createElement('span');
    label.className = 'track-label';
    label.textContent = track;
    row.appendChild(label);
    for (let s = 0; s < 16; s += 1) {
      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'step-btn';
      btn.dataset.track = track;
      btn.dataset.step = String(s);
      const vel = seq.pattern[track][s];
      btn.dataset.velocity = String(vel);
      btn.dataset.vel = vel === 0 ? 'off' : vel < 1 ? 'ghost' : 'accent';
      if (vel > 0) btn.classList.add('is-active');
      btn.addEventListener('click', async () => {
        await ensureAudio();
        const next = seq.toggleStep(track, s);
        btn.dataset.velocity = String(next);
        btn.dataset.vel = next === 0 ? 'off' : next < 1 ? 'ghost' : 'accent';
        btn.classList.toggle('is-active', next > 0);
      });
      row.appendChild(btn);
    }
    grid.appendChild(row);
  }
}

function bind() {
  document.getElementById('btn-start').addEventListener('click', async () => {
    await ensureAudio();
    await seq.start();
  });
  document.getElementById('btn-stop').addEventListener('click', () => seq.stop());

  document.getElementById('bpm').addEventListener('input', (e) => {
    seq.setBpm(/** @type {HTMLInputElement} */ (e.target).value);
    document.getElementById('bpm-val').textContent = String(seq.bpm);
    if (master && /** @type {HTMLInputElement} */ (document.getElementById('delay-sync')).checked) {
      master.syncDelayToBpm(seq.bpm);
      const ms = Math.round(master.tapeDelay.delayNode.delayTime.value * 1000);
      setRange('delay-ms', ms);
      document.getElementById('delay-ms-val').textContent = `${ms} ms`;
    }
  });
  document.getElementById('swing').addEventListener('input', (e) => {
    seq.setSwing(/** @type {HTMLInputElement} */ (e.target).value);
    document.getElementById('swing-val').textContent = `${seq.swing}%`;
  });
  document.getElementById('preset').addEventListener('change', (e) => {
    seq.loadPreset(/** @type {HTMLSelectElement} */ (e.target).value);
    syncControlsFromSeq();
    renderGrid();
  });

  for (const id of ['cassette', 'cutoff', 'reso', 'bits', 'downsample', 'crush-mix', 'delay-ms', 'delay-fb', 'delay-wet']) {
    document.getElementById(id).addEventListener('input', async () => {
      await ensureAudio();
      applyFxFromUi();
    });
  }

  document.getElementById('delay-sync').addEventListener('change', async (e) => {
    await ensureAudio();
    if (/** @type {HTMLInputElement} */ (e.target).checked && master) {
      master.syncDelayToBpm(seq.bpm);
      const ms = Math.round(master.tapeDelay.delayNode.delayTime.value * 1000);
      setRange('delay-ms', ms);
      document.getElementById('delay-ms-val').textContent = `${ms} ms`;
    }
  });

  document.querySelectorAll('[data-fx-preset]').forEach((btn) => {
    btn.addEventListener('click', async () => {
      await ensureAudio();
      loadFxPreset(/** @type {keyof typeof FX_PRESETS} */ (btn.getAttribute('data-fx-preset')));
    });
  });

  syncControlsFromSeq();
  renderGrid();
}

bind();
