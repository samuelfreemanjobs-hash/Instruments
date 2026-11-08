const STORAGE_KEY = "disklordz-audio-coach-v2";
const REMINDER_KEY = "disklordz-coach-reminder";

const FALLBACK_PLAYBOOKS = {};

async function loadJson(path) {
  try {
    const res = await fetch(path);
    if (res.ok) return res.json();
  } catch {
    /* offline */
  }
  return null;
}

function loadState() {
  try {
    return JSON.parse(localStorage.getItem(STORAGE_KEY) || "{}");
  } catch {
    return {};
  }
}

function saveState(state) {
  state.updated_at = new Date().toISOString();
  localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
}

function ensureState() {
  const s = loadState();
  if (!s.completed) s.completed = {};
  if (!s.phase) s.phase = "foundation";
  return s;
}

function nextItem(items, completed) {
  return items.find((i) => !completed[i.id]) || null;
}

function cursorPrompt(task) {
  return `DiskLordz Audio — implement for me (VST lane only):

Task: ${task.headline}
Context: docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md §18
Sprint item id: ${task.id}

Report using §18.3 format (completed, files, build, tests, limitations, next).`;
}

function renderTask(task, playbooks, state, items) {
  const card = document.getElementById("coach-card");
  const stepsPanel = document.getElementById("steps-panel");
  const donePanel = document.getElementById("done-panel");
  const btnDone = document.getElementById("btn-done");
  const btnDelegate = document.getElementById("btn-delegate");

  const doneCount = items.filter((i) => state.completed[i.id]).length;
  document.getElementById("sprint-line").textContent = `Sprint ${doneCount} / ${items.length}`;

  if (!task) {
    card.innerHTML = `
      <h2 class="headline">Sprint complete</h2>
      <p>You cleared the 10-step startup list. Next: weekly review + beta planning.</p>
      <p class="why">Run <code>weekly_review.py --write</code> or chat with audio-business-copilot.</p>
    `;
    stepsPanel.classList.add("hidden");
    donePanel.classList.add("hidden");
    btnDone.disabled = true;
    btnDelegate.classList.add("hidden");
    return;
  }

  const book = playbooks[task.id] || {};
  const owner = book.owner || "you";
  const badge = owner === "cursor" ? "Delegate to Cursor" : "Your task";

  card.innerHTML = `
    <span class="badge">${badge}</span>
    <h2 class="headline">${book.headline || task.label}</h2>
    <p class="why">${book.why || ""}</p>
    <p class="meta">~${book.minutes ?? 30} min</p>
  `;

  const steps = book.steps || [task.label];
  const ol = document.getElementById("step-list");
  ol.innerHTML = steps.map((s) => `<li>${s}</li>`).join("");
  stepsPanel.classList.remove("hidden");

  document.getElementById("done-when").textContent = book.done_when || "You marked it done in Coach.";
  donePanel.classList.remove("hidden");

  btnDone.disabled = false;
  btnDone.onclick = () => {
    state.completed[task.id] = true;
    saveState(state);
    maybeNotifyNext(task.id);
    init();
  };

  if (owner === "cursor") {
    btnDelegate.classList.remove("hidden");
    btnDelegate.onclick = async () => {
      const text = cursorPrompt({ ...task, ...book, headline: book.headline || task.label });
      try {
        await navigator.clipboard.writeText(text);
        btnDelegate.textContent = "Copied — paste in Cursor";
      } catch {
        window.prompt("Cursor prompt:", text);
      }
    };
  } else {
    btnDelegate.classList.add("hidden");
  }
}

function renderChecklist(items, state) {
  const list = document.getElementById("checklist");
  list.innerHTML = items
    .map((item) => {
      const checked = state.completed[item.id];
      return `<li class="${checked ? "done" : ""}">
        <span>${item.label}</span>
      </li>`;
    })
    .join("");
}

function exportProgress(state, items) {
  const completed_ids = items.filter((i) => state.completed[i.id]).map((i) => i.id);
  return {
    updated_at: state.updated_at,
    completed_ids,
    notes: { weekly: state.notes || "" },
    current_phase: state.phase,
    build_status: "operator-reported",
    open_bugs: 0,
    next_milestone: "Milestone 001 — Plugin Foundation",
  };
}

function maybeNotifyNext(finishedId) {
  if (Notification.permission !== "granted") return;
  new Notification("DiskLordz Coach", {
    body: `Nice — finished “${finishedId}”. Open the app for your next task.`,
    icon: "./icon.svg",
  });
}

function scheduleDailyReminder() {
  const t = localStorage.getItem(REMINDER_KEY) || "09:00";
  const [h, m] = t.split(":").map(Number);
  const now = new Date();
  const next = new Date(now);
  next.setHours(h, m, 0, 0);
  if (next <= now) next.setDate(next.getDate() + 1);
  const ms = next.getTime() - now.getTime();
  setTimeout(async () => {
    if (Notification.permission === "granted") {
      const checklist = await loadJson("../data/sprint-checklist.json");
      const playbooks = (await loadJson("../data/task-playbooks.json")) || FALLBACK_PLAYBOOKS;
      const state = ensureState();
      const items = checklist?.items || [];
      const task = nextItem(items, state.completed);
      const book = task ? playbooks[task.id] : null;
      new Notification("DiskLordz — today's task", {
        body: book?.headline || "Open Coach for your next step",
        icon: "./icon.svg",
      });
    }
    scheduleDailyReminder();
  }, ms);
}

async function init() {
  if ("serviceWorker" in navigator) {
    navigator.serviceWorker.register("./sw.js").catch(() => {});
  }

  const checklist = await loadJson("../data/sprint-checklist.json");
  const playbooks = (await loadJson("../data/task-playbooks.json")) || FALLBACK_PLAYBOOKS;
  const items = checklist?.items || [];
  const state = ensureState();
  const task = nextItem(items, state.completed);

  renderTask(task, playbooks, state, items);
  renderChecklist(items, state);

  document.getElementById("toggle-checklist").addEventListener("click", (e) => {
    e.preventDefault();
    document.getElementById("checklist-section").classList.toggle("hidden");
  });

  document.getElementById("sync-progress").addEventListener("click", () => {
    const blob = new Blob([JSON.stringify(exportProgress(state, items), null, 2)], {
      type: "application/json",
    });
    const a = document.createElement("a");
    a.href = URL.createObjectURL(blob);
    a.download = "progress.json";
    a.click();
    URL.revokeObjectURL(a.href);
  });

  document.getElementById("reminder-time").addEventListener("change", (e) => {
    localStorage.setItem(REMINDER_KEY, e.target.value);
    scheduleDailyReminder();
  });

  document.getElementById("btn-notify").addEventListener("click", async () => {
    const perm = await Notification.requestPermission();
    if (perm === "granted") {
      scheduleDailyReminder();
      document.getElementById("btn-notify").textContent = "Notifications on";
      const checklist2 = await loadJson("../data/sprint-checklist.json");
      const playbooks2 = (await loadJson("../data/task-playbooks.json")) || {};
      const state2 = ensureState();
      const task2 = nextItem(checklist2?.items || [], state2.completed);
      const book = task2 ? playbooks2[task2.id] : null;
      new Notification("DiskLordz Coach", {
        body: book?.headline || "You're set — we'll nudge you daily.",
        icon: "./icon.svg",
      });
    }
  });

  const savedTime = localStorage.getItem(REMINDER_KEY);
  if (savedTime) document.getElementById("reminder-time").value = savedTime;
  if (Notification.permission === "granted") scheduleDailyReminder();
}

init();
