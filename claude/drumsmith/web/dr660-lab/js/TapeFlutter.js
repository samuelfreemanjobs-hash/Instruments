/**
 * Cassette wow (slow) + flutter (fast) via dual-LFO modulated delay.
 */
export class TapeWowAndFlutter {
  /** @param {AudioContext} audioCtx */
  constructor(audioCtx) {
    this.audioCtx = audioCtx;
    this.inputNode = audioCtx.createGain();
    this.outputNode = audioCtx.createGain();

    this.modDelay = audioCtx.createDelay(0.1);
    this.modDelay.delayTime.setValueAtTime(0.015, audioCtx.currentTime);

    this.wowOsc = audioCtx.createOscillator();
    this.wowOsc.type = 'sine';
    this.wowOsc.frequency.setValueAtTime(0.85, audioCtx.currentTime);
    this.wowDepth = audioCtx.createGain();
    this.wowDepth.gain.setValueAtTime(0.0018, audioCtx.currentTime);

    this.flutterOsc = audioCtx.createOscillator();
    this.flutterOsc.type = 'triangle';
    this.flutterOsc.frequency.setValueAtTime(7.4, audioCtx.currentTime);
    this.flutterDepth = audioCtx.createGain();
    this.flutterDepth.gain.setValueAtTime(0.0004, audioCtx.currentTime);

    this.wowOsc.connect(this.wowDepth);
    this.wowDepth.connect(this.modDelay.delayTime);
    this.flutterOsc.connect(this.flutterDepth);
    this.flutterDepth.connect(this.modDelay.delayTime);

    this.inputNode.connect(this.modDelay);
    this.modDelay.connect(this.outputNode);

    this.wowOsc.start();
    this.flutterOsc.start();
  }

  /** @param {number} amount 0..1 tape wear */
  setWearAmount(amount) {
    const a = Math.max(0, Math.min(1, amount));
    const now = this.audioCtx.currentTime;
    this.wowDepth.gain.setTargetAtTime(a * 0.0035, now, 0.05);
    this.flutterDepth.gain.setTargetAtTime(a * 0.0009, now, 0.05);
  }
}
