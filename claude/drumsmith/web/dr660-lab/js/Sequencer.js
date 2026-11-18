/**
 * DR-660–style step sequencer using Web Audio lookahead scheduling.
 * Swing delays odd 0-based 16th indices (even 1-based steps), Roland-style.
 * Supports CHH ratchets and melodic cowbell pitch multipliers.
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
  const swingFactor = (swing - 50) / 50;
  return swingFactor * sixteenthSec;
}

/**
 * @param {number} velocity
 * @param {number} subdivisions
 * @param {number} index
 */
export function ratchetVelocity(velocity, subdivisions, index) {
  if (subdivisions <= 1) return velocity;
  return velocity * (0.65 + 0.35 * (index / (subdivisions - 1)));
}

/**
 * Flatten multi-bar Gemini pattern JSON into sequencer rows.
 * @param {{ bpm?: number, swing?: number, bars: Array<{ tracks: Record<string, number[]>, hiHatRatchets?: number[], cowbellPitch?: number[] }> }} doc
 */
export function flattenMemphisPattern(doc) {
  const tracks = {};
  const hiHatRatchets = [];
  const cowbellPitch = [];
  const trackNames = new Set();
  for (const bar of doc.bars) {
    for (const name of Object.keys(bar.tracks)) trackNames.add(name);
  }
  for (const name of trackNames) tracks[name] = [];

  for (const bar of doc.bars) {
    for (const name of trackNames) {
      const row = bar.tracks[name] || Array(16).fill(0);
      tracks[name].push(...row);
    }
    hiHatRatchets.push(...(bar.hiHatRatchets || Array(16).fill(1)));
    cowbellPitch.push(...(bar.cowbellPitch || Array(16).fill(1)));
  }

  return {
    bpm: doc.bpm ?? 91,
    swing: doc.swing ?? 57,
    totalSteps: doc.bars.length * 16,
    pattern: tracks,
    hiHatRatchets,
    cowbellPitch,
  };
}

/**
 * @param {{ nextNoteTime: number, currentStep: number, totalSteps: number, getSixteenthDuration: () => number }} state
 * @param {number} now
 * @param {number} [graceSec=0.01]
 */
export function catchUpExpiredSteps(state, now, graceSec = 0.01) {
  let skipped = 0;
  const limit = now - graceSec;
  while (state.nextNoteTime < limit) {
    state.nextNoteTime += state.getSixteenthDuration();
    state.currentStep = (state.currentStep + 1) % state.totalSteps;
    skipped += 1;
    if (skipped > 10000) break;
  }
  return skipped;
}

export class DR660Sequencer {
  /**
   * @param {AudioContext} audioCtx
   * @param {(trackId: string, time: number, velocity: number, opts?: { pitch?: number }) => void} triggerSound
   * @param {(stepIndex: number) => void} [onStepVisual]
   * @param {() => void} [onStopAudio]
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

    /** @type {number[]} */
    this.hiHatRatchets = Array(16).fill(1);
    /** @type {number[]} */
    this.cowbellPitch = Array(16).fill(1);

    /** Lock + Mutate — locked tracks keep their rows when mutating */
    this.locked = {
      kick: true,
      snare: true,
      rim: false,
      chh: false,
      ohh: false,
      cowbell: false,
      perc1: false,
      perc2: false,
    };

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

    if (triggerTime < this.audioCtx.currentTime - 0.002) return;

    for (const track of Object.keys(this.pattern)) {
      const row = this.pattern[track];
      if (!row || stepNumber >= row.length) continue;
      const velocity = row[stepNumber];
      if (!(velocity > 0)) continue;

      if (track === 'chh') {
        const subdivisions = Math.max(1, this.hiHatRatchets[stepNumber] || 1);
        if (subdivisions > 1) {
          const subStep = sixteenth / subdivisions;
          for (let i = 0; i < subdivisions; i += 1) {
            const subTime = triggerTime + i * subStep;
            if (subTime < this.audioCtx.currentTime - 0.002) continue;
            this.triggerSound(track, subTime, ratchetVelocity(velocity, subdivisions, i));
          }
          continue;
        }
      }

      const pitch = track === 'cowbell' ? this.cowbellPitch[stepNumber] || 1 : 1;
      this.triggerSound(track, triggerTime, velocity, { pitch });
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
    this.transportGen += 1;
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

  /** @param {ReturnType<typeof flattenMemphisPattern>} flat */
  applyFlattenedPattern(flat) {
    this.bpm = flat.bpm;
    this.swing = flat.swing;
    this.totalSteps = flat.totalSteps;
    this.pattern = flat.pattern;
    this.hiHatRatchets = flat.hiHatRatchets;
    this.cowbellPitch = flat.cowbellPitch;
    this.currentStep = 0;
  }

  /** @param {string} track */
  toggleLock(track) {
    if (!(track in this.locked)) return false;
    this.locked[track] = !this.locked[track];
    return this.locked[track];
  }

  /**
   * Regenerate unlocked tracks (hats, perc, cowbell melody) — kick/snare stay if locked.
   * @param {() => number} [rng=Math.random]
   */
  mutateUnlocked(rng = Math.random) {
    const n = this.totalSteps;
    const pitchChoices = [1, 0.891, 1.059, 1.189, 0.794, 1.335];

    if (!this.locked.chh) {
      this.pattern.chh = Array.from({ length: n }, (_, i) => {
        if (i % 2 === 0) return 0.55 + rng() * 0.45;
        return rng() < 0.35 ? 0.45 : 0;
      });
      this.hiHatRatchets = Array.from({ length: n }, () => {
        const r = rng();
        if (r < 0.08) return 4;
        if (r < 0.18) return 3;
        return 1;
      });
    }
    if (!this.locked.ohh) {
      this.pattern.ohh = Array.from({ length: n }, (_, i) =>
        i % 8 === 7 && rng() < 0.55 ? 0.75 : 0,
      );
    }
    if (!this.locked.rim) {
      this.pattern.rim = Array.from({ length: n }, () => (rng() < 0.12 ? 0.5 + rng() * 0.3 : 0));
    }
    if (!this.locked.cowbell) {
      this.pattern.cowbell = Array.from({ length: n }, (_, i) => {
        if (i % 4 === 0) return 0.85 + rng() * 0.15;
        if (i % 4 === 3 && rng() < 0.5) return 0.7;
        return rng() < 0.08 ? 0.6 : 0;
      });
      this.cowbellPitch = Array.from({ length: n }, () => pitchChoices[Math.floor(rng() * pitchChoices.length)]);
    }
    if (!this.locked.perc1) {
      this.pattern.perc1 = Array.from({ length: n }, () => (rng() < 0.1 ? 0.5 : 0));
    }
    if (!this.locked.perc2) {
      this.pattern.perc2 = Array.from({ length: n }, () => (rng() < 0.08 ? 0.5 : 0));
    }
    // Ghost snare fills only when snare unlocked
    if (!this.locked.snare) {
      this.pattern.snare = Array.from({ length: n }, (_, i) => {
        if (i % 16 === 4 || i % 16 === 12) return 1;
        return rng() < 0.06 ? 0.45 : 0;
      });
    }
    if (!this.locked.kick) {
      this.pattern.kick = Array.from({ length: n }, (_, i) => {
        if (i % 8 === 0) return 1;
        if (i % 8 === 6 && rng() < 0.6) return 0.7;
        return rng() < 0.05 ? 0.6 : 0;
      });
    }
  }

  /** @param {string} name */
  loadPreset(name) {
    if (name === 'crawl') {
      this.totalSteps = 16;
      this.hiHatRatchets = Array(16).fill(1);
      this.cowbellPitch = Array(16).fill(1);
      this.bpm = 94;
      this.swing = 54;
      this.pattern.kick = [1, 0, 0, 0, 0, 0, 0.6, 0, 1, 0, 0, 0, 0, 0, 0, 0];
      this.pattern.snare = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0];
      this.pattern.rim = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
      this.pattern.chh = [0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0, 0.6, 0];
      this.pattern.cowbell = [1, 0, 0, 0.6, 0, 0, 1, 0, 0, 0.6, 0, 0, 1, 0, 0.6, 0];
      this.pattern.ohh = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
      this.pattern.perc1 = Array(16).fill(0);
      this.pattern.perc2 = Array(16).fill(0);
      this.hiHatRatchets = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 4];
    } else if (name === 'bounce') {
      this.totalSteps = 16;
      this.bpm = 134;
      this.swing = 58;
      this.pattern.kick = [1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0.6, 0];
      this.pattern.snare = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.5];
      this.pattern.rim = Array(16).fill(0);
      this.pattern.chh = [1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5];
      this.pattern.cowbell = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0];
      this.pattern.ohh = [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0];
      this.pattern.perc1 = Array(16).fill(0);
      this.pattern.perc2 = Array(16).fill(0);
      this.hiHatRatchets = Array(16).fill(1);
      this.cowbellPitch = Array(16).fill(1);
    }
  }
}
