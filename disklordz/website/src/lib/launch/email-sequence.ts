import { isTripwireLive } from "@/lib/launch/campaign-day";

function urls() {
  const origin =
    process.env.LAUNCH_PUBLIC_BASE_URL ??
    (process.env.VERCEL_URL ? `https://${process.env.VERCEL_URL}` : "http://localhost:3000");
  return {
    download: process.env.LAUNCH_VOCAL_STEMS_URL ?? `${origin}/launch/vocals`,
    launch: `${origin}/launch`,
    tripwire:
      process.env.LAUNCH_TRIPWIRE_SAMPLE_URL ??
      process.env.LAUNCH_MEMPHIS_VAULT_URL ??
      `${origin}/launch/thank-you`,
    flagship: process.env.LAUNCH_FLAGSHIP_URL ?? `${origin}/launch/thank-you`,
  };
}

export type SequenceStep = 1 | 2 | 3 | 4;

export function emailForStep(step: SequenceStep): { subject: string; html: string } {
  const u = urls();
  const tripwireBlock = isTripwireLive()
    ? `<p><a href="${u.tripwire}">Get Memphis Vault Lite (tripwire sample)</a></p>`
    : `<p>Tripwire sample unlocks on Day 2 — watch your inbox.</p>`;

  switch (step) {
    case 1:
      return {
        subject: "[DOWNLOAD] Your 1994 Memphis vocal stems are inside",
        html: `<p>Your stem pack is ready.</p><p><a href="${u.download}">Download the ZIP</a></p><p>Tip: cut a narrow notch around 3.2 kHz so your snare cuts through tape and distortion.</p><p><a href="${u.launch}">Try Cassette Smash in the browser</a></p>`,
      };
    case 2:
      return {
        subject: "Day 2: Memphis phonk sample pack (lite)",
        html: `<p>Yesterday you grabbed the vocals. Today the knock matches.</p>${tripwireBlock}<p><a href="${u.launch}">Soundboard</a></p>`,
      };
    case 3:
      return {
        subject: "Why Memphis beats sounded violent (and your DAW doesn't)",
        html: `<p>Clean DAWs lie to phonk producers — 12-bit, tape, clipping on purpose.</p><p><a href="${u.launch}">Hear clean vs crushed</a></p>${tripwireBlock}`,
      };
    case 4:
      return {
        subject: "[Closing] Launch week pricing moves tonight",
        html: `<p>Last call on launch week.</p>${tripwireBlock}<p><a href="${u.flagship}">Memphis Tape FX early bird</a></p><p>Lost stems? <a href="${u.download}">Download again</a></p>`,
      };
    default:
      return { subject: "Disklordz", html: "<p>Disklordz</p>" };
  }
}

export const SEQUENCE_DELAYS_HOURS: Record<SequenceStep, number> = {
  1: 0,
  2: 24,
  3: 48,
  4: 72,
};
