/**
 * Novation UltraNova SysEx helpers (USB MIDI).
 * Header matches captured dumps: F0 00 20 29 02 40 6F … F7
 * See Novation MIDI implementation PDF + community adaptations (KnobKraft-style).
 */

export const NOVATION_ULTRANOVA_HEADER = [0x00, 0x20, 0x29, 0x02, 0x40, 0x6f] as const;

export function wrapUltraNovaSysex(payload: number[]): Uint8Array {
  return Uint8Array.from([0xf0, ...NOVATION_ULTRANOVA_HEADER, ...payload, 0xf7]);
}

/** Request program data dump for bank/program (0-based bank, 0-based program in bank). */
export function buildProgramDumpRequest(bank: number, program: number): Uint8Array {
  return wrapUltraNovaSysex([0x41, 0x00, 0x00, 0x01, bank & 0x7f, program & 0x7f]);
}

/** Request edit-buffer (current panel) dump — common Novation layout. */
export function buildEditBufferDumpRequest(): Uint8Array {
  return wrapUltraNovaSysex([0x40, 0x00, 0x00, 0x01]);
}

export function isUltraNovaSysex(data: Uint8Array): boolean {
  if (data.length < 8 || data[0] !== 0xf0 || data[data.length - 1] !== 0xf7) return false;
  for (let i = 0; i < NOVATION_ULTRANOVA_HEADER.length; i++) {
    if (data[1 + i] !== NOVATION_ULTRANOVA_HEADER[i]) return false;
  }
  return true;
}

/** Best-effort patch name scan (16 printable ASCII chars) inside a program dump. */
export function guessPatchNameFromSysex(data: Uint8Array): string | null {
  if (!isUltraNovaSysex(data)) return null;
  const body = data.slice(7, data.length - 1);
  let best = "";
  for (let i = 0; i <= body.length - 4; i++) {
    let s = "";
    for (let j = 0; j < 16 && i + j < body.length; j++) {
      const b = body[i + j];
      if (b >= 0x20 && b <= 0x7e) s += String.fromCharCode(b);
      else break;
    }
    if (s.length >= 4 && s.length > best.length) best = s.trim();
  }
  return best || null;
}

export function sysexToHex(data: Uint8Array, maxBytes = 256): string {
  const slice = data.slice(0, maxBytes);
  const hex = [...slice].map((b) => b.toString(16).padStart(2, "0")).join(" ");
  return data.length > maxBytes ? `${hex} … (+${data.length - maxBytes} bytes)` : hex;
}

export function parseSyxFile(buffer: ArrayBuffer): Uint8Array[] {
  const all = new Uint8Array(buffer);
  const messages: Uint8Array[] = [];
  let start = -1;
  for (let i = 0; i < all.length; i++) {
    if (all[i] === 0xf0) start = i;
    if (all[i] === 0xf7 && start >= 0) {
      messages.push(all.slice(start, i + 1));
      start = -1;
    }
  }
  if (messages.length === 0 && all.length > 0) messages.push(all);
  return messages;
}
