const macroLabels = [
  "Blend", "Tone", "Shape", "Air", "Sub", "Dirt", "Motion", "Space",
];

function makeKnobSvg(index) {
  const arc = 220 * (index / 7);
  return `
    <svg viewBox="0 0 100 100" data-macro="${index + 1}">
      <defs>
        <linearGradient id="metalGrad" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0%" stop-color="#dfe5f4"/>
          <stop offset="45%" stop-color="#6d7b97"/>
          <stop offset="100%" stop-color="#303a4e"/>
        </linearGradient>
      </defs>
      <circle cx="50" cy="50" r="38" class="knob-body"/>
      <path class="knob-led" d="M50 50 L50 18" transform="rotate(${arc - 110} 50 50)"/>
      <circle cx="50" cy="50" r="6" fill="#0d1018"/>
    </svg>`;
}

function buildKnobs() {
  const row = document.getElementById("macro-knobs");
  macroLabels.forEach((label, i) => {
    const wrap = document.createElement("div");
    wrap.className = "knob";
    wrap.innerHTML = makeKnobSvg(i) + `<label>${label}</label>`;
    const svg = wrap.querySelector("svg");
    let value = 0.5;
    const setValue = (v) => {
      value = Math.min(1, Math.max(0, v));
      const rot = -135 + value * 270;
      svg.setAttribute("data-value", value.toFixed(3));
      const led = svg.querySelector(".knob-led");
      led.setAttribute("transform", `rotate(${rot - 110} 50 50)`);
    };
    setValue(0.5);
    svg.addEventListener("wheel", (e) => {
      e.preventDefault();
      setValue(value + (e.deltaY < 0 ? 0.03 : -0.03));
    });
    svg.addEventListener("dblclick", (e) => {
      if (e.ctrlKey || e.metaKey) setValue(0.5);
    });
    row.appendChild(wrap);
  });
}

function animateScope() {
  const canvas = document.getElementById("scope");
  const ctx = canvas.getContext("2d");
  let phase = 0;
  const draw = () => {
    const w = canvas.width;
    const h = canvas.height;
    ctx.fillStyle = "#090b10";
    ctx.fillRect(0, 0, w, h);
    ctx.strokeStyle = "#38e8ff";
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    for (let x = 0; x < w; x++) {
      const t = (x / w) * Math.PI * 8 + phase;
      const y = h * 0.5 + Math.sin(t) * (h * 0.35) * (0.3 + 0.7 * Math.sin(phase * 0.1));
      if (x === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
    phase += 0.2;
    requestAnimationFrame(draw);
  };
  draw();
}

document.getElementById("dice")?.addEventListener("click", () => {
  document.body.dispatchEvent(new CustomEvent("playsynth-dice"));
});
document.getElementById("midi-drag")?.addEventListener("click", () => {
  document.body.dispatchEvent(new CustomEvent("playsynth-midi-export"));
});

buildKnobs();
animateScope();