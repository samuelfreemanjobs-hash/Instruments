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

/**
 * Advance pattern clock past expired times without scheduling.
 * @param {{ nextNoteTime: number, currentStep: number, totalSteps: number, getSixteenthDuration: () => number }} state
 * @param {number} now
 * @param {number} [graceSec=0.01]
 * @returns {number} steps skipped
 */
export function catchUpExpiredSteps(state, now, graceSec = 0.01) {
  let skipped = 0;
  const limit = now - graceSec;
  while (state.nextNoteTime < limit) {
    state.nextNoteTime += state.getSixteenthDuration();
    state.currentStep = (state.currentStep + 1) % state.totalSteps;
    skipped += 1;
    if (skipped > 10000) break; // safety
  }
  return skipped;
}

export class DR660Sequencer {
  /**
   * @param {AudioContext} audioCtx
   * @param {(trackId: string, time: number, velocity: number) => void} triggerSound
   * @param {(stepIndex: number) => void} [onStepVisual]
   * @param {() => void} [onStopAudio] cancel pending scheduled voices
   */
  constructor(audioCtx, triggerSound, onStepVisual, onStopAudio) {
    this.audioCtx = audioCtx;
    this.triggerSound = triggerSound;
    this.onStepVisual = onStepVisual || null;
    this.onStopAudio = onStopAudio || null;

    this.isPlaying = false;
    this.bpm = 94;
    this.swing = 56;
    this.currentStep = 0;
    this.totalSteps = 16;
    this.transportGen = 0;

    this.lookahead = 25.0;
    this.scheduleAheadTime = 0.1;
    this.nextNoteTime = 0.0;
    this.timerID = null;

    /** @type {Record<string, number[]>} */
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
   * @param {number} gen
   */
  scheduleNote(stepNumber, baseTime, gen) {
    const sixteenth = this.getSixteenthDuration();
    const triggerTime = baseTime + swingOffsetSec(stepNumber, this.swing, sixteenth);

    // Do not start audio for notes already in the past
    if (triggerTime < this.audioCtx.currentTime - 0.002) return;

    for (const track of Object.keys(this.pattern)) {
      const velocity = this.pattern[track][stepNumber];
      if (velocity > 0) this.triggerSound(track, triggerTime, velocity);
    }

    const timeUntilTriggerMs = Math.max(0, (triggerTime - this.audioCtx.currentTime) * 1000);
    const step = stepNumber;
    setTimeout(() => {
      if (this.isPlaying && this.transportGen === gen && this.onStepVisual) {
        this.onStepVisual(step);
      }
    }, timeUntilTriggerMs);
  }

  scheduler() {
    if (!this.isPlaying) return;
    const now = this.audioCtx.currentTime;
    catchUpExpiredSteps(this, now, 0.01);
    const gen = this.transportGen;
    while (this.nextNoteTime < now + this.scheduleAheadTime) {
      this.scheduleNote(this.currentStep, this.nextNoteTime, gen);
      this.nextNote();
    }
  }

  async start() {
    if (this.isPlaying) return;
    if (this.audioCtx.state === 'suspended') await this.audioCtx.resume();

    this.transportGen += 1;
    this.isPlaying = true;
    this.currentStep = 0;
    this.nextNoteTime = this.audioCtx.currentTime + 0.05;
    this.timerID = setInterval(() => this.scheduler(), this.lookahead);
  }

  stop() {
    this.isPlaying = false;
    this.transportGen += 1; // invalidate pending visual + gate late audio via engine stop
    if (this.timerID != null) clearInterval(this.timerID);
    this.timerID = null;
    if (this.onStopAudio) this.onStopAudio();
    if (this.onStepVisual) this.onStepVisual(-1);
  }

  setBpm(newBpm) {
    this.bpm = Math.max(50, Math.min(200, Number(newBpm) || this.bpm));
  }

  setSwing(newSwing) {
    this.swing = Math.max(50, Math.min(75, Number(newSwing) || this.swing));
  }

  /**
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
