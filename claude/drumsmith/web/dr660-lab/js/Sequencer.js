/**
 * DR-660–style 16-step sequencer using Web Audio lookahead scheduling.
 * Swing delays odd 0-based 16th indices (even 1-based steps), Roland-style.
 */

/** @param {number} bpm */
export function sixteenthDurationSec(bpm) {
  const b = Math.max(50, Math.min(200, bpm));
  return 60 / b / 4;
}

/**
 * @param {number} stepNumber 0-based step index
 * @param {number} swingPercent 50–75
 * @param {number} sixteenthSec
 * @returns {number} delay seconds to add to even 16ths (1-based)
 */
export function swingOffsetSec(stepNumber, swingPercent, sixteenthSec) {
  const swing = Math.max(50, Math.min(75, swingPercent));
  if (stepNumber % 2 !== 1 || swing <= 50) return 0;
  const swingFactor = (swing - 50) / 50; // 50→0 … 75→0.5
  return swingFactor * sixteenthSec;
}

export class DR660Sequencer {
  /**
   * @param {AudioContext} audioCtx
   * @param {(trackId: string, time: number, velocity: number) => void} triggerSound
   * @param {(stepIndex: number) => void} [onStepVisual]
   */
  constructor(audioCtx, triggerSound, onStepVisual) {
    this.audioCtx = audioCtx;
    this.triggerSound = triggerSound;
    this.onStepVisual = onStepVisual || null;

    this.isPlaying = false;
    this.bpm = 94; // Memphis crawl default (CNV lane)
    this.swing = 56;
    this.currentStep = 0;
    this.totalSteps = 16;

    this.lookahead = 25.0;
    this.scheduleAheadTime = 0.1;
    this.nextNoteTime = 0.0;
    this.timerID = null;

    /** @type {Record<string, number[]>} 0 off · ~0.5–0.6 ghost · 1.0 accent */
    this.pattern = {
      kick: [1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0],
      snare: [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
      rim: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
      chh: [1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5],
      ohh: [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
      cowbell: [1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0],
      perc1: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      perc2: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    };
  }

  getSixteenthDuration() {
    return sixteenthDurationSec(this.bpm);
  }

  nextNote() {
    this.nextNoteTime += this.getSixteenthDuration();
    this.currentStep += 1;
    if (this.currentStep >= this.totalSteps) this.currentStep = 0;
  }

  /**
   * @param {number} stepNumber
   * @param {number} baseTime
   */
  scheduleNote(stepNumber, baseTime) {
    const sixteenth = this.getSixteenthDuration();
    const triggerTime = baseTime + swingOffsetSec(stepNumber, this.swing, sixteenth);

    for (const track of Object.keys(this.pattern)) {
      const velocity = this.pattern[track][stepNumber];
      if (velocity > 0) this.triggerSound(track, triggerTime, velocity);
    }

    const timeUntilTriggerMs = Math.max(0, (triggerTime - this.audioCtx.currentTime) * 1000);
    const step = stepNumber;
    setTimeout(() => {
      if (this.isPlaying && this.onStepVisual) this.onStepVisual(step);
    }, timeUntilTriggerMs);
  }

  scheduler() {
    while (this.nextNoteTime < this.audioCtx.currentTime + this.scheduleAheadTime) {
      this.scheduleNote(this.currentStep, this.nextNoteTime);
      this.nextNote();
    }
  }

  async start() {
    if (this.isPlaying) return;
    if (this.audioCtx.state === 'suspended') await this.audioCtx.resume();

    this.isPlaying = true;
    this.currentStep = 0;
    this.nextNoteTime = this.audioCtx.currentTime + 0.05;
    this.timerID = setInterval(() => this.scheduler(), this.lookahead);
  }

  stop() {
    this.isPlaying = false;
    if (this.timerID != null) clearInterval(this.timerID);
    this.timerID = null;
    if (this.onStepVisual) this.onStepVisual(-1);
  }

  setBpm(newBpm) {
    this.bpm = Math.max(50, Math.min(200, Number(newBpm) || this.bpm));
  }

  setSwing(newSwing) {
    this.swing = Math.max(50, Math.min(75, Number(newSwing) || this.swing));
  }

  /**
   * Cycle: Off → Normal → Accent → Off
   * @param {string} track
   * @param {number} stepIndex
   */
  toggleStep(track, stepIndex) {
    const row = this.pattern[track];
    if (!row || stepIndex < 0 || stepIndex >= row.length) return 0;
    const current = row[stepIndex];
    if (current === 0) row[stepIndex] = 0.6;
    else if (current < 1.0) row[stepIndex] = 1.0;
    else row[stepIndex] = 0;
    return row[stepIndex];
  }

  /** @param {string} name */
  loadPreset(name) {
    if (name === 'crawl') {
      this.bpm = 94;
      this.swing = 54;
      this.pattern.kick = [1, 0, 0, 0, 0, 0, 0.6, 0, 1, 0, 0, 0, 0, 0, 0, 0];
      this.pattern.snare = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
      this.pattern.chh = [0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0];
      this.pattern.cowbell = [1, 0, 0, 0.6, 0, 0, 1, 0, 0, 0.6, 0, 0, 1, 0, 0.6, 0];
      this.pattern.ohh = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    } else if (name === 'bounce') {
      this.bpm = 134;
      this.swing = 58;
      this.pattern.kick = [1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0.6, 0];
      this.pattern.snare = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.5];
      this.pattern.chh = [1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5];
      this.pattern.cowbell = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0];
      this.pattern.ohh = [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0];
    }
  }
}
