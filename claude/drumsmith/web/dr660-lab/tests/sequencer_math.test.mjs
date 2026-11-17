import { describe, it } from 'node:test';
import assert from 'node:assert/strict';
import { sixteenthDurationSec, swingOffsetSec } from '../js/Sequencer.js';

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
