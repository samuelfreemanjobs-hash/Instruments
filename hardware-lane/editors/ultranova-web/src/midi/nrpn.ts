/** Standard MIDI NRPN sequence (Novation UltraNova uses MSB=0 by default). */

export const CC_NRPN_MSB = 99;
export const CC_NRPN_LSB = 98;
export const CC_DATA_ENTRY_MSB = 6;
export const CC_DATA_ENTRY_LSB = 38;

export function buildNrpnMessages(
  channel: number,
  nrpnMsb: number,
  nrpnLsb: number,
  value: number,
  includeMsb = nrpnMsb !== 0,
): number[][] {
  const ch = (Math.max(1, channel) - 1) & 0x0f;
  const v = Math.max(0, Math.min(127, Math.round(value)));
  const msgs: number[][] = [];
  if (includeMsb) {
    msgs.push([0xb0 + ch, CC_NRPN_MSB, nrpnMsb & 0x7f]);
  }
  msgs.push([0xb0 + ch, CC_NRPN_LSB, nrpnLsb & 0x7f]);
  msgs.push([0xb0 + ch, CC_DATA_ENTRY_MSB, v]);
  return msgs;
}

export function sendNrpn(
  output: MIDIOutput,
  channel: number,
  nrpnMsb: number,
  nrpnLsb: number,
  value: number,
): void {
  for (const msg of buildNrpnMessages(channel, nrpnMsb, nrpnLsb, value)) {
    output.send(msg);
  }
}

export function sendCc(output: MIDIOutput, channel: number, cc: number, value: number): void {
  const ch = (Math.max(1, channel) - 1) & 0x0f;
  const v = Math.max(0, Math.min(127, Math.round(value)));
  output.send([0xb0 + ch, cc & 0x7f, v]);
}
