/** LED column next to vertical faders. */

const FADER_IDS = ["fxFilter", "fxBitcrush", "fxDrive", "fxCassette", "fxGain"];
const LED_SEGMENTS = 12;

function ensureLedSegments(meter) {
  if (meter.querySelector(".led-seg")) return;
  for (let i = 0; i < LED_SEGMENTS; i++) {
    const seg = document.createElement("span");
    seg.className = "led-seg";
    meter.appendChild(seg);
  }
}

export function bindFaderLeds() {
  FADER_IDS.forEach((id) => {
    const input = document.getElementById(id);
    const meter = document.querySelector(`[data-led-for="${id}"]`);
    if (!input || !meter) return;
    ensureLedSegments(meter);
    const update = () => {
      const v = Number(input.value);
      const segs = meter.querySelectorAll(".led-seg");
      const lit = Math.round(v * segs.length);
      segs.forEach((seg, i) => {
        seg.classList.toggle("on", i < lit);
      });
    };
    input.addEventListener("input", update);
    update();
  });
}
