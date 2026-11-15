import { describe, expect, it } from "vitest";
import { retailCalendarEvents } from "./season.js";

describe("retailCalendarEvents", () => {
  it("returns ordered peak retail events for a year", () => {
    const events = retailCalendarEvents(2026);
    expect(events.length).toBeGreaterThanOrEqual(4);
    expect(events.some((e) => e.id === "black-friday")).toBe(true);
    expect(events.some((e) => e.peakEvent === "cyber_monday")).toBe(true);
  });
});
