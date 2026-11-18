/**
 * Lo-fi tape delay: dry/wet + filtered feedback loop (degrading repeats).
 */
export class LoFiTapeDelay {
  /** @param {AudioContext} audioCtx */
  constructor(audioCtx) {
    this.audioCtx = audioCtx;
    this.inputNode = audioCtx.createGain();
    this.outputNode = audioCtx.createGain();

    this.delayNode = audioCtx.createDelay(2.0);
    // Dotted 8th ≈ at 94 BPM: (60/94)*(3/4) ≈ 0.479; Gemini used 0.375 @ ~92
    this.delayNode.delayTime.value = 0.375;

    this.feedbackFilter = audioCtx.createBiquadFilter();
    this.feedbackFilter.type = 'lowpass';
    this.feedbackFilter.frequency.value = 3200;

    this.feedbackGain = audioCtx.createGain();
    this.feedbackGain.gain.value = 0.45;

    this.wetGain = audioCtx.createGain();
    this.wetGain.gain.value = 0.35;

    this.dryGain = audioCtx.createGain();
    this.dryGain.gain.value = 0.8;

    // Dry
    this.inputNode.connect(this.dryGain);
    this.dryGain.connect(this.outputNode);

    // Wet
    this.inputNode.connect(this.delayNode);
    this.delayNode.connect(this.wetGain);
    this.wetGain.connect(this.outputNode);

    // Feedback: delay → dark LPF → feedback → delay
    this.delayNode.connect(this.feedbackFilter);
    this.feedbackFilter.connect(this.feedbackGain);
    this.feedbackGain.connect(this.delayNode);
  }

  /** @param {number} timeInSeconds */
  setDelayTime(timeInSeconds) {
    const t = Math.max(0.01, Math.min(2, timeInSeconds));
    this.delayNode.delayTime.setTargetAtTime(t, this.audioCtx.currentTime, 0.05);
  }

  /** @param {number} amount */
  setFeedback(amount) {
    const a = Math.min(0.85, Math.max(0, amount));
    this.feedbackGain.gain.setTargetAtTime(a, this.audioCtx.currentTime, 0.05);
  }

  /** @param {number} wet 0..1 */
  setWet(wet) {
    const w = Math.max(0, Math.min(1, wet));
    this.wetGain.gain.setTargetAtTime(w, this.audioCtx.currentTime, 0.05);
  }

  /** Sync delay time to dotted-8th of BPM. */
  setDelayFromBpm(bpm) {
    const dotted8 = (60 / Math.max(50, bpm)) * 0.75;
    this.setDelayTime(dotted8);
  }
}
