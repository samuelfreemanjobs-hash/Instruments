/**
 * Lightweight Web Audio one-shots for lab preview (not native drumsmith-render).
 * Velocities use squared gain (DR-660 accent feel). CHH chokes ringing OHH.
 * Tracks active sources so Stop can cancel queued / ringing voices.
 */
import { Cowbell808Synth } from './CowbellSynth.js';

export class DrumEngine {
  /** @param {AudioContext} ctx @param {AudioNode} destination */
  constructor(ctx, destination) {
    this.ctx = ctx;
    this.destination = destination;
    /** @type {{ stop: (t: number) => void } | null} */
    this._openHat = null;
    /** @type {Set<AudioScheduledSourceNode>} */
    this._active = new Set();
    this.cowbell = new Cowbell808Synth(ctx, destination, (src, end) => this.#track(src, 0, end));
  }

  /**
   * Immediately stop all scheduled/ringing voices (transport Stop).
   * @param {number} [when]
   */
  stopAll(when) {
    const t = when ?? this.ctx.currentTime;
    this.#chokeOpenHat(t);
    for (const src of [...this._active]) {
      try {
        src.stop(t);
      } catch {
        /* already stopped */
      }
    }
    this._active.clear();
  }

  /**
   * @param {AudioScheduledSourceNode} src
   * @param {number} time
   * @param {number} endTime
   */
  #track(src, time, endTime) {
    this._active.add(src);
    src.addEventListener(
      'ended',
      () => {
        this._active.delete(src);
      },
      { once: true },
    );
    // Safety cleanup if ended never fires after stop
    const ttlMs = Math.max(50, (endTime - this.ctx.currentTime + 0.5) * 1000);
    setTimeout(() => this._active.delete(src), ttlMs);
  }

  /**
   * @param {string} track
   * @param {number} time
   * @param {number} velocity 0..1
   * @param {{ pitch?: number }} [opts]
   */
  trigger(track, time, velocity, opts = {}) {
    const v = Math.max(0.05, Math.min(1, velocity));
    const g = v * v; // logarithmic-ish accent curve
    switch (track) {
      case 'kick':
        this.#kick(time, g);
        break;
      case 'snare':
        this.#snare(time, g);
        break;
      case 'rim':
        this.#noiseBurst(time, g * 0.35, 0.04, 1800);
        break;
      case 'chh':
        this.#chokeOpenHat(time);
        this.#noiseBurst(time, g * 0.22, 0.035, 9000, v < 1.0 ? 0.96 : 1);
        break;
      case 'ohh':
        this.#chokeOpenHat(time);
        this._openHat = this.#noiseBurst(time, g * 0.28, 0.22, 7000);
        break;
      case 'cowbell':
        this.cowbell.trigger(time, v, opts.pitch ?? 1);
        break;
      case 'perc1':
        this.#tone(time, g * 0.4, 440, 0.08);
        break;
      case 'perc2':
        this.#tone(time, g * 0.4, 660, 0.06);
        break;
      default:
        break;
    }
  }

  /** @param {number} time */
  #chokeOpenHat(time) {
    if (this._openHat) {
      try {
        this._openHat.stop(time);
      } catch {
        /* already stopped */
      }
      this._active.delete(/** @type {AudioScheduledSourceNode} */ (this._openHat));
      this._openHat = null;
    }
  }

  /**
   * MEMPHIS-660 kick stack: click (1–2.8 kHz) + body knock + sub foundation.
   * @param {number} time
   * @param {number} v
   */
  #kick(time, v) {
    const end = time + 0.28;
    // Sub foundation ~43 Hz with +48 st pitch spike (~22 ms)
    const sub = this.ctx.createOscillator();
    const subGain = this.ctx.createGain();
    sub.type = 'sine';
    sub.frequency.setValueAtTime(43.65 * Math.pow(2, 48 / 12), time);
    sub.frequency.exponentialRampToValueAtTime(43.65, time + 0.022);
    subGain.gain.setValueAtTime(0.0001, time);
    subGain.gain.linearRampToValueAtTime(0.95 * v, time + 0.005);
    subGain.gain.exponentialRampToValueAtTime(0.0001, time + 0.18);
    sub.connect(subGain);
    subGain.connect(this.destination);
    sub.start(time);
    sub.stop(end);
    this.#track(sub, time, end);

    // Body knock 65–120 Hz region
    const body = this.ctx.createOscillator();
    const bodyGain = this.ctx.createGain();
    body.type = 'sine';
    body.frequency.setValueAtTime(110, time);
    body.frequency.exponentialRampToValueAtTime(72, time + 0.05);
    bodyGain.gain.setValueAtTime(0.0001, time);
    bodyGain.gain.linearRampToValueAtTime(0.45 * v, time + 0.003);
    bodyGain.gain.exponentialRampToValueAtTime(0.0001, time + 0.09);
    body.connect(bodyGain);
    bodyGain.connect(this.destination);
    body.start(time);
    body.stop(end);
    this.#track(body, time, end);

    // Woody click 1–2.8 kHz (short noise through bandpass)
    const frames = Math.max(1, Math.floor(this.ctx.sampleRate * 0.012));
    const buffer = this.ctx.createBuffer(1, frames, this.ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < frames; i += 1) data[i] = Math.random() * 2 - 1;
    const click = this.ctx.createBufferSource();
    click.buffer = buffer;
    const bp = this.ctx.createBiquadFilter();
    bp.type = 'bandpass';
    bp.frequency.value = 1800;
    bp.Q.value = 1.4;
    const clickGain = this.ctx.createGain();
    clickGain.gain.setValueAtTime(0.0001, time);
    clickGain.gain.linearRampToValueAtTime(0.28 * v, time + 0.001);
    clickGain.gain.exponentialRampToValueAtTime(0.0001, time + 0.014);
    click.connect(bp);
    bp.connect(clickGain);
    clickGain.connect(this.destination);
    click.start(time);
    click.stop(time + 0.02);
    this.#track(click, time, time + 0.02);
  }

  /**
   * MEMPHIS-DR660 snare triad: membrane pitch drop + delayed BP wires + flam clap.
   * @param {number} time
   * @param {number} v
   */
  #snare(time, v) {
    const end = time + 0.32;
    const root = 220;

    // Membrane body ~220 Hz with +24 st pitch spike (~18 ms)
    const membrane = this.ctx.createOscillator();
    const membraneGain = this.ctx.createGain();
    membrane.type = 'sine';
    membrane.frequency.setValueAtTime(root * Math.pow(2, 24 / 12), time);
    membrane.frequency.exponentialRampToValueAtTime(root, time + 0.018);
    membraneGain.gain.setValueAtTime(0.0001, time);
    membraneGain.gain.linearRampToValueAtTime(0.72 * v, time + 0.003);
    membraneGain.gain.exponentialRampToValueAtTime(0.0001, time + 0.09);
    membrane.connect(membraneGain);
    membraneGain.connect(this.destination);
    membrane.start(time);
    membrane.stop(end);
    this.#track(membrane, time, end);

    // Noise wires BP ~2.4 kHz, onset delayed ~2.5 ms
    const noiseDur = 0.2;
    const noiseFrames = Math.max(1, Math.floor(this.ctx.sampleRate * noiseDur));
    const noiseBuf = this.ctx.createBuffer(1, noiseFrames, this.ctx.sampleRate);
    const noiseData = noiseBuf.getChannelData(0);
    for (let i = 0; i < noiseFrames; i += 1) noiseData[i] = Math.random() * 2 - 1;
    const wires = this.ctx.createBufferSource();
    wires.buffer = noiseBuf;
    const bp = this.ctx.createBiquadFilter();
    bp.type = 'bandpass';
    bp.frequency.value = 2400;
    bp.Q.value = 1.2;
    const wireGain = this.ctx.createGain();
    const wireStart = time + 0.0025;
    wireGain.gain.setValueAtTime(0.0001, wireStart);
    wireGain.gain.linearRampToValueAtTime(0.5 * v, wireStart + 0.002);
    wireGain.gain.exponentialRampToValueAtTime(0.0001, wireStart + 0.19);
    wires.connect(bp);
    bp.connect(wireGain);
    wireGain.connect(this.destination);
    wires.start(wireStart);
    wires.stop(wireStart + noiseDur);
    this.#track(wires, wireStart, wireStart + noiseDur);

    // Flam / clap layer ~4 ms late, HPF ~500 Hz
    const flamDelay = 0.004;
    const clapDur = 0.08;
    const clapFrames = Math.max(1, Math.floor(this.ctx.sampleRate * clapDur));
    const clapBuf = this.ctx.createBuffer(1, clapFrames, this.ctx.sampleRate);
    const clapData = clapBuf.getChannelData(0);
    for (let i = 0; i < clapFrames; i += 1) clapData[i] = Math.random() * 2 - 1;
    const clap = this.ctx.createBufferSource();
    clap.buffer = clapBuf;
    const hp = this.ctx.createBiquadFilter();
    hp.type = 'highpass';
    hp.frequency.value = 500;
    const clapGain = this.ctx.createGain();
    const clapStart = time + flamDelay;
    clapGain.gain.setValueAtTime(0.0001, clapStart);
    clapGain.gain.linearRampToValueAtTime(0.42 * v, clapStart + 0.001);
    clapGain.gain.exponentialRampToValueAtTime(0.0001, clapStart + 0.045);
    clap.connect(hp);
    hp.connect(clapGain);
    clapGain.connect(this.destination);
    clap.start(clapStart);
    clap.stop(clapStart + clapDur);
    this.#track(clap, clapStart, clapStart + clapDur);
  }

  /**
   * @param {number} time
   * @param {number} v
   * @param {number} freq
   * @param {number} dur
   */
  #tone(time, v, freq, dur) {
    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    osc.type = 'triangle';
    osc.frequency.value = freq;
    gain.gain.setValueAtTime(0.0001, time);
    gain.gain.exponentialRampToValueAtTime(Math.max(0.0001, v), time + 0.003);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + dur);
    osc.connect(gain);
    gain.connect(this.destination);
    osc.start(time);
    osc.stop(time + dur + 0.02);
    this.#track(osc, time, time + dur + 0.02);
  }

  /**
   * @param {number} time
   * @param {number} v
   * @param {number} dur
   * @param {number} hpHz
   * @param {number} [playbackRate=1]
   * @returns {AudioBufferSourceNode}
   */
  #noiseBurst(time, v, dur, hpHz, playbackRate = 1) {
    const frames = Math.max(1, Math.floor(this.ctx.sampleRate * dur));
    const buffer = this.ctx.createBuffer(1, frames, this.ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < frames; i += 1) data[i] = Math.random() * 2 - 1;
    const src = this.ctx.createBufferSource();
    src.buffer = buffer;
    src.playbackRate.setValueAtTime(playbackRate, time);
    const hp = this.ctx.createBiquadFilter();
    hp.type = 'highpass';
    hp.frequency.value = hpHz;
    const gain = this.ctx.createGain();
    gain.gain.setValueAtTime(0.0001, time);
    gain.gain.exponentialRampToValueAtTime(Math.max(0.0001, v), time + 0.002);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + dur);
    src.connect(hp);
    hp.connect(gain);
    gain.connect(this.destination);
    src.start(time);
    src.stop(time + dur + 0.02);
    this.#track(src, time, time + dur + 0.02);
    return src;
  }
}
