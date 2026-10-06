async function api(path, options = {}) {
  const res = await fetch(path, options);
  if (!res.ok) throw new Error(await res.text());
  return res.json();
}

async function loadSynths() {
  const synths = await api("/api/synths");
  const sel = document.getElementById("synthSelect");
  sel.innerHTML = "";
  synths.forEach((s) => {
    const opt = document.createElement("option");
    opt.value = s.id;
    opt.textContent = s.stub ? `${s.name} (stub)` : s.name;
    sel.appendChild(opt);
  });
}

function synthId() {
  return document.getElementById("synthSelect").value;
}

document.getElementById("btnBatch").addEventListener("click", async () => {
  const body = {
    synth_id: synthId(),
    count: Number(document.getElementById("count").value),
    category: document.getElementById("category").value,
  };
  const data = await api("/api/batch/generate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  document.getElementById("batchOut").textContent = JSON.stringify(data, null, 2);
  refreshVault();
});

document.getElementById("btnPrompt").addEventListener("click", async () => {
  const body = {
    synth_id: synthId(),
    count: 4,
    prompt: document.getElementById("prompt").value,
  };
  const data = await api("/api/prompt/generate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  document.getElementById("promptOut").textContent = JSON.stringify(data, null, 2);
  refreshVault();
});

document.querySelectorAll(".chip").forEach((btn) => {
  btn.addEventListener("click", () => {
    document.getElementById("prompt").value = btn.dataset.prompt;
  });
});

const dropzone = document.getElementById("dropzone");
const fileInput = document.getElementById("fileInput");

dropzone.addEventListener("click", () => fileInput.click());
dropzone.addEventListener("dragover", (e) => {
  e.preventDefault();
  dropzone.classList.add("dragover");
});
dropzone.addEventListener("dragleave", () => dropzone.classList.remove("dragover"));
dropzone.addEventListener("drop", (e) => {
  e.preventDefault();
  dropzone.classList.remove("dragover");
  if (e.dataTransfer.files[0]) uploadClone(e.dataTransfer.files[0]);
});
fileInput.addEventListener("change", () => {
  if (fileInput.files[0]) uploadClone(fileInput.files[0]);
});

async function uploadClone(file) {
  const fd = new FormData();
  fd.append("file", file);
  const res = await fetch(`/api/clone?synth_id=${encodeURIComponent(synthId())}&count=4`, {
    method: "POST",
    body: fd,
  });
  const data = await res.json();
  document.getElementById("cloneOut").textContent = JSON.stringify(data, null, 2);
  refreshVault();
}

async function refreshVault() {
  const presets = await api("/api/memory?limit=20");
  const ul = document.getElementById("vault");
  ul.innerHTML = "";
  presets.forEach((p) => {
    const li = document.createElement("li");
    li.innerHTML = `<span>${p.name} · ${p.synth_id}</span>`;
    const actions = document.createElement("span");
    const preview = document.createElement("button");
    preview.textContent = "Preview";
    preview.style.width = "auto";
    preview.addEventListener("click", async () => {
      const info = await api(`/api/preview/${p.id}`);
      new Audio(info.url).play();
    });
    const exp = document.createElement("button");
    exp.textContent = "Export";
    exp.style.width = "auto";
    exp.addEventListener("click", async () => {
      const info = await api(`/api/export/${p.id}`);
      alert(`Export ${info.size} bytes · ${info.hex_preview}`);
    });
    actions.append(preview, exp);
    li.appendChild(actions);
    ul.appendChild(li);
  });
}

document.getElementById("btnRefresh").addEventListener("click", refreshVault);

function buildKeyboard() {
  const kb = document.getElementById("keyboard");
  const notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  notes.forEach((n, i) => {
    const b = document.createElement("button");
    b.className = "key" + (n.includes("#") ? " black" : "");
    b.textContent = n;
    b.addEventListener("click", () => {
      document.getElementById("keyStatus").textContent = `MIDI note ${60 + i} (${n}4)`;
    });
    kb.appendChild(b);
  });
}

loadSynths().then(refreshVault);
buildKeyboard();
