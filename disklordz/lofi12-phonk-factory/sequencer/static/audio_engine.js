/** Browser preview voices (beat over backing without hardware). */

let audioCtx = null;

function ctx() {
  if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
  return audioCtx;
}

export function resumeAudio() {
  return ctx().resume();
}

function env(ac, t0, attack, decay, peak = 1) {
  const g = ac.createGain();
  g.gain.setValueAtTime(0.0001, t0);
  g.gain.linearRampToValueAtTime(peak, t0 + attack);
  g.gain.exponentialRampToValueAtTime(0.0001, t0 + attack + decay);
  return g;
}

export function previewNote(note, velocity, durationSec = 0.12) {
  const ac = ctx();
  const t0 = ac.currentTime;
  const vel = (velocity / 127) * 0.5;
  const slot = note - 35;

  if (slot <= 3) {
    const o = ac.createOscillator();
    o.type = "sine";
    o.frequency.setValueAtTime(120, t0);
    o.frequency.exponentialRampToValueAtTime(42, t0 + 0.08);
    const g = env(ac, t0, 0.001, 0.35, vel);
    o.connect(g).connect(ac.destination);
    o.start(t0);
    o.stop(t0 + 0.4);
    return;
  }
  if (slot <= 6) {
    const n = ac.createBufferSource();
    const len = ac.sampleRate * 0.15;
    const buf = ac.createBuffer(1, len, ac.sampleRate);
    const d = buf.getChannelData(0);
    for (let i = 0; i < len; i++) d[i] = (Math.random() * 2 - 1) * Math.exp(-i / (ac.sampleRate * 0.05));
    n.buffer = buf;
    const g = env(ac, t0, 0.001, 0.12, vel);
    n.connect(g).connect(ac.destination);
    n.start(t0);
    return;
  }
  const o = ac.createOscillator();
  o.type = "triangle";
  o.frequency.value = 680 + slot * 20;
  const g = env(ac, t0, 0.002, 0.09, vel * 0.7);
  o.connect(g).connect(ac.destination);
  o.start(t0);
  o.stop(t0 + durationSec);
}

/** @type {AudioBufferSourceNode | null} */
let backingSource = null;
/** @type {AudioBuffer | null} */
let backingBuffer = null;

export function hasBacking() {
  return backingBuffer !== null;
}

export function stopBacking() {
  if (backingSource) {
    try {
      backingSource.stop();
    } catch (_) {}
    backingSource = null;
  }
}

function startBackingFromBuffer(loop = true) {
  if (!backingBuffer) return;
  stopBacking();
  const ac = ctx();
  const src = ac.createBufferSource();
  src.buffer = backingBuffer;
  src.loop = loop;
  src.connect(ac.destination);
  src.start();
  backingSource = src;
}

/** Decode and optionally start looped playback; keeps buffer for transport sync. */
export async function playBackingArrayBuffer(arrayBuffer, loop = true, autoplay = true) {
  const ac = ctx();
  backingBuffer = await ac.decodeAudioData(arrayBuffer.slice(0));
  if (autoplay) startBackingFromBuffer(loop);
}

/** Restart backing from bar 1 (call when sequencer transport starts). */
export function restartBacking(loop = true) {
  if (!backingBuffer) return;
  startBackingFromBuffer(loop);
}
