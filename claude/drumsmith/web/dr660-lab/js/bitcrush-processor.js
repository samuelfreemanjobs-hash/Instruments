/**
 * AudioWorklet: sample-hold rate reduction + N-bit quantize (12-bit Memphis grit).
 */
class BitcrushProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return [
      { name: 'bits', defaultValue: 12, minValue: 4, maxValue: 16, automationRate: 'k-rate' },
      { name: 'downsample', defaultValue: 4, minValue: 1, maxValue: 32, automationRate: 'k-rate' },
      { name: 'mix', defaultValue: 1, minValue: 0, maxValue: 1, automationRate: 'k-rate' },
    ];
  }

  constructor() {
    super();
    this._hold = 0;
    this._count = 0;
  }

  /**
   * @param {Float32Array[][]} inputs
   * @param {Float32Array[][]} outputs
   * @param {Record<string, Float32Array>} parameters
   */
  process(inputs, outputs, parameters) {
    const input = inputs[0];
    const output = outputs[0];
    if (!input || !input[0] || !output || !output[0]) return true;

    const bits = parameters.bits[0];
    const downsample = Math.max(1, Math.floor(parameters.downsample[0]));
    const mix = parameters.mix[0];
    const levels = Math.pow(2, bits - 1);
    const inCh = input[0];
    const outCh = output[0];

    for (let i = 0; i < outCh.length; i += 1) {
      const dry = inCh[i] || 0;
      if (this._count <= 0) {
        this._hold = Math.round(dry * levels) / levels;
        this._count = downsample;
      }
      this._count -= 1;
      outCh[i] = dry * (1 - mix) + this._hold * mix;
    }

    // Mirror mono crush to other channels if present
    for (let c = 1; c < output.length; c += 1) {
      if (output[c]) output[c].set(outCh);
    }
    return true;
  }
}

registerProcessor('bitcrush-processor', BitcrushProcessor);
