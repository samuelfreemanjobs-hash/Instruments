/**
 * Core program parameters for live edit via NRPN (MSB 0) / CC.
 * UltraNova aligns closely with Novation MiniNova NRPN map; verify against:
 * https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/novation/downloads/10539/ultranova-midi-implementation2.pdf
 */

export type ParamDef = {
  id: string;
  label: string;
  section: string;
  min: number;
  max: number;
  defaultValue: number;
  nrpnLsb?: number;
  nrpnMsb?: number;
  cc?: number;
  hint?: string;
};

export const PARAMETER_SECTIONS = [
  "Voice",
  "Oscillators",
  "Mixer",
  "Filter 1",
  "Filter 2",
  "Filter envelope",
  "Amp envelope",
  "LFO",
  "FX levels",
] as const;

export const ULTRANOVA_PARAMETERS: ParamDef[] = [
  { id: "portamento", label: "Portamento", section: "Voice", min: 0, max: 127, defaultValue: 0, cc: 5 },
  { id: "polyMode", label: "Polyphony mode", section: "Voice", min: 0, max: 4, defaultValue: 2, nrpnMsb: 0, nrpnLsb: 3 },
  { id: "keyboardOctave", label: "Keyboard octave", section: "Voice", min: 124, max: 4, defaultValue: 0, cc: 13 },
  { id: "osc1Wave", label: "Osc 1 wave", section: "Oscillators", min: 0, max: 71, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 19 },
  { id: "osc1Semi", label: "Osc 1 semitones", section: "Oscillators", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 26 },
  { id: "osc1Level", label: "Osc 1 level", section: "Mixer", min: 0, max: 127, defaultValue: 100, nrpnMsb: 0, nrpnLsb: 51 },
  { id: "osc2Wave", label: "Osc 2 wave", section: "Oscillators", min: 0, max: 71, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 29 },
  { id: "osc2Semi", label: "Osc 2 semitones", section: "Oscillators", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 37 },
  { id: "osc2Level", label: "Osc 2 level", section: "Mixer", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 52 },
  { id: "osc3Wave", label: "Osc 3 wave", section: "Oscillators", min: 0, max: 71, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 41 },
  { id: "osc3Level", label: "Osc 3 level", section: "Mixer", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 53 },
  { id: "noiseLevel", label: "Noise level", section: "Mixer", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 56 },
  { id: "filterRoute", label: "Filter routing", section: "Filter 1", min: 0, max: 5, defaultValue: 1, nrpnMsb: 0, nrpnLsb: 60 },
  { id: "filter1Type", label: "Filter 1 type", section: "Filter 1", min: 0, max: 13, defaultValue: 3, cc: 68, nrpnMsb: 0, nrpnLsb: 67 },
  { id: "filter1Freq", label: "Filter 1 frequency", section: "Filter 1", min: 0, max: 127, defaultValue: 90, cc: 74, nrpnMsb: 0, nrpnLsb: 73 },
  { id: "filter1Res", label: "Filter 1 resonance", section: "Filter 1", min: 0, max: 127, defaultValue: 0, cc: 71, nrpnMsb: 0, nrpnLsb: 70 },
  { id: "filter1Drive", label: "Filter 1 drive", section: "Filter 1", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 62 },
  { id: "filter2Freq", label: "Filter 2 frequency", section: "Filter 2", min: 0, max: 127, defaultValue: 127, cc: 83 },
  { id: "filter2Res", label: "Filter 2 resonance", section: "Filter 2", min: 0, max: 127, defaultValue: 0, cc: 85 },
  { id: "filtEnvAtk", label: "Filter env attack", section: "Filter envelope", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 1 },
  { id: "filtEnvDec", label: "Filter env decay", section: "Filter envelope", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 2 },
  { id: "filtEnvSus", label: "Filter env sustain", section: "Filter envelope", min: 0, max: 127, defaultValue: 127, nrpnMsb: 0, nrpnLsb: 3 },
  { id: "filtEnvRel", label: "Filter env release", section: "Filter envelope", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 4 },
  { id: "ampEnvAtk", label: "Amp env attack", section: "Amp envelope", min: 0, max: 127, defaultValue: 0, nrpnMsb: 0, nrpnLsb: 15 },
  { id: "ampEnvDec", label: "Amp env decay", section: "Amp envelope", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 16 },
  { id: "ampEnvSus", label: "Amp env sustain", section: "Amp envelope", min: 0, max: 127, defaultValue: 127, nrpnMsb: 0, nrpnLsb: 17 },
  { id: "ampEnvRel", label: "Amp env release", section: "Amp envelope", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 18 },
  { id: "lfo1Rate", label: "LFO 1 rate", section: "LFO", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 76 },
  { id: "lfo2Rate", label: "LFO 2 rate", section: "LFO", min: 0, max: 127, defaultValue: 64, nrpnMsb: 0, nrpnLsb: 85 },
  { id: "fxPan", label: "FX pan depth", section: "FX levels", min: 0, max: 127, defaultValue: 64, cc: 10 },
  { id: "fx1Level", label: "FX slot 1 level", section: "FX levels", min: 0, max: 127, defaultValue: 0, cc: 91 },
  { id: "fx2Level", label: "FX slot 2 level", section: "FX levels", min: 0, max: 127, defaultValue: 0, cc: 92 },
];

export function defaultParameterState(): Record<string, number> {
  const state: Record<string, number> = {};
  for (const p of ULTRANOVA_PARAMETERS) state[p.id] = p.defaultValue;
  return state;
}
