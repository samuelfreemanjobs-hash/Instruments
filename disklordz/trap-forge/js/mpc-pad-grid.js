/**
 * Akai MPC-style 4×4 audition matrix: drum select, pitch shift, hat rolls.
 */

import { DRUM_ORDER } from "./trap-presets.js";

const PAD_LAYOUT = [
  { type: "drum", id: "kick", label: "KICK" },
  { type: "drum", id: "sub808", label: "808" },
  { type: "drum", id: "snare", label: "SNR" },
  { type: "drum", id: "clap", label: "CLAP" },
  { type: "drum", id: "closedhat", label: "CH" },
  { type: "drum", id: "openhat", label: "OH" },
  { type: "drum", id: "perc", label: "PERC" },
  { type: "pitch", semi: 7, label: "+7st" },
  { type: "pitch", semi: 12, label: "+12" },
  { type: "roll", div: "16", label: "1/16" },
  { type: "roll", div: "32", label: "1/32" },
  { type: "roll", div: "64", label: "1/64" },
  { type: "velocity", v: 1, label: "HARD" },
  { type: "velocity", v: 0.72, label: "MED" },
  { type: "velocity", v: 0.42, label: "SOFT" },
  { type: "drum", id: "kick", label: "KICK2" },
];

export function mountMpcPadGrid(container, handlers) {
  if (!container) return;
  container.innerHTML = "";
  container.className = "grid grid-cols-4 gap-2 max-w-[14rem]";
  PAD_LAYOUT.forEach((pad, idx) => {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className =
      "aspect-square rounded-md bg-slate-900 border border-slate-600 hover:border-cyanAccent hover:bg-brand-800 text-[10px] font-bold tracking-tight";
    btn.textContent = pad.label;
    btn.title = `Pad ${idx + 1}`;
    btn.addEventListener("click", () => {
      if (pad.type === "drum") handlers.selectDrum(pad.id);
      else if (pad.type === "pitch") handlers.auditionPitchSemi(pad.semi);
      else if (pad.type === "roll") handlers.auditionHatRoll(pad.div);
      else if (pad.type === "velocity") handlers.auditionVelocity(pad.v);
    });
    container.appendChild(btn);
  });
}

export { DRUM_ORDER };
