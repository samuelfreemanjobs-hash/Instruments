const state = {
  playing: false,
  motionRec: true,
  currentStep: 0,
  steps: Array.from({ length: 16 }, () => ({ on: false, accent: false })),
  selectedPart: 0,
  bpm: 92.4,
  swing: 62,
};

const downbeats = [0, 4, 8, 12];
let playInterval = null;

function $(sel) {
  return document.querySelector(sel);
}

function $all(sel) {
  return document.querySelectorAll(sel);
}

function initSteps() {
  const row = $("#step-row");
  row.innerHTML = "";
  for (let i = 0; i < 16; i++) {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className = "step-key" + (downbeats.includes(i) ? " downbeat" : "");
    btn.dataset.index = String(i);
    btn.innerHTML = `<span>${String(i + 1).padStart(2, "0")}</span><span class="accent-dot"></span>`;
    btn.addEventListener("click", (e) => {
      if (e.shiftKey) {
        state.steps[i].accent = !state.steps[i].accent;
      } else {
        state.steps[i].on = !state.steps[i].on;
      }
      renderSteps();
    });
    row.appendChild(btn);
  }
  renderSteps();
}

function renderSteps() {
  $all(".step-key").forEach((el, i) => {
    const s = state.steps[i];
    el.classList.toggle("active", s.on);
    el.classList.toggle("accent", s.accent);
    el.classList.toggle("playing", state.playing && state.currentStep === i);
  });
}

function drawMotionLanes() {
  const lanes = [
    { id: "lane-pitch", color: "#38bdf8", points: [20, 40, 35, 25, 50, 45, 65, 20, 80, 35, 95, 30] },
    { id: "lane-cutoff", color: "#0ea5e9", points: [20, 50, 40, 48, 55, 30, 70, 42, 85, 28, 95, 38] },
    { id: "lane-pan", color: "#f59e0b", points: [20, 35, 45, 40, 60, 38, 75, 42, 90, 36, 95, 40] },
  ];

  lanes.forEach((lane) => {
    const svg = document.querySelector(`#${lane.id} svg`);
    if (!svg) return;
    const w = svg.clientWidth || 400;
    const h = svg.clientHeight || 24;
    const pts = lane.points.map((v, idx) => {
      if (idx % 2 === 0) return (v / 100) * w;
      return (v / 100) * h;
    });
    let d = `M ${pts[0]} ${pts[1]}`;
    for (let i = 2; i < pts.length; i += 2) d += ` L ${pts[i]} ${pts[i + 1]}`;
    svg.innerHTML = `<path d="${d}" fill="none" stroke="${lane.color}" stroke-width="1.5" opacity="0.9"/>`;
  });
}

function animateVu() {
  const needles = $all(".vu-meter .needle");
  needles.forEach((n) => {
    const angle = state.playing ? -25 + Math.random() * 35 : -25;
    n.style.transform = `translateX(-50%) rotate(${angle}deg)`;
  });
  $all(".track-meter span").forEach((m) => {
    m.style.height = state.playing ? `${30 + Math.random() * 55}%` : "20%";
  });
}

function startTransport() {
  if (playInterval) return;
  state.playing = true;
  $("#btn-play").classList.add("active");
  const ms = (60 / state.bpm / 4) * 1000;
  playInterval = setInterval(() => {
    state.currentStep = (state.currentStep + 1) % 16;
    renderSteps();
    animateVu();
  }, ms);
  animateVu();
}

function stopTransport() {
  state.playing = false;
  if (playInterval) {
    clearInterval(playInterval);
    playInterval = null;
  }
  $("#btn-play").classList.remove("active");
  state.currentStep = 0;
  renderSteps();
  animateVu();
}

function initRibbon() {
  const ribbon = $("#touch-ribbon");
  const dot = $("#ribbon-dot");
  ribbon.addEventListener("pointermove", (e) => {
    const rect = ribbon.getBoundingClientRect();
    const x = ((e.clientX - rect.left) / rect.width) * 100;
    const y = ((e.clientY - rect.top) / rect.height) * 100;
    dot.style.left = `${x}%`;
    dot.style.top = `${y}%`;
  });
}

function initParts() {
  $all(".part-cell").forEach((cell, idx) => {
    cell.addEventListener("click", () => {
      $all(".part-cell").forEach((c) => c.classList.remove("selected"));
      cell.classList.add("selected");
      state.selectedPart = idx;
    });
    cell.querySelectorAll(".led").forEach((led) => {
      led.addEventListener("click", (e) => {
        e.stopPropagation();
        led.classList.toggle("on");
      });
    });
  });
}

function initTransport() {
  $("#btn-play").addEventListener("click", () => {
    if (state.playing) stopTransport();
    else startTransport();
  });
  $("#btn-stop").addEventListener("click", stopTransport);
  $("#btn-rec").addEventListener("click", () => {
    state.motionRec = !state.motionRec;
    $("#btn-rec").classList.toggle("rec-armed", state.motionRec);
    $("#motion-status").classList.toggle("motion-rec-armed", state.motionRec);
    $("#motion-status").textContent = state.motionRec ? "MOTION REC ●" : "MOTION REC ○";
  });
}

function initKnobs() {
  $all(".knob").forEach((knob) => {
    let val = 0.5;
    knob.addEventListener("pointerdown", (e) => {
      e.preventDefault();
      const startY = e.clientY;
      const move = (ev) => {
        val = Math.max(0, Math.min(1, val - (ev.clientY - startY) * 0.005));
        knob.style.background = `conic-gradient(from ${200 + val * 240}deg, var(--graphite), var(--obsidian-3))`;
      };
      window.addEventListener("pointermove", move);
      window.addEventListener(
        "pointerup",
        () => window.removeEventListener("pointermove", move),
        { once: true }
      );
    });
  });
}

window.addEventListener("resize", drawMotionLanes);
window.addEventListener("DOMContentLoaded", () => {
  initSteps();
  initParts();
  initRibbon();
  initTransport();
  initKnobs();
  drawMotionLanes();
  // Demo pattern
  [0, 4, 8, 12, 2, 6, 10, 14].forEach((i) => {
    state.steps[i].on = true;
  });
  state.steps[4].accent = true;
  state.steps[12].accent = true;
  renderSteps();
});
