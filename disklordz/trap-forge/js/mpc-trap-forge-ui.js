/**
 * MPC TRAP-FORGE // Elite Drum Generator — procedural OfflineAudioContext engine.
 * Standalone UI logic (paired with index.html shell).
 */
(function () {
  "use strict";

  const PRESET_LIBRARY = [
    {
      name: "SPINZ_808_DIRTY",
      cat: "808",
      desc: "Classic Atlanta Spinz 808 with clipping",
      amp: { attack: 0.002, decay: 0.45, sustain: 0.5, release: 0.95 },
      filter: { type: "lowpass", cutoff: 3200, reso: 3.5, env: 0.35, decay: 0.3 },
      transient: { attack: 7.0, sustain: 1.0, pitchStart: 180, pitchDecay: 0.045, baseFreq: 46 },
      tx: { drive: 58, type: "hard", subHarm: 35, noise: 5, ceiling: -0.1 },
    },
    {
      name: "SOUTHSIDE_SNARE",
      cat: "SNARE",
      desc: "Sharp high crack layered snare",
      amp: { attack: 0.001, decay: 0.18, sustain: 0.05, release: 0.22 },
      filter: { type: "bandpass", cutoff: 4500, reso: 5.5, env: 0.6, decay: 0.15 },
      transient: { attack: 9.0, sustain: -2.0, pitchStart: 320, pitchDecay: 0.02, baseFreq: 185 },
      tx: { drive: 30, type: "tape", subHarm: 10, noise: 75, ceiling: -0.2 },
    },
    {
      name: "METRO_THUMP_KICK",
      cat: "KICK",
      desc: "Deep 50Hz sub punch with click transient",
      amp: { attack: 0.001, decay: 0.28, sustain: 0.0, release: 0.3 },
      filter: { type: "lowpass", cutoff: 6500, reso: 2.0, env: 0.8, decay: 0.08 },
      transient: { attack: 11.0, sustain: -1.0, pitchStart: 280, pitchDecay: 0.035, baseFreq: 52 },
      tx: { drive: 40, type: "tube", subHarm: 20, noise: 12, ceiling: -0.1 },
    },
    {
      name: "DRILL_GLIDE_SUB",
      cat: "808",
      desc: "Sustained UK/NY drill glide bass",
      amp: { attack: 0.003, decay: 0.8, sustain: 0.85, release: 1.2 },
      filter: { type: "lowpass", cutoff: 2200, reso: 4.8, env: 0.2, decay: 0.4 },
        transient: { attack: 4.0, sustain: 2.0, pitchStart: 120, pitchDecay: 0.06, baseFreq: 41, glideMs: 120, glideSemi: 7 },
      tx: { drive: 68, type: "tube", subHarm: 40, noise: 2, ceiling: -0.1 },
    },
    {
      name: "PIERRE_BOUNCY_PERC",
      cat: "PERC",
      desc: "Resonant playful trap synth ping",
      amp: { attack: 0.001, decay: 0.15, sustain: 0.1, release: 0.25 },
      filter: { type: "bandpass", cutoff: 3600, reso: 9.0, env: 0.7, decay: 0.12 },
      transient: { attack: 5.0, sustain: 0.0, pitchStart: 450, pitchDecay: 0.05, baseFreq: 260 },
      tx: { drive: 22, type: "tape", subHarm: 15, noise: 18, ceiling: -0.3 },
    },
    {
      name: "CRISP_SIZZLE_HAT",
      cat: "HIHAT",
      desc: "Micro-tuned metallic closed hat",
      amp: { attack: 0.001, decay: 0.08, sustain: 0.0, release: 0.1 },
      filter: { type: "highpass", cutoff: 7800, reso: 6.0, env: 0.3, decay: 0.05 },
      transient: { attack: 8.0, sustain: -6.0, pitchStart: 500, pitchDecay: 0.01, baseFreq: 380 },
      tx: { drive: 25, type: "tape", subHarm: 0, noise: 90, ceiling: -0.2 },
    },
  ];

  let currentSound = JSON.parse(JSON.stringify(PRESET_LIBRARY[0]));
  let liveAudioCtx = null;
  let padMode = "16levels";
  let lastPlayedFreq = null;
  let isSequencerPlaying = false;
  let seqTimerId = null;
  let seqCurrentStep = 0;

  const kitLibrary = [
    { name: "KICK_DEEP", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[2])) },
    { name: "KICK_PUNCH", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[2])) },
    { name: "SNARE_SOUTHSIDE", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[1])) },
    { name: "SNARE_CRACK", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[1])) },
    { name: "CLAP_LAYER", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[1])) },
    { name: "RIMSHOT_TIGHT", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[4])) },
    { name: "HAT_CLOSED", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[5])) },
    { name: "HAT_TICK", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[5])) },
    { name: "HAT_OPEN", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[5])) },
    { name: "CYMBAL_CRASH", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[5])) },
    { name: "808_SPINZ", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[0])) },
    { name: "808_DRILL", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[3])) },
    { name: "PERC_BOUNCE", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[4])) },
    { name: "PERC_WOOD", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[4])) },
    { name: "VOX_CHANT", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[4])) },
    { name: "FX_TRANSITION", ...JSON.parse(JSON.stringify(PRESET_LIBRARY[4])) },
  ];

  const MPC_FORGE_SYSTEM = `You are a legendary Trap Drum DSP Engineer for Akai MPC hardware.
Convert the user's natural language drum aesthetic into precise procedural DSP parameters in JSON.
The output JSON schema MUST match:
{
  "name": "SHORT_UPPERCASE_NAME",
  "cat": "808" | "KICK" | "SNARE" | "HIHAT" | "PERC",
  "desc": "Short description",
  "amp": { "attack": number (0.001-0.2), "decay": number (0.05-1.5), "sustain": number (0.0-1.0), "release": number (0.05-2.5) },
  "filter": { "type": "lowpass"|"highpass"|"bandpass"|"notch", "cutoff": number (60-16000), "reso": number (0.5-16.0), "env": number (-1.0 to 1.0), "decay": number (0.02-1.0) },
  "transient": { "attack": number (-6 to 14), "sustain": number (-12 to 4), "pitchStart": number (40-500), "pitchDecay": number (0.01-0.25), "baseFreq": number (32-400) },
  "tx": { "drive": number (0-100), "type": "hard"|"tape"|"tube"|"fold", "subHarm": number (0-80), "noise": number (0-95), "ceiling": number (-3.0 to 0.0) }
}`;

  function showToast(title, message, type = "info") {
    const container = document.getElementById("toastContainer");
    if (!container) return;
    const toast = document.createElement("div");
    const colors = {
      info: "border-mpc-cyan text-mpc-cyan bg-mpc-panel/95",
      success: "border-mpc-neon text-mpc-neon bg-mpc-panel/95",
      error: "border-mpc-accent text-mpc-accent bg-mpc-panel/95",
    };
    toast.className = `border rounded-xl p-3 shadow-2xl backdrop-blur-md pointer-events-auto transform transition-all duration-300 max-w-sm ${colors[type] || colors.info}`;
    toast.innerHTML = `<div class="text-xs font-mono font-bold tracking-wide uppercase">${title}</div><div class="text-xs font-sans text-slate-300 mt-0.5">${message}</div>`;
    container.appendChild(toast);
    setTimeout(() => {
      toast.style.opacity = "0";
      toast.style.transform = "translateY(10px)";
      setTimeout(() => toast.remove(), 300);
    }, 3500);
  }

  function mergeForgedSound(parsedSound) {
    currentSound = {
      name: parsedSound.name || "CUSTOM_FORGE",
      cat: parsedSound.cat || currentSound.cat || "808",
      desc: parsedSound.desc || currentSound.desc,
      amp: { ...currentSound.amp, ...(parsedSound.amp || {}) },
      filter: { ...currentSound.filter, ...(parsedSound.filter || {}) },
      transient: { ...currentSound.transient, ...(parsedSound.transient || {}) },
      tx: { ...currentSound.tx, ...(parsedSound.tx || {}) },
    };
    syncUiFromState();
    playCurrentSound(0);
  }

  function getAudioContext() {
    if (!liveAudioCtx) {
      const AudioContextClass = window.AudioContext || window.webkitAudioContext;
      liveAudioCtx = new AudioContextClass();
    }
    if (liveAudioCtx.state === "suspended") liveAudioCtx.resume();
    return liveAudioCtx;
  }

  function drawWaveform(buffer) {
    const canvas = document.getElementById("waveCanvas");
    if (!canvas || !buffer) return;
    const ctx = canvas.getContext("2d");
    const w = canvas.width;
    const h = canvas.height;
    const data = buffer.getChannelData(0);
    ctx.fillStyle = "#07080a";
    ctx.fillRect(0, 0, w, h);
    ctx.strokeStyle = "#ff2e43";
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    const step = Math.ceil(data.length / w);
    for (let x = 0; x < w; x++) {
      const i = x * step;
      const y = (1 - data[i] * 0.85) * 0.5 * h;
      if (x === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
  }

  function applyDacEmulation(renderedBuffer, dacType = "clean") {
    if (dacType === "clean") return;
    const data = renderedBuffer.getChannelData(0);
    const len = data.length;
    if (dacType === "mpc60") {
      const steps = 4096;
      const downsampleFactor = 44100 / 26040;
      let lastSample = 0;
      for (let i = 0; i < len; i++) {
        if (i % Math.round(downsampleFactor) === 0) {
          lastSample = Math.round(data[i] * (steps / 2)) / (steps / 2);
        }
        data[i] = lastSample * 1.05;
      }
    } else if (dacType === "mpc3000") {
      const steps = 65536;
      for (let i = 0; i < len; i++) {
        const quant = Math.round(data[i] * (steps / 2)) / (steps / 2);
        data[i] = Math.tanh(quant * 1.15) * 0.95;
      }
    } else if (dacType === "sp1200") {
      const steps = 4096;
      const downsampleFactor = 44100 / 26040;
      let lastSample = 0;
      for (let i = 0; i < len; i++) {
        if (i % Math.round(downsampleFactor) === 0) {
          lastSample = Math.round(data[i] * (steps / 2)) / (steps / 2);
        }
        data[i] = lastSample + 0.02 * Math.sin(i * 0.4);
      }
    }
  }

  async function synthesizeDrumBuffer(soundParams, semitoneOffset = 0, glideFromFreq = null) {
    const sampleRate = 44100;
    const totalDecay = soundParams.amp.attack + soundParams.amp.decay + soundParams.amp.release;
    const baseDuration =
      soundParams.cat === "808"
        ? Math.max(1.8, totalDecay + 0.6)
        : soundParams.cat === "HIHAT"
          ? Math.min(0.4, totalDecay + 0.05)
          : Math.max(0.4, totalDecay + 0.2);

    const length = Math.floor(sampleRate * baseDuration);
    const offlineCtx = new OfflineAudioContext(1, length, sampleRate);
    const freqMultiplier = Math.pow(2, semitoneOffset / 12);
    const rootBaseFreq = soundParams.transient.baseFreq * freqMultiplier;
    const startPitchFreq = soundParams.transient.pitchStart * freqMultiplier;
    const now = 0;

    const osc = offlineCtx.createOscillator();
    osc.type = soundParams.cat === "HIHAT" ? "square" : soundParams.cat === "PERC" ? "triangle" : "sine";

    if (glideFromFreq && document.getElementById("glideToggle")?.checked) {
      osc.frequency.setValueAtTime(glideFromFreq, now);
      osc.frequency.exponentialRampToValueAtTime(Math.max(20, rootBaseFreq), now + 0.08);
    } else if (
      soundParams.cat === "808" &&
      (soundParams.transient.glideMs ?? 0) > 0 &&
      (soundParams.transient.glideSemi ?? 0) !== 0
    ) {
      const glideT = Math.max(0.01, (soundParams.transient.glideMs ?? 120) / 1000);
      const target =
        rootBaseFreq * Math.pow(2, (soundParams.transient.glideSemi ?? 7) / 12);
      osc.frequency.setValueAtTime(Math.max(20, rootBaseFreq), now);
      osc.frequency.exponentialRampToValueAtTime(Math.max(20, target), now + glideT);
    } else {
      osc.frequency.setValueAtTime(Math.max(20, startPitchFreq), now);
      osc.frequency.exponentialRampToValueAtTime(Math.max(20, rootBaseFreq), now + soundParams.transient.pitchDecay);
    }

    let subOsc = null;
    let subGain = null;
    if (soundParams.tx.subHarm > 0) {
      subOsc = offlineCtx.createOscillator();
      subOsc.type = "triangle";
      subOsc.frequency.setValueAtTime(startPitchFreq * 0.5, now);
      subOsc.frequency.exponentialRampToValueAtTime(Math.max(20, rootBaseFreq * 0.5), now + soundParams.transient.pitchDecay * 1.2);
      subGain = offlineCtx.createGain();
      subGain.gain.setValueAtTime((soundParams.tx.subHarm / 100) * 0.5, now);
      subOsc.connect(subGain);
    }

    let noiseNode = null;
    let noiseGain = null;
    if (soundParams.tx.noise > 0 || soundParams.cat === "SNARE" || soundParams.cat === "HIHAT") {
      const noiseBuffer = offlineCtx.createBuffer(1, length, sampleRate);
      const noiseData = noiseBuffer.getChannelData(0);
      let b0 = 0,
        b1 = 0,
        b2 = 0;
      for (let i = 0; i < length; i++) {
        const white = Math.random() * 2 - 1;
        b0 = 0.99886 * b0 + white * 0.0555179;
        b1 = 0.99332 * b1 + white * 0.0750759;
        b2 = 0.969 * b2 + white * 0.153852;
        const pink = b0 + b1 + b2 + white * 0.5362;
        noiseData[i] = (white * 0.6 + pink * 0.4) * 0.7;
      }
      noiseNode = offlineCtx.createBufferSource();
      noiseNode.buffer = noiseBuffer;
      noiseGain = offlineCtx.createGain();
      const noiseDecay =
        soundParams.cat === "HIHAT"
          ? soundParams.amp.decay * 0.8
          : soundParams.cat === "SNARE"
            ? soundParams.amp.decay * 1.3
            : 0.04;
      noiseGain.gain.setValueAtTime((soundParams.tx.noise / 100) * 0.85, now);
      noiseGain.gain.exponentialRampToValueAtTime(0.0001, now + noiseDecay);
      noiseNode.connect(noiseGain);
    }

    const filter = offlineCtx.createBiquadFilter();
    filter.type = soundParams.filter.type || "lowpass";
    const baseCutoff = soundParams.filter.cutoff;
    filter.Q.value = soundParams.filter.reso;
    filter.frequency.setValueAtTime(Math.max(20, baseCutoff), now);
    if (soundParams.filter.env !== 0) {
      const peakCutoff = Math.min(20000, Math.max(30, baseCutoff * (1 + soundParams.filter.env * 3)));
      filter.frequency.setValueAtTime(peakCutoff, now);
      filter.frequency.exponentialRampToValueAtTime(Math.max(20, baseCutoff), now + soundParams.filter.decay);
    }

    const ampGain = offlineCtx.createGain();
    const a = Math.max(0.001, soundParams.amp.attack);
    const d = Math.max(0.01, soundParams.amp.decay);
    const s = soundParams.amp.sustain;
    const r = Math.max(0.01, soundParams.amp.release);
    ampGain.gain.setValueAtTime(0.0001, now);
    ampGain.gain.linearRampToValueAtTime(1.0, now + a);
    ampGain.gain.exponentialRampToValueAtTime(Math.max(0.0001, s), now + a + d);
    ampGain.gain.exponentialRampToValueAtTime(0.00001, now + a + d + r);

    const waveShaper = offlineCtx.createWaveShaper();
    const drive = soundParams.tx.drive / 100;
    waveShaper.curve = makeDistortionCurve(drive, soundParams.tx.type);
    waveShaper.oversample = "4x";

    const mixer = offlineCtx.createGain();
    osc.connect(mixer);
    if (subGain) subGain.connect(mixer);
    if (noiseGain) noiseGain.connect(mixer);
    mixer.connect(filter);
    filter.connect(ampGain);
    ampGain.connect(waveShaper);

    const masterGain = offlineCtx.createGain();
    masterGain.gain.value = Math.pow(10, soundParams.tx.ceiling / 20);
    waveShaper.connect(masterGain);
    masterGain.connect(offlineCtx.destination);

    osc.start(now);
    if (subOsc) subOsc.start(now);
    if (noiseNode) noiseNode.start(now);

    const renderedBuffer = await offlineCtx.startRendering();
    applyDirectTransientPunch(renderedBuffer, soundParams.transient.attack, soundParams.transient.sustain);
    const selectedDac = document.getElementById("dacEmulationSelect")?.value || "clean";
    applyDacEmulation(renderedBuffer, selectedDac);
    return renderedBuffer;
  }

  function makeDistortionCurve(amount, type = "hard") {
    const n_samples = 44100;
    const curve = new Float32Array(n_samples);
    const deg = Math.PI / 180;
    const k = amount * 40;
    for (let i = 0; i < n_samples; ++i) {
      const x = (i * 2) / n_samples - 1;
      if (type === "hard") {
        const threshold = 1 - amount * 0.65;
        curve[i] = Math.max(-threshold, Math.min(threshold, x * (1 + amount * 3))) / threshold;
      } else if (type === "tube") {
        const driven = x * (1 + amount * 2.5);
        curve[i] = Math.tanh(driven) + 0.15 * Math.sin(driven * Math.PI);
      } else if (type === "fold") {
        curve[i] = Math.sin(x * (1 + amount * 4));
      } else {
        curve[i] = k === 0 ? x : ((3 + k) * x * 20 * deg) / (Math.PI + k * Math.abs(x));
      }
    }
    return curve;
  }

  function applyDirectTransientPunch(buffer, attackDb, sustainDb) {
    const data = buffer.getChannelData(0);
    const attackGain = Math.pow(10, attackDb / 20);
    const sustainGain = Math.pow(10, sustainDb / 20);
    const punchSamples = Math.floor(buffer.sampleRate * 0.015);
    for (let i = 0; i < data.length; i++) {
      if (i < punchSamples) {
        const t = i / punchSamples;
        data[i] = Math.max(-1, Math.min(1, data[i] * (attackGain * (1 - t) + sustainGain * t)));
      } else {
        data[i] = Math.max(-1, Math.min(1, data[i] * sustainGain));
      }
    }
  }

  async function playCurrentSound(semitoneOffset = 0) {
    const audioCtx = getAudioContext();
    const freqMultiplier = Math.pow(2, semitoneOffset / 12);
    const targetFreq = currentSound.transient.baseFreq * freqMultiplier;
    const previewBtn = document.getElementById("masterPreviewBtn");
    if (previewBtn) {
      previewBtn.classList.add("preview-trigger-active");
      setTimeout(() => previewBtn.classList.remove("preview-trigger-active"), 150);
    }
    const buffer = await synthesizeDrumBuffer(currentSound, semitoneOffset, lastPlayedFreq);
    lastPlayedFreq = targetFreq;
    drawWaveform(buffer);
    const source = audioCtx.createBufferSource();
    source.buffer = buffer;
    source.connect(audioCtx.destination);
    source.start();
  }

  function syncUiFromState() {
    document.getElementById("currentSoundName").textContent = currentSound.name;
    document.getElementById("currentSoundDesc").textContent = currentSound.desc || "Procedural Trap Drum DSP Engine";
    document.getElementById("currentCategoryTag").textContent = currentSound.cat;
    document.getElementById("ampAttack").value = currentSound.amp.attack;
    document.getElementById("ampAttackVal").textContent = `${Math.round(currentSound.amp.attack * 1000)} ms`;
    document.getElementById("ampDecay").value = currentSound.amp.decay;
    document.getElementById("ampDecayVal").textContent = `${Math.round(currentSound.amp.decay * 1000)} ms`;
    document.getElementById("ampSustain").value = currentSound.amp.sustain;
    document.getElementById("ampSustainVal").textContent = `${Math.round(currentSound.amp.sustain * 100)} %`;
    document.getElementById("ampRelease").value = currentSound.amp.release;
    document.getElementById("ampReleaseVal").textContent = `${Math.round(currentSound.amp.release * 1000)} ms`;
    document.getElementById("filterType").value = currentSound.filter.type;
    document.getElementById("filterCutoff").value = currentSound.filter.cutoff;
    document.getElementById("filterCutoffVal").textContent = `${currentSound.filter.cutoff.toLocaleString()} Hz`;
    document.getElementById("filterReso").value = currentSound.filter.reso;
    document.getElementById("filterResoVal").textContent = currentSound.filter.reso.toFixed(1);
    document.getElementById("filterEnv").value = currentSound.filter.env;
    document.getElementById("filterEnvVal").textContent = `${currentSound.filter.env > 0 ? "+" : ""}${Math.round(currentSound.filter.env * 100)}%`;
    document.getElementById("filterDecay").value = currentSound.filter.decay;
    document.getElementById("filterDecayVal").textContent = `${Math.round(currentSound.filter.decay * 1000)} ms`;
    document.getElementById("transientAttack").value = currentSound.transient.attack;
    document.getElementById("transientAttackVal").textContent = `${currentSound.transient.attack > 0 ? "+" : ""}${currentSound.transient.attack.toFixed(1)} dB`;
    document.getElementById("transientSustain").value = currentSound.transient.sustain;
    document.getElementById("transientSustainVal").textContent = `${currentSound.transient.sustain > 0 ? "+" : ""}${currentSound.transient.sustain.toFixed(1)} dB`;
    document.getElementById("pitchStart").value = currentSound.transient.pitchStart;
    document.getElementById("pitchStartVal").textContent = `${currentSound.transient.pitchStart} Hz`;
    document.getElementById("pitchDecay").value = currentSound.transient.pitchDecay;
    document.getElementById("pitchDecayVal").textContent = `${Math.round(currentSound.transient.pitchDecay * 1000)} ms`;
    document.getElementById("pitchBaseVal").textContent = `${currentSound.transient.baseFreq} Hz`;
    document.getElementById("glideMs").value = currentSound.transient.glideMs ?? 0;
    document.getElementById("glideMsVal").textContent = `${currentSound.transient.glideMs ?? 0} ms`;
    document.getElementById("glideSemi").value = currentSound.transient.glideSemi ?? 0;
    document.getElementById("glideSemiVal").textContent = `+${currentSound.transient.glideSemi ?? 0} st`;
    document.getElementById("distType").value = currentSound.tx.type;
    document.getElementById("driveAmount").value = currentSound.tx.drive;
    document.getElementById("driveAmountVal").textContent = `${currentSound.tx.drive} %`;
    document.getElementById("subHarmonics").value = currentSound.tx.subHarm;
    document.getElementById("subHarmonicsVal").textContent = `${currentSound.tx.subHarm} %`;
    document.getElementById("noiseAmount").value = currentSound.tx.noise;
    document.getElementById("noiseAmountVal").textContent = `${currentSound.tx.noise} %`;
    document.getElementById("outputCeiling").value = currentSound.tx.ceiling;
    document.getElementById("outputCeilingVal").textContent = `${currentSound.tx.ceiling.toFixed(1)} dB`;
  }

  function bindSlider(id, targetObj, key, unit = "", factor = 1) {
    const slider = document.getElementById(id);
    const display = document.getElementById(`${id}Val`);
    slider.addEventListener("input", (e) => {
      const val = parseFloat(e.target.value);
      targetObj[key] = val;
      if (unit === "ms") display.textContent = `${Math.round(val * 1000)} ms`;
      else if (unit === "secms") display.textContent = `${Math.round(val)} ms`;
      else if (unit === "semi") display.textContent = `+${Math.round(val)} st`;
      else if (unit === "%") display.textContent = `${Math.round(val * factor)} %`;
      else if (unit === "dB") display.textContent = `${val > 0 ? "+" : ""}${val.toFixed(1)} dB`;
      else if (unit === "Hz") display.textContent = `${val.toLocaleString()} Hz`;
      else display.textContent = val.toFixed(1);
    });
    slider.addEventListener("change", () => playCurrentSound(0));
  }

  async function callGeminiAiForge(promptText) {
    const aiBtnSpinner = document.getElementById("aiBtnSpinner");
    const generateAiBtn = document.getElementById("generateAiBtn");
    aiBtnSpinner.classList.remove("hidden");
    generateAiBtn.disabled = true;
    try {
    const modelSelect = document.getElementById("geminiModelSelect");
    const model = modelSelect?.value || localStorage.getItem("trapforge_gemini_model") || "gemini-2.0-flash";

    try {
      const proxyRes = await fetch("/api/ai/vibe", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ prompt: promptText, drumId: currentSound.cat, model, schema: "mpc_elite" }),
      });
      if (proxyRes.ok) {
        const data = await proxyRes.json();
        if (data.sound) {
          mergeForgedSound(data.sound);
          showToast("AI FORGE COMPLETE", `Sculpted "${currentSound.name}" from your prompt!`, "success");
          return;
        }
      }
    } catch (_) {
      /* fall through */
    }

    const keyInput = document.getElementById("geminiApiKey");
    const apiKey = keyInput?.value?.trim() || localStorage.getItem("trapforge_gemini_key") || "";
    if (apiKey) {
      localStorage.setItem("trapforge_gemini_key", apiKey);
      const apiUrl = `https://generativelanguage.googleapis.com/v1beta/models/${model}:generateContent?key=${encodeURIComponent(apiKey)}`;
      const payload = {
        contents: [{ parts: [{ text: `Design an elite trap drum for this prompt: "${promptText}"` }] }],
        systemInstruction: { parts: [{ text: MPC_FORGE_SYSTEM }] },
        generationConfig: { responseMimeType: "application/json" },
      };
      try {
        const response = await fetch(apiUrl, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload),
        });
        if (response.ok) {
          const data = await response.json();
          const jsonText = data.candidates?.[0]?.content?.parts?.[0]?.text;
          if (jsonText) {
            mergeForgedSound(JSON.parse(jsonText));
            showToast("AI FORGE COMPLETE", `Sculpted "${currentSound.name}" from your prompt!`, "success");
            return;
          }
        }
      } catch (_) {
        /* fall through */
      }
    }

    mutateProceduralRandom();
    showToast("MUTATION FORGED", `Generated algorithmic variant for "${promptText}"`, "info");
    } finally {
      aiBtnSpinner.classList.add("hidden");
      generateAiBtn.disabled = false;
    }
  }

  function mutateProceduralRandom() {
    currentSound.name = `MUTANT_${Math.floor(Math.random() * 900 + 100)}`;
    currentSound.transient.attack = parseFloat((Math.random() * 12).toFixed(1));
    currentSound.transient.pitchStart = Math.round(120 + Math.random() * 320);
    currentSound.transient.pitchDecay = parseFloat((0.02 + Math.random() * 0.08).toFixed(3));
    currentSound.tx.drive = Math.round(20 + Math.random() * 65);
    currentSound.amp.decay = parseFloat((0.15 + Math.random() * 0.7).toFixed(2));
    syncUiFromState();
    playCurrentSound(0);
  }

  const seqTracks = [
    { name: "808 SUB", presetIdx: 0, steps: [1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0] },
    { name: "KICK", presetIdx: 2, steps: [1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0] },
    { name: "SNARE", presetIdx: 1, steps: [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0] },
    { name: "HI-HAT", presetIdx: 5, steps: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1] },
  ];

  function renderSequencerTracks() {
    const container = document.getElementById("seqTracksContainer");
    container.innerHTML = "";
    seqTracks.forEach((track, tIdx) => {
      const row = document.createElement("div");
      row.className = "flex items-center gap-2";
      const label = document.createElement("div");
      label.className = "w-20 text-[11px] font-tech font-bold text-slate-300 truncate";
      label.textContent = track.name;
      row.appendChild(label);
      const stepsRow = document.createElement("div");
      stepsRow.className = "flex items-center gap-1.5 flex-1";
      for (let s = 0; s < 16; s++) {
        const stepBtn = document.createElement("button");
        const isQuarter = s % 4 === 0;
        const isActive = track.steps[s] === 1;
        stepBtn.className = `seq-step w-6 h-7 rounded text-[10px] font-mono transition-all ${
          isActive
            ? "bg-mpc-orange text-white shadow-md"
            : isQuarter
              ? "bg-mpc-card border border-mpc-border/90 text-slate-500"
              : "bg-mpc-black border border-mpc-border/40 text-slate-600"
        }`;
        stepBtn.setAttribute("data-track", tIdx);
        stepBtn.setAttribute("data-step", s);
        stepBtn.textContent = s + 1;
        stepBtn.addEventListener("click", () => {
          track.steps[s] = track.steps[s] === 1 ? 0 : 1;
          renderSequencerTracks();
        });
        stepsRow.appendChild(stepBtn);
      }
      row.appendChild(stepsRow);
      container.appendChild(row);
    });
  }

  function stepSequencerTick() {
    const bpm = parseInt(document.getElementById("seqBpm").value, 10);
    const swing = parseFloat(document.getElementById("seqSwing").value);
    const base16thMs = 60000 / bpm / 4;
    const isOdd = seqCurrentStep % 2 !== 0;
    const currentDelay = isOdd ? base16thMs * (2 * swing) : base16thMs * (2 * (1 - swing));
    seqTracks.forEach((track) => {
      if (track.steps[seqCurrentStep] === 1) {
        const preset = PRESET_LIBRARY[track.presetIdx];
        synthesizeDrumBuffer(preset, 0).then((buf) => {
          const ctx = getAudioContext();
          const src = ctx.createBufferSource();
          src.buffer = buf;
          src.connect(ctx.destination);
          src.start();
        });
      }
    });
    document.querySelectorAll(".seq-step").forEach((btn) => {
      const s = parseInt(btn.getAttribute("data-step"), 10);
      btn.classList.toggle("ring-2", s === seqCurrentStep);
      btn.classList.toggle("ring-mpc-cyan", s === seqCurrentStep);
    });
    seqCurrentStep = (seqCurrentStep + 1) % 16;
    seqTimerId = setTimeout(stepSequencerTick, Math.max(10, currentDelay));
  }

  function audioBufferToWav(buffer, bitDepth = "wav24") {
    const sampleRate = buffer.sampleRate;
    const data = buffer.getChannelData(0);
    const numSamples = data.length;
    let bytesPerSample = 3;
    if (bitDepth === "wav16") bytesPerSample = 2;
    if (bitDepth === "wav32") bytesPerSample = 4;
    const blockAlign = bytesPerSample;
    const byteRate = sampleRate * blockAlign;
    const dataSize = numSamples * blockAlign;
    const arrayBuffer = new ArrayBuffer(44 + dataSize);
    const view = new DataView(arrayBuffer);
    const writeString = (offset, string) => {
      for (let i = 0; i < string.length; i++) view.setUint8(offset + i, string.charCodeAt(i));
    };
    writeString(0, "RIFF");
    view.setUint32(4, 36 + dataSize, true);
    writeString(8, "WAVE");
    writeString(12, "fmt ");
    view.setUint32(16, 16, true);
    view.setUint16(20, bitDepth === "wav32" ? 3 : 1, true);
    view.setUint16(22, 1, true);
    view.setUint32(24, sampleRate, true);
    view.setUint32(28, byteRate, true);
    view.setUint16(32, blockAlign, true);
    view.setUint16(34, bytesPerSample * 8, true);
    writeString(36, "data");
    view.setUint32(40, dataSize, true);
    let offset = 44;
    if (bitDepth === "wav16") {
      for (let i = 0; i < numSamples; i++, offset += 2) {
        const s = Math.max(-1, Math.min(1, data[i]));
        view.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7fff, true);
      }
    } else if (bitDepth === "wav24") {
      for (let i = 0; i < numSamples; i++, offset += 3) {
        const s = Math.max(-1, Math.min(1, data[i]));
        const val = Math.floor(s < 0 ? s * 0x800000 : s * 0x7fffff);
        view.setUint8(offset, val & 0xff);
        view.setUint8(offset + 1, (val >> 8) & 0xff);
        view.setUint8(offset + 2, (val >> 16) & 0xff);
      }
    } else {
      for (let i = 0; i < numSamples; i++, offset += 4) view.setFloat32(offset, data[i], true);
    }
    return new Blob([view], { type: "audio/wav" });
  }

  function generateMpcProgramXpm(kitSounds) {
    let padsXml = "";
    kitSounds.forEach((snd, index) => {
      padsXml += `
    <Pad number="${index + 1}">
      <Name>${snd.name}</Name>
      <SampleName>MPC_${snd.cat}_${snd.name}.wav</SampleName>
      <TuneFine>0</TuneFine>
      <TuneCoarse>0</TuneCoarse>
      <FilterCutoff>${snd.filter.cutoff}</FilterCutoff>
      <FilterResonance>${Math.round(snd.filter.reso * 10)}</FilterResonance>
      <AmpAttack>${snd.amp.attack}</AmpAttack>
      <AmpDecay>${snd.amp.decay}</AmpDecay>
    </Pad>`;
    });
    return `<?xml version="1.0" encoding="UTF-8"?>
<MPCVObject>
  <Version>
    <File_Version>2.1</File_Version>
    <Application>MPC TRAP-FORGE</Application>
  </Version>
  <Program type="Drum">
    <ProgramName>TRAP_FORGE_KIT</ProgramName>
    <Pads>${padsXml}
    </Pads>
  </Program>
</MPCVObject>`;
  }

  function init() {
    let deferredPrompt;
    const installBtn = document.getElementById("installPwaBtn");
    window.addEventListener("beforeinstallprompt", (e) => {
      e.preventDefault();
      deferredPrompt = e;
      installBtn?.classList.remove("hidden");
    });
    installBtn?.addEventListener("click", async () => {
      if (!deferredPrompt) return;
      deferredPrompt.prompt();
      const { outcome } = await deferredPrompt.userChoice;
      if (outcome === "accepted") installBtn.classList.add("hidden");
      deferredPrompt = null;
    });

    if ("serviceWorker" in navigator) {
      navigator.serviceWorker.register("./sw.js").catch(() => {});
    }

    document.getElementById("masterPreviewBtn")?.addEventListener("click", () => playCurrentSound(0));
    document.querySelectorAll(".preview-btn-inline").forEach((btn) => btn.addEventListener("click", () => playCurrentSound(0)));

    window.addEventListener("keydown", (e) => {
      if (e.target.tagName === "INPUT" || e.target.tagName === "SELECT" || e.target.tagName === "TEXTAREA") return;
      if (e.code === "Space") {
        e.preventDefault();
        playCurrentSound(0);
        const rootPad = document.querySelector('.mpc-pad[data-root="true"]');
        if (rootPad) {
          rootPad.classList.add("mpc-pad-active");
          setTimeout(() => rootPad.classList.remove("mpc-pad-active"), 140);
        }
      }
    });

    document.querySelectorAll(".nav-tab").forEach((tab) => {
      tab.addEventListener("click", () => {
        const targetId = tab.getAttribute("data-target");
        document.querySelectorAll(".nav-tab").forEach((t) => {
          t.className =
            "nav-tab px-3.5 py-2 rounded-t-lg bg-mpc-panel/50 border-t-2 border-transparent text-slate-400 hover:text-white flex items-center gap-1.5 transition";
        });
        tab.className =
          "nav-tab active-tab px-3.5 py-2 rounded-t-lg bg-mpc-panel border-t-2 border-l border-r border-mpc-accent text-white flex items-center gap-1.5 transition";
        document.querySelectorAll(".tab-page").forEach((page) => {
          page.classList.toggle("hidden", page.id !== targetId);
        });
      });
    });

    bindSlider("ampAttack", currentSound.amp, "attack", "ms");
    bindSlider("ampDecay", currentSound.amp, "decay", "ms");
    bindSlider("ampSustain", currentSound.amp, "sustain", "%", 100);
    bindSlider("ampRelease", currentSound.amp, "release", "ms");
    bindSlider("filterCutoff", currentSound.filter, "cutoff", "Hz");
    bindSlider("filterReso", currentSound.filter, "reso", "");
    bindSlider("filterEnv", currentSound.filter, "env", "%", 100);
    bindSlider("filterDecay", currentSound.filter, "decay", "ms");
    bindSlider("transientAttack", currentSound.transient, "attack", "dB");
    bindSlider("transientSustain", currentSound.transient, "sustain", "dB");
    bindSlider("pitchStart", currentSound.transient, "pitchStart", "Hz");
    bindSlider("pitchDecay", currentSound.transient, "pitchDecay", "ms");
    bindSlider("glideMs", currentSound.transient, "glideMs", "secms");
    bindSlider("glideSemi", currentSound.transient, "glideSemi", "semi");
    bindSlider("driveAmount", currentSound.tx, "drive", "%");
    bindSlider("subHarmonics", currentSound.tx, "subHarm", "%");
    bindSlider("noiseAmount", currentSound.tx, "noise", "%");
    bindSlider("outputCeiling", currentSound.tx, "ceiling", "dB");

    document.getElementById("filterType")?.addEventListener("change", (e) => {
      currentSound.filter.type = e.target.value;
      playCurrentSound(0);
    });
    document.getElementById("distType")?.addEventListener("change", (e) => {
      currentSound.tx.type = e.target.value;
      playCurrentSound(0);
    });
    document.getElementById("dacEmulationSelect")?.addEventListener("change", () => {
      playCurrentSound(0);
      showToast("DAC ENGAGED", `Hardware profile: ${document.getElementById("dacEmulationSelect").selectedOptions[0].text}`, "info");
    });

    const pads = document.querySelectorAll(".mpc-pad");
    let rollInterval = null;

    function renderPadsUI() {
      const title = document.getElementById("padMatrixTitle");
      if (padMode === "16levels") title.textContent = "AKAI MPC 16-LEVELS PITCH AUDITION MATRIX";
      else title.textContent = "AKAI MPC 16-PAD TRAP KIT MATRIX";
      pads.forEach((pad, idx) => {
        pad.className =
          "mpc-pad bg-mpc-card border border-mpc-border rounded-xl p-3 sm:p-4 flex flex-col items-center justify-center cursor-pointer mpc-pad-shadow select-none transition-all duration-75 text-center";
        if (padMode === "16levels") {
          const semitone = parseInt(pad.getAttribute("data-semitone"), 10);
          const isRoot = pad.getAttribute("data-root") === "true";
          pad.innerHTML = isRoot
            ? `<span class="text-mpc-accent font-bold text-xs sm:text-sm">C1 (ROOT)</span><span class="text-[10px] text-slate-400 font-mono">0 st</span>`
            : `<span class="text-xs sm:text-sm text-slate-200 font-bold font-mono">${semitone > 0 ? "+" : ""}${semitone} st</span>`;
          if (isRoot) pad.classList.add("border-mpc-accent/80", "bg-mpc-card/90");
        } else {
          const kitItem = kitLibrary[idx] || currentSound;
          const isSelected = kitItem.name === currentSound.name;
          pad.innerHTML = `<span class="text-[10px] text-slate-400 font-mono">PAD ${idx + 1}</span><span class="text-xs font-bold font-tech truncate w-full ${isSelected ? "text-mpc-cyan" : "text-slate-200"}">${kitItem.name.replace(/_/g, " ")}</span>`;
          if (isSelected) pad.classList.add("border-mpc-cyan/80", "bg-mpc-cyan/10");
        }
      });
    }

    pads.forEach((pad, idx) => {
      const triggerPad = () => {
        pad.classList.add("mpc-pad-active");
        if (padMode === "16levels") {
          playCurrentSound(parseInt(pad.getAttribute("data-semitone"), 10));
        } else {
          currentSound = JSON.parse(JSON.stringify(kitLibrary[idx]));
          syncUiFromState();
          playCurrentSound(0);
          renderPadsUI();
        }
      };
      const releasePad = () => {
        pad.classList.remove("mpc-pad-active");
        if (rollInterval) {
          clearInterval(rollInterval);
          rollInterval = null;
        }
      };
      pad.addEventListener("mousedown", () => {
        triggerPad();
        const rollRate = parseInt(document.getElementById("rollRateSelect").value, 10);
        if (rollRate > 0) {
          const bpm = parseInt(document.getElementById("seqBpm")?.value || 140, 10);
          rollInterval = setInterval(triggerPad, 60000 / bpm / (rollRate / 4));
        }
      });
      pad.addEventListener("mouseup", releasePad);
      pad.addEventListener("mouseleave", releasePad);
      pad.addEventListener("touchstart", (e) => {
        e.preventDefault();
        triggerPad();
      });
      pad.addEventListener("touchend", releasePad);
    });

    document.getElementById("padMode16Levels")?.addEventListener("click", () => {
      padMode = "16levels";
      document.getElementById("padMode16Levels").className = "px-2.5 py-1 rounded-md bg-mpc-accent text-white font-bold transition";
      document.getElementById("padModeKit").className = "px-2.5 py-1 rounded-md text-slate-400 hover:text-white transition";
      renderPadsUI();
    });
    document.getElementById("padModeKit")?.addEventListener("click", () => {
      padMode = "kit";
      document.getElementById("padModeKit").className = "px-2.5 py-1 rounded-md bg-mpc-cyan text-black font-bold transition";
      document.getElementById("padMode16Levels").className = "px-2.5 py-1 rounded-md text-slate-400 hover:text-white transition";
      renderPadsUI();
    });

    document.querySelectorAll(".cat-selector").forEach((btn) => {
      btn.addEventListener("click", () => {
        const cat = btn.getAttribute("data-cat");
        document.querySelectorAll(".cat-selector").forEach((b) => {
          b.className =
            "cat-selector px-3 py-1.5 rounded-lg border border-mpc-border bg-mpc-card hover:border-mpc-accent text-slate-300 transition";
        });
        btn.className = "cat-selector px-3 py-1.5 rounded-lg border border-mpc-accent bg-mpc-accent/20 text-white transition";
        const matched = PRESET_LIBRARY.find((p) => p.cat === cat);
        if (matched) {
          currentSound = JSON.parse(JSON.stringify(matched));
          syncUiFromState();
          playCurrentSound(0);
        }
      });
    });

    const presetPillsContainer = document.getElementById("presetPills");
    PRESET_LIBRARY.forEach((preset) => {
      const pBtn = document.createElement("button");
      pBtn.className =
        "px-2.5 py-1.5 rounded-lg bg-mpc-card hover:bg-mpc-border border border-mpc-border text-slate-300 hover:text-white transition text-xs font-mono truncate";
      pBtn.textContent = preset.name;
      pBtn.addEventListener("click", () => {
        currentSound = JSON.parse(JSON.stringify(preset));
        syncUiFromState();
        playCurrentSound(0);
        showToast("PRESET LOADED", `Loaded ${preset.name} (${preset.desc})`, "info");
      });
      presetPillsContainer.appendChild(pBtn);
    });

    const promptInput = document.getElementById("promptInput");
    const generateAiBtn = document.getElementById("generateAiBtn");
    generateAiBtn?.addEventListener("click", () => {
      const p = promptInput.value.trim();
      if (!p) {
        showToast("TYPE A VIBE", "Please enter a style or pick an inspiration pill!", "error");
        return;
      }
      callGeminiAiForge(p);
    });
    promptInput?.addEventListener("keydown", (e) => {
      if (e.key === "Enter") generateAiBtn.click();
    });
    document.querySelectorAll(".vibe-pill").forEach((pill) => {
      pill.addEventListener("click", () => {
        const prompt = pill.getAttribute("data-prompt");
        promptInput.value = prompt;
        callGeminiAiForge(prompt);
      });
    });
    document.getElementById("quickRandomBtn")?.addEventListener("click", mutateProceduralRandom);

    document.getElementById("seqPlayBtn")?.addEventListener("click", () => {
      isSequencerPlaying = !isSequencerPlaying;
      const playText = document.getElementById("seqPlayText");
      if (isSequencerPlaying) {
        getAudioContext();
        seqCurrentStep = 0;
        stepSequencerTick();
        playText.textContent = "STOP";
        document.getElementById("seqPlayBtn").className =
          "px-5 py-1.5 rounded-lg bg-mpc-accent/20 border border-mpc-accent text-mpc-accent font-tech font-bold text-xs uppercase transition";
      } else {
        clearTimeout(seqTimerId);
        playText.textContent = "PLAY";
        document.getElementById("seqPlayBtn").className =
          "px-5 py-1.5 rounded-lg bg-mpc-neon/20 border border-mpc-neon/60 text-mpc-neon font-tech font-bold text-xs uppercase transition";
        document.querySelectorAll(".seq-step").forEach((b) => b.classList.remove("ring-2", "ring-mpc-cyan"));
      }
    });
    document.getElementById("seqClearBtn")?.addEventListener("click", () => {
      seqTracks.forEach((t) => t.steps.fill(0));
      renderSequencerTracks();
    });

    document.querySelectorAll(".export-specific-btn").forEach((btn) => {
      btn.addEventListener("click", async () => {
        const format = btn.getAttribute("data-format");
        if (format === "xpm") {
          const blob = new Blob([generateMpcProgramXpm(kitLibrary)], { type: "application/xml" });
          const url = URL.createObjectURL(blob);
          const a = document.createElement("a");
          a.href = url;
          a.download = "TRAP_FORGE_KIT.xpm";
          a.click();
          URL.revokeObjectURL(url);
          showToast("XPM EXPORTED", "Akai Drum Program saved for MPC hardware!", "success");
          return;
        }
        const buffer = await synthesizeDrumBuffer(currentSound, 0);
        const wavBlob = audioBufferToWav(buffer, format);
        const filename = `MPC_${currentSound.cat}_${currentSound.name}.wav`;
        const url = URL.createObjectURL(wavBlob);
        const a = document.createElement("a");
        a.href = url;
        a.download = filename;
        a.click();
        URL.revokeObjectURL(url);
        showToast("EXPORT COMPLETE", `Downloaded ${filename}`, "success");
      });
    });

    const helpModal = document.getElementById("helpModal");
    document.getElementById("quickHelpBtn")?.addEventListener("click", () => helpModal.classList.remove("hidden"));
    document.getElementById("closeHelpBtn")?.addEventListener("click", () => helpModal.classList.add("hidden"));
    document.getElementById("dismissHelpBtn")?.addEventListener("click", () => helpModal.classList.add("hidden"));

    const keyInput = document.getElementById("geminiApiKey");
    const stored = localStorage.getItem("trapforge_gemini_key");
    if (keyInput && stored) keyInput.value = stored;
    keyInput?.addEventListener("change", () => {
      localStorage.setItem("trapforge_gemini_key", keyInput.value.trim());
    });

    syncUiFromState();
    renderPadsUI();
    renderSequencerTracks();
  }

  if (document.readyState === "loading") document.addEventListener("DOMContentLoaded", init);
  else init();
})();
