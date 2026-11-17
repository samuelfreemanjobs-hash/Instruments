/**
 * Lightweight Web Audio one-shots for lab preview (not native drumsmith-render).
 */

export class DrumEngine {
  /** @param {AudioContext} ctx @param {AudioNode} destination */
  constructor(ctx, destination) {
    this.ctx = ctx;
    this.destination = destination;
  }

  /**
   * @param {string} track
   * @param {number} time
   * @param {number} velocity 0..1
   */
  trigger(track, time, velocity) {
    const v = Math.max(0.05, Math.min(1, velocity));
    switch (track) {
      case 'kick':
        this.#kick(time, v);
        break;
      case 'snare':
        this.#snare(time, v);
        break;
      case 'rim':
        this.#noiseBurst(time, v * 0.35, 0.04, 1800);
        break;
      case 'chh':
        this.#noiseBurst(time, v * 0.22, 0.035, 9000);
        break;
      case 'ohh':
        this.#noiseBurst(time, v * 0.28, 0.22, 7000);
        break;
      case 'cowbell':
        this.#cowbell(time, v);
        break;
      case 'perc1':
        this.#tone(time, v * 0.4, 440, 0.08);
        break;
      case 'perc2':
        this.#tone(time, v * 0.4, 660, 0.06);
        break;
      default:
        break;
    }
  }

  /** @param {number} time @param {number} v */
  #kick(time, v) {
    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(140, time);
    osc.frequency.exponentialRampToValueAtTime(42, time + 0.12);
    gain.gain.setValueAtTime(0.0001, time);
    gain.gain.exponentialRampToValueAtTime(0.9 * v, time + 0.005);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.45);
    osc.connect(gain);
    gain.connect(this.destination);
    osc.start(time);
    osc.stop(time + 0.5);
  }

  /** @param {number} time @param {number} v */
  #snare(time, v) {
    this.#tone(time, v * 0.35, 180, 0.08);
    this.#noiseBurst(time, v * 0.55, 0.12, 3500);
  }

  /** @param {number} time @param {number} v */
  #cowbell(time, v) {
    const g = this.ctx.createGain();
    g.gain.setValueAtTime(0.0001, time);
    g.gain.exponentialRampToValueAtTime(0.35 * v, time + 0.002);
    g.gain.exponentialRampToValueAtTime(0.0001, time + 0.55);
    g.connect(this.destination);
    for (const f of [560, 845]) {
      const o = this.ctx.createOscillator();
      o.type = 'square';
      o.frequency.value = f;
      o.connect(g);
      o.start(time);
      o.stop(time + 0.55);
    }
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
    gain.gain.exponentialRampToValueAtTime(v, time + 0.003);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + dur);
    osc.connect(gain);
    gain.connect(this.destination);
    osc.start(time);
    osc.stop(time + dur + 0.02);
  }

  /**
   * @param {number} time
   * @param {number} v
   * @param {number} dur
   * @param {number} hpHz
   */
  #noiseBurst(time, v, dur, hpHz) {
    const frames = Math.max(1, Math.floor(this.ctx.sampleRate * dur));
    const buffer = this.ctx.createBuffer(1, frames, this.ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < frames; i += 1) data[i] = Math.random() * 2 - 1;
    const src = this.ctx.createBufferSource();
    src.buffer = buffer;
    const hp = this.ctx.createBiquadFilter();
    hp.type = 'highpass';
    hp.frequency.value = hpHz;
    const gain = this.ctx.createGain();
    gain.gain.setValueAtTime(0.0001, time);
    gain.gain.exponentialRampToValueAtTime(v, time + 0.002);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + dur);
    src.connect(hp);
    hp.connect(gain);
    gain.connect(this.destination);
    src.start(time);
    src.stop(time + dur + 0.02);
  }
}
