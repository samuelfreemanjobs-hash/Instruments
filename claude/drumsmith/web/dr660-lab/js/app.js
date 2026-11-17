import { DR660Sequencer } from './Sequencer.js';
import { DrumEngine } from './DrumEngine.js';
import { MasterChain } from './MasterChain.js';

const TRACKS = ['kick', 'snare', 'rim', 'chh', 'ohh', 'cowbell', 'perc1', 'perc2'];

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

  const saved = {
    bpm: seq.bpm,
    swing: seq.swing,
    pattern: structuredClone(seq.pattern),
  };

  seq = new DR660Sequencer(
    audioCtx,
    (track, time, velocity) => drums.trigger(track, time, velocity),
    (step) => paintPlayhead(step),
  );
  seq.bpm = saved.bpm;
  seq.swing = saved.swing;
  seq.pattern = saved.pattern;

  master.setCassetteDrive(Number(/** @type {HTMLInputElement} */ (document.getElementById('cassette')).value));
  master.setFilterCutoff(Number(/** @type {HTMLInputElement} */ (document.getElementById('cutoff')).value));
  master.setFilterQ(Number(/** @type {HTMLInputElement} */ (document.getElementById('reso')).value));
}

function paintPlayhead(step) {
  document.querySelectorAll('.cell').forEach((el) => {
    el.classList.toggle('now', Number(el.dataset.step) === step);
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
    const label = document.createElement('span');
    label.className = 'trk';
    label.textContent = track;
    row.appendChild(label);
    for (let s = 0; s < 16; s += 1) {
      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'cell';
      btn.dataset.track = track;
      btn.dataset.step = String(s);
      const vel = seq.pattern[track][s];
      btn.dataset.vel = vel === 0 ? 'off' : vel < 1 ? 'ghost' : 'accent';
      btn.addEventListener('click', async () => {
        await ensureAudio();
        const next = seq.toggleStep(track, s);
        btn.dataset.vel = next === 0 ? 'off' : next < 1 ? 'ghost' : 'accent';
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
  document.getElementById('cassette').addEventListener('input', async (e) => {
    await ensureAudio();
    master.setCassetteDrive(Number(/** @type {HTMLInputElement} */ (e.target).value));
  });
  document.getElementById('cutoff').addEventListener('input', async (e) => {
    await ensureAudio();
    const hz = Number(/** @type {HTMLInputElement} */ (e.target).value);
    master.setFilterCutoff(hz);
    document.getElementById('cutoff-val').textContent = `${hz} Hz`;
  });
  document.getElementById('reso').addEventListener('input', async (e) => {
    await ensureAudio();
    master.setFilterQ(Number(/** @type {HTMLInputElement} */ (e.target).value));
  });

  document.querySelectorAll('[data-section-btn]').forEach((btn) => {
    btn.addEventListener('click', () => {
      const id = btn.getAttribute('data-section-btn');
      document.querySelectorAll('[data-section-btn]').forEach((b) => b.classList.toggle('on', b === btn));
      document.querySelectorAll('[data-section]').forEach((panel) => {
        panel.classList.toggle('on', panel.getAttribute('data-section') === id);
      });
    });
  });

  syncControlsFromSeq();
  renderGrid();
}

bind();
