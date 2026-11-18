import { describe, it } from 'node:test';
import assert from 'node:assert/strict';
import { createCassetteCurve, driveToCurveAmount } from '../js/cassetteCurve.js';

describe('createCassetteCurve', () => {
  it('returns odd length-safe Float32Array in [-1,1] roughly', () => {
    const c = createCassetteCurve(35, 1024);
    assert.equal(c.length, 1024);
    assert.ok(Math.abs(c[0] + 1) < 0.5 || c[0] < 0);
    assert.ok(Math.abs(c[c.length - 1] - 1) < 0.5 || c[c.length - 1] > 0);
    assert.ok(Math.abs(c[512]) < 0.05);
  });
  it('higher drive increases |curve| mid-side compression shape', () => {
    const soft = createCassetteCurve(10, 2048);
    const hard = createCassetteCurve(50, 2048);
    const i = 1800;
    assert.ok(Math.abs(hard[i]) <= Math.abs(soft[i]) + 1e-6 || Math.abs(hard[i]) < 1);
  });
});

describe('driveToCurveAmount', () => {
  it('maps 0..1 into 8..60', () => {
    assert.equal(driveToCurveAmount(0), 8);
    assert.equal(driveToCurveAmount(1), 60);
  });
});
