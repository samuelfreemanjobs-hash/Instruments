import { describe, it } from 'node:test';
import assert from 'node:assert/strict';
import {
  sixteenthDurationSec,
  swingOffsetSec,
  catchUpExpiredSteps,
  ratchetVelocity,
  flattenMemphisPattern,
} from '../js/Sequencer.js';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

describe('sixteenthDurationSec', () => {
  it('matches 60/bpm/4 at 94 BPM', () => {
    assert.ok(Math.abs(sixteenthDurationSec(94) - 60 / 94 / 4) < 1e-12);
  });
  it('clamps bpm', () => {
    assert.equal(sixteenthDurationSec(10), sixteenthDurationSec(50));
    assert.equal(sixteenthDurationSec(999), sixteenthDurationSec(200));
  });
});

describe('swingOffsetSec', () => {
  const six = sixteenthDurationSec(120); // 0.125
  it('is zero at 50% or on even 0-based steps', () => {
    assert.equal(swingOffsetSec(1, 50, six), 0);
    assert.equal(swingOffsetSec(0, 66, six), 0);
  });
  it('delays odd 0-based steps when swing > 50', () => {
    const off = swingOffsetSec(1, 75, six);
    assert.ok(Math.abs(off - 0.5 * six) < 1e-12);
  });
  it('scales linearly between 50 and 75', () => {
    const mid = swingOffsetSec(3, 62.5, six);
    assert.ok(Math.abs(mid - 0.25 * six) < 1e-12);
  });
});

describe('catchUpExpiredSteps', () => {
  it('skips past notes and advances step clock', () => {
    const six = 0.1;
    const state = {
      nextNoteTime: 0,
      currentStep: 0,
      totalSteps: 16,
      getSixteenthDuration: () => six,
    };
    const skipped = catchUpExpiredSteps(state, 0.35, 0.01);
    assert.equal(skipped, 4); // 0, 0.1, 0.2, 0.3 are past 0.34 limit
    assert.ok(Math.abs(state.nextNoteTime - 0.4) < 1e-12);
    assert.equal(state.currentStep, 4);
  });

  it('does nothing when next note is still ahead', () => {
    const state = {
      nextNoteTime: 1.0,
      currentStep: 3,
      totalSteps: 16,
      getSixteenthDuration: () => 0.1,
    };
    assert.equal(catchUpExpiredSteps(state, 0.5, 0.01), 0);
    assert.equal(state.currentStep, 3);
  });
});

describe('ratchetVelocity', () => {
  it('ramps from 0.65× to 1.0× across subdivisions', () => {
    assert.ok(Math.abs(ratchetVelocity(1, 4, 0) - 0.65) < 1e-12);
    assert.ok(Math.abs(ratchetVelocity(1, 4, 3) - 1) < 1e-12);
  });
});

describe('flattenMemphisPattern', () => {
  it('loads 4-bar JSON to 64 steps with ratchets and pitch', () => {
    const here = dirname(fileURLToPath(import.meta.url));
    const raw = JSON.parse(
      readFileSync(join(here, '../data/memphis_4bar_pattern.json'), 'utf8'),
    );
    const flat = flattenMemphisPattern(raw);
    assert.equal(flat.totalSteps, 64);
    assert.equal(flat.pattern.kick.length, 64);
    assert.equal(flat.hiHatRatchets.length, 64);
    assert.equal(flat.cowbellPitch.length, 64);
    assert.equal(flat.bpm, 91);
    assert.equal(flat.hiHatRatchets[10], 3); // bar1 step 11
    assert.ok(Math.abs(flat.cowbellPitch[6] - 1.059) < 1e-12);
  });
});

describe('mutateUnlocked lock invariant', () => {
  it('keeps locked kick/snare rows identical', async () => {
    const { DR660Sequencer } = await import('../js/Sequencer.js');
    const seq = new DR660Sequencer(
      { currentTime: 0, state: 'running', resume: async () => {} },
      () => {},
    );
    seq.loadPreset('crawl');
    const kick = [...seq.pattern.kick];
    const snare = [...seq.pattern.snare];
    let i = 0;
    const rng = () => {
      i += 1;
      return (i % 10) / 10;
    };
    seq.mutateUnlocked(rng);
    assert.deepEqual(seq.pattern.kick, kick);
    assert.deepEqual(seq.pattern.snare, snare);
    assert.ok(seq.pattern.chh.some((v, idx) => v !== 0.6 || idx % 2 !== 0) || seq.hiHatRatchets.some((r) => r > 1));
  });
});
