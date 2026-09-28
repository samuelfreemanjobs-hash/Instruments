/** Drag 24-bit WAV files from the browser into FL Studio, Ableton, Logic, etc. */
import { downloadBuffer } from "./dsp-core.js";

export function makeWavFile(arrayBuffer, filename) {
  return new File([arrayBuffer], filename, { type: "audio/wav" });
}

export function wireDragWav(element, getBufferFn, getNameFn) {
  element.draggable = true;
  element.addEventListener("dragstart", (e) => {
    const buf = getBufferFn();
    if (!buf) {
      e.preventDefault();
      return;
    }
    const name = getNameFn();
    const file = makeWavFile(buf, name);
    e.dataTransfer.effectAllowed = "copy";
    e.dataTransfer.items.add(file);
    e.dataTransfer.setData("text/plain", name);
  });
}

export function refreshDndShelf(container, items) {
  container.innerHTML = "";
  items.forEach(({ id, label, getBuffer, filename }) => {
    const chip = document.createElement("button");
    chip.type = "button";
    chip.className = "dnd-chip";
    chip.textContent = label;
    chip.title = "Drag into your DAW";
    wireDragWav(chip, getBuffer, () => filename());
    chip.addEventListener("click", () => {
      const buf = getBuffer();
      if (buf) downloadBuffer(buf, filename());
    });
    container.appendChild(chip);
  });
}
