const STORAGE_KEY = "disklordz-audio-command-v1";

const FALLBACK_ITEMS = [
  { id: "company-identity", label: "Company identity — naming, logo direction, visual system" },
  { id: "dev-environment", label: "Development environment — C++, JUCE, CMake, Visual Studio verified" },
  { id: "github-repo", label: "GitHub repository — private company repo + folder structure (DPF layout)" },
  { id: "plugin-skeleton", label: "Plugin skeleton — blank VST3 loads in test host" },
  { id: "midi-output", label: "MIDI output — generated notes reach DAW instrument" },
  { id: "theory-engine", label: "Music theory engine — notes, intervals, keys, scales" },
  { id: "chord-engine", label: "Chord engine — triads, sevenths, extended chords" },
  { id: "progression-generator", label: "Progression generator — validate four-chord progressions" },
  { id: "first-interface", label: "First interface — key selector, chord browser, progression grid" },
  { id: "first-alpha", label: "First alpha — packaged CHORD ENGINE prototype" },
];

function loadState() {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (raw) return JSON.parse(raw);
  } catch {
    /* ignore */
  }
  return {
    completed: {},
    phase: "foundation",
    notes: "",
    updated_at: new Date().toISOString(),
  };
}

function saveState(state) {
  state.updated_at = new Date().toISOString();
  localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
}

function render(items, state) {
  const list = document.getElementById("checklist");
  const score = document.getElementById("score");
  list.innerHTML = "";
  let done = 0;
  items.forEach((item) => {
    const li = document.createElement("li");
    const checked = Boolean(state.completed[item.id]);
    if (checked) done += 1;
    li.className = checked ? "done" : "";
    const id = `chk-${item.id}`;
    li.innerHTML = `
      <input type="checkbox" id="${id}" ${checked ? "checked" : ""} data-id="${item.id}" />
      <label for="${id}">${item.label}</label>
    `;
    list.appendChild(li);
  });
  score.textContent = `${done} / ${items.length}`;

  document.getElementById("phase").value = state.phase || "foundation";
  document.getElementById("weekly-notes").value = state.notes || "";
}

async function loadChecklist() {
  try {
    const res = await fetch("../data/sprint-checklist.json");
    if (res.ok) {
      const data = await res.json();
      return data.items.map((i) => ({ id: i.id, label: i.label }));
    }
  } catch {
    /* file:// or offline */
  }
  return FALLBACK_ITEMS;
}

function weeklySnippet(state, items) {
  const done = items.filter((i) => state.completed[i.id]).map((i) => i.label);
  const open = items.filter((i) => !state.completed[i.id]).map((i) => i.label);
  const lines = [
    "## DiskLordz Audio — weekly review (PWA export)",
    "",
    `**Phase:** ${state.phase}`,
    `**Updated:** ${state.updated_at}`,
    "",
    "### Completed",
    ...(done.length ? done.map((d) => `- ${d}`) : ["- (none)"]),
    "",
    "### Outstanding",
    ...(open.length ? open.map((d) => `- ${d}`) : ["- (all sprint items done)"]),
    "",
    "### Notes",
    state.notes || "_(none)_",
    "",
    "### Next",
    "- Run `python3 disklordz/audio-command/scripts/weekly_review.py --write`",
    "- Invoke **audio-business-copilot** for planning; Cursor for VST implementation",
  ];
  return lines.join("\n");
}

async function main() {
  if ("serviceWorker" in navigator) {
    navigator.serviceWorker.register("./sw.js").catch(() => {});
  }

  const items = await loadChecklist();
  let state = loadState();
  render(items, state);

  document.getElementById("checklist").addEventListener("change", (e) => {
    const t = e.target;
    if (t.matches('input[type="checkbox"]')) {
      state.completed[t.dataset.id] = t.checked;
      saveState(state);
      render(items, state);
    }
  });

  document.getElementById("phase").addEventListener("change", (e) => {
    state.phase = e.target.value;
    saveState(state);
  });

  document.getElementById("weekly-notes").addEventListener("input", (e) => {
    state.notes = e.target.value;
    saveState(state);
  });

  document.getElementById("export-json").addEventListener("click", () => {
    const payload = {
      updated_at: state.updated_at,
      completed_ids: Object.keys(state.completed).filter((k) => state.completed[k]),
      notes: { weekly: state.notes },
      current_phase: state.phase,
      build_status: "operator-reported",
      open_bugs: 0,
      next_milestone: "See docs/DISKLORDZ_MASTER_PLAN.md",
    };
    const blob = new Blob([JSON.stringify(payload, null, 2)], { type: "application/json" });
    const a = document.createElement("a");
    a.href = URL.createObjectURL(blob);
    a.download = "progress.json";
    a.click();
    URL.revokeObjectURL(a.href);
  });

  document.getElementById("copy-review").addEventListener("click", async () => {
    const text = weeklySnippet(state, items);
    try {
      await navigator.clipboard.writeText(text);
    } catch {
      window.prompt("Copy weekly review:", text);
    }
  });
}

main();
