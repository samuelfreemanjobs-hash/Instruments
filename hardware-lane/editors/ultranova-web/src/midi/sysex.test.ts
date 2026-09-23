import { describe, expect, it } from "vitest";
import { buildEditBufferDumpRequest, isUltraNovaSysex, wrapUltraNovaSysex } from "./sysex";

describe("sysex", () => {
  it("wraps Novation UltraNova header", () => {
    const msg = wrapUltraNovaSysex([0x40, 0x00]);
    expect(msg[0]).toBe(0xf0);
    expect([...msg.slice(1, 7)]).toEqual([0x00, 0x20, 0x29, 0x02, 0x40, 0x6f]);
    expect(msg[msg.length - 1]).toBe(0xf7);
  });

  it("detects ultranova sysex", () => {
    const msg = buildEditBufferDumpRequest();
    expect(isUltraNovaSysex(msg)).toBe(true);
  });
});
