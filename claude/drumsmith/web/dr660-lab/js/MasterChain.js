/**
 * Memphis master FX bus:
 * voices → pre-emphasis HPF → bitcrush (12-bit / rate reduce) → dark LPF → cassette shaper → out
 */
import { createCassetteCurve, driveToCurveAmount } from './cassetteCurve.js';

export class MemphisFXBus {
  /** @param {AudioContext} ctx */
  constructor(ctx) {
    this.ctx = ctx;
    this.input = ctx.createGain(); // entry for all drum voices
    this.preEmphasis = ctx.createBiquadFilter();
    this.filter = ctx.createBiquadFilter();
    this.saturation = ctx.createWaveShaper();
    this.outputGain = ctx.createGain();
    /** @type {AudioWorkletNode | null} */
    this.bitcrush = null;
    this._bitcrushReady = false;

    this.preEmphasis.type = 'highpass';
    this.preEmphasis.frequency.value = 80;
    this.preEmphasis.Q.value = 0.5;

    this.filter.type = 'lowpass';
    this.filter.frequency.value = 9500;
    this.filter.Q.value = 1.2;

    this.outputGain.gain.value = 0.8;
    this.setDrive(0.45);

    // Temporary dry path until worklet loads
    this.input.connect(this.preEmphasis);
    this.preEmphasis.connect(this.filter);
    this.filter.connect(this.saturation);
    this.saturation.connect(this.outputGain);
    this.outputGain.connect(ctx.destination);
  }

  /** Load AudioWorklet bitcrusher and insert into chain. */
  async initBitcrush() {
    if (this._bitcrushReady) return;
    const url = new URL('./bitcrush-processor.js', import.meta.url);
    await this.ctx.audioWorklet.addModule(url.href);
    this.bitcrush = new AudioWorkletNode(this.ctx, 'bitcrush-processor', {
      numberOfInputs: 1,
      numberOfOutputs: 1,
      outputChannelCount: [1],
      parameterData: { bits: 12, downsample: 4, mix: 0.85 },
    });

    // Rebuild: input → HPF → bitcrush → LPF → shaper → out
    this.preEmphasis.disconnect();
    this.preEmphasis.connect(this.bitcrush);
    this.bitcrush.connect(this.filter);
    this._bitcrushReady = true;
    this.setBitcrush(12, 4, 0.85);
  }

  /** @param {number} drive01 0..1 UI drive */
  setDrive(drive01) {
    const amount = driveToCurveAmount(drive01);
    this.saturation.curve = createCassetteCurve(amount);
    this.saturation.oversample = '4x';
  }

  /** @deprecated use setDrive */
  setCassetteDrive(amount) {
    this.setDrive(amount);
  }

  /** @param {number} hz */
  setFilterCutoff(hz) {
    const t = this.ctx.currentTime;
    this.filter.frequency.setValueAtTime(Math.max(200, Math.min(18000, hz)), t);
  }

  /** @param {number} q */
  setFilterQ(q) {
    const t = this.ctx.currentTime;
    this.filter.Q.setValueAtTime(Math.max(0.1, Math.min(18, q)), t);
  }

  /**
   * @param {number} bits
   * @param {number} downsample
   * @param {number} mix
   */
  setBitcrush(bits, downsample, mix) {
    if (!this.bitcrush) return;
    const t = this.ctx.currentTime;
    this.bitcrush.parameters.get('bits').setValueAtTime(bits, t);
    this.bitcrush.parameters.get('downsample').setValueAtTime(downsample, t);
    this.bitcrush.parameters.get('mix').setValueAtTime(mix, t);
  }
}

/** Back-compat alias */
export class MasterChain extends MemphisFXBus {}
