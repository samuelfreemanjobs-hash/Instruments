/**
 * TR-808 / DR-660 style dual-square cowbell with bandpass + pitch droop.
 */
export class Cowbell808Synth {
  /**
   * @param {AudioContext} audioCtx
   * @param {AudioNode} destinationNode
   * @param {(src: AudioScheduledSourceNode, endTime: number) => void} [onTrack]
   */
  constructor(audioCtx, destinationNode, onTrack) {
    this.audioCtx = audioCtx;
    this.destination = destinationNode;
    this.onTrack = onTrack || null;
  }

  /**
   * @param {number} time
   * @param {number} [velocity=1]
   * @param {number} [pitchMultiplier=1]
   */
  trigger(time, velocity = 1.0, pitchMultiplier = 1.0) {
    const ctx = this.audioCtx;
    const osc1 = ctx.createOscillator();
    const osc2 = ctx.createOscillator();
    osc1.type = 'square';
    osc2.type = 'square';

    const baseFreq1 = 540 * pitchMultiplier;
    const baseFreq2 = 800 * pitchMultiplier;
    osc1.frequency.setValueAtTime(baseFreq1, time);
    osc2.frequency.setValueAtTime(baseFreq2, time);
    osc1.frequency.exponentialRampToValueAtTime(baseFreq1 * 0.985, time + 0.08);
    osc2.frequency.exponentialRampToValueAtTime(baseFreq2 * 0.985, time + 0.08);

    const bandpass = ctx.createBiquadFilter();
    bandpass.type = 'bandpass';
    bandpass.frequency.setValueAtTime(2640 * pitchMultiplier, time);
    bandpass.Q.setValueAtTime(3.5, time);

    const vca = ctx.createGain();
    const peakGain = 0.6 * (velocity * velocity);
    vca.gain.setValueAtTime(0.001, time);
    vca.gain.linearRampToValueAtTime(peakGain, time + 0.004);
    vca.gain.exponentialRampToValueAtTime(Math.max(0.0001, peakGain * 0.25), time + 0.045);
    vca.gain.exponentialRampToValueAtTime(0.0001, time + 0.38);

    osc1.connect(bandpass);
    osc2.connect(bandpass);
    bandpass.connect(vca);
    vca.connect(this.destination);

    const end = time + 0.4;
    osc1.start(time);
    osc2.start(time);
    osc1.stop(end);
    osc2.stop(end);
    if (this.onTrack) {
      this.onTrack(osc1, end);
      this.onTrack(osc2, end);
    }
  }
}
