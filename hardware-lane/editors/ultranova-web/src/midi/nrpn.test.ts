import { describe, expect, it } from "vitest";
import { buildNrpnMessages } from "./nrpn";

describe("buildNrpnMessages", () => {
  it("omits NRPN MSB when zero (UltraNova default)", () => {
    const msgs = buildNrpnMessages(1, 0, 73, 100);
    expect(msgs).toEqual([
      [0xb0, 98, 73],
      [0xb0, 6, 100],
    ]);
  });

  it("includes NRPN MSB when non-zero", () => {
    const msgs = buildNrpnMessages(2, 2, 64, 5, true);
    expect(msgs[0]).toEqual([0xb1, 99, 2]);
  });
});
