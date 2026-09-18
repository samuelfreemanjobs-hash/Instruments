/**
 * Browser preview of Cassette Smash Lite — asymmetric drive, LF bump, wow/flutter wobble.
 * Not bit-identical to the shipping JUCE plugin; tuned for A/B on /launch.
 */

export type CassetteSmashParams = {
  drive: number; // 0..1
  wobble: number; // 0..1
};

function makeAsymmetricCurve(drive: number): Float32Array {
  const samples = 4096;
  const curve = new Float32Array(samples);
  const k = 2 + drive * 18;
  for (let i = 0; i < samples; i += 1) {
    const x = (i * 2) / samples - 1;
    const pos = Math.tanh(k * x);
    const neg = Math.tanh(k * x * 0.72) * 0.92;
    curve[i] = x >= 0 ? pos : neg;
  }
  return curve;
}

export type CassetteSmashNodes = {
  input: GainNode;
  output: GainNode;
  disconnect: () => void;
  setParams: (p: CassetteSmashParams) => void;
};

export function createCassetteSmashChain(
  ctx: AudioContext,
  initial: CassetteSmashParams,
): CassetteSmashNodes {
  const input = ctx.createGain();
  const preGain = ctx.createGain();
  const shaper = ctx.createWaveShaper();
  shaper.oversample = "4x";
  const lfBump = ctx.createBiquadFilter();
  lfBump.type = "lowshelf";
  lfBump.frequency.value = 70;
  lfBump.gain.value = 3.5;

  const hfCut = ctx.createBiquadFilter();
  hfCut.type = "lowpass";
  hfCut.frequency.value = 10500;
  hfCut.Q.value = 0.7;

  const delay = ctx.createDelay(0.05);
  delay.delayTime.value = 0.003;
  const wet = ctx.createGain();
  wet.gain.value = 0.35;
  const dry = ctx.createGain();
  dry.gain.value = 1;

  const wowOsc = ctx.createOscillator();
  wowOsc.type = "sine";
  wowOsc.frequency.value = 0.5;
  const flutterOsc = ctx.createOscillator();
  flutterOsc.type = "sine";
  flutterOsc.frequency.value = 6;
  const wowGain = ctx.createGain();
  wowGain.gain.value = 0.0008;
  const flutterGain = ctx.createGain();
  flutterGain.gain.value = 0.00035;

  wowOsc.connect(wowGain).connect(delay.delayTime);
  flutterOsc.connect(flutterGain).connect(delay.delayTime);

  input.connect(preGain);
  preGain.connect(shaper);
  shaper.connect(lfBump);
  lfBump.connect(hfCut);
  hfCut.connect(dry);
  hfCut.connect(delay);
  delay.connect(wet);

  const output = ctx.createGain();
  dry.connect(output);
  wet.connect(output);

  wowOsc.start();
  flutterOsc.start();

  const setParams = (p: CassetteSmashParams) => {
    const drive = Math.min(1, Math.max(0, p.drive));
    const wobble = Math.min(1, Math.max(0, p.wobble));
    preGain.gain.value = 1 + drive * 4.5;
    shaper.curve = makeAsymmetricCurve(drive) as Float32Array<ArrayBuffer>;
    lfBump.gain.value = 2 + drive * 4;
    wowGain.gain.value = 0.0004 + wobble * 0.0012;
    flutterGain.gain.value = 0.00015 + wobble * 0.0005;
    wet.gain.value = 0.15 + wobble * 0.45;
  };

  setParams(initial);

  return {
    input,
    output,
    disconnect: () => {
      wowOsc.stop();
      flutterOsc.stop();
      input.disconnect();
      output.disconnect();
    },
    setParams,
  };
}
