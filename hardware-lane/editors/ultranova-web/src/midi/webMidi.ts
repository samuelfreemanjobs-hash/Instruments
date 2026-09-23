export type MidiPorts = {
  inputs: MIDIInput[];
  outputs: MIDIOutput[];
};

export async function requestMidiAccess(): Promise<MIDIAccess> {
  if (!navigator.requestMIDIAccess) {
    throw new Error("Web MIDI is not supported in this browser (use Chrome or Edge).");
  }
  return navigator.requestMIDIAccess({ sysex: true });
}

export function listPorts(access: MIDIAccess): MidiPorts {
  return {
    inputs: [...access.inputs.values()],
    outputs: [...access.outputs.values()],
  };
}

export function findOutputByName(outputs: MIDIOutput[], namePart: string): MIDIOutput | undefined {
  const lower = namePart.toLowerCase();
  return outputs.find((o) => o.name?.toLowerCase().includes(lower));
}
