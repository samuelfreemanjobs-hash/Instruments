/**
 * Master bus stubs: grit filter + cassette-ish waveshaper (tunable later).
 */

export class MasterChain {
  /** @param {AudioContext} ctx */
  constructor(ctx) {
    this.ctx = ctx;
    this.input = ctx.createGain();
    this.filter = ctx.createBiquadFilter();
    this.shaper = ctx.createWaveShaper();
    this.output = ctx.createGain();

    this.filter.type = 'lowpass';
    this.filter.frequency.value = 9200;
    this.filter.Q.value = 0.7;

    this.setCassetteDrive(0.35);
    this.output.gain.value = 0.85;

    this.input.connect(this.filter);
    this.filter.connect(this.shaper);
    this.shaper.connect(this.output);
    this.output.connect(ctx.destination);
  }

  /** @param {number} amount 0..1 */
  setCassetteDrive(amount) {
    const a = Math.max(0, Math.min(1, amount));
    const curve = new Float32Array(256);
    const k = 1 + a * 12;
    for (let i = 0; i < curve.length; i += 1) {
      const x = (i * 2) / (curve.length - 1) - 1;
      curve[i] = ((1 + k) * x) / (1 + k * Math.abs(x));
    }
    this.shaper.curve = curve;
    this.shaper.oversample = '2x';
  }

  /** @param {number} hz */
  setFilterCutoff(hz) {
    this.filter.frequency.value = Math.max(200, Math.min(18000, hz));
  }

  /** @param {number} q */
  setFilterQ(q) {
    this.filter.Q.value = Math.max(0.1, Math.min(18, q));
  }
}
