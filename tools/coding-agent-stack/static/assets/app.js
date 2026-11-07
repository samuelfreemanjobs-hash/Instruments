const logEl = document.getElementById("log");
const form = document.getElementById("chat");
const promptEl = document.getElementById("prompt");
const sendBtn = document.getElementById("send");
const modeEl = document.getElementById("mode");
const routineWrap = document.getElementById("routine-wrap");
const routineEl = document.getElementById("routine");

function append(role, text) {
  const div = document.createElement("div");
  div.className = `msg ${role}`;
  div.textContent = text;
  logEl.appendChild(div);
  logEl.scrollTop = logEl.scrollHeight;
}

async function loadRoutines() {
  try {
    const res = await fetch("/grok/routines");
    const data = await res.json();
    routineEl.innerHTML = "";
    const none = document.createElement("option");
    none.value = "";
    none.textContent = "(no routine — free prompt)";
    routineEl.appendChild(none);
    const routines = data.routines || {};
    for (const [id, label] of Object.entries(routines)) {
      const opt = document.createElement("option");
      opt.value = id;
      opt.textContent = label + " (" + id + ")";
      routineEl.appendChild(opt);
    }
  } catch {
  }
}

function syncRoutineVisibility() {
  const grok = modeEl.value === "grok";
  routineWrap.hidden = !grok;
}

modeEl.addEventListener("change", syncRoutineVisibility);
loadRoutines();
syncRoutineVisibility();

form.addEventListener("submit", async (e) => {
  e.preventDefault();
  const prompt = promptEl.value.trim();
  if (!prompt) return;
  append("user", prompt);
  promptEl.value = "";
  sendBtn.disabled = true;
  const mode = modeEl.value;
  const threadId = "pwa-" + Date.now();
  try {
    let url = "/agent";
    let body = { prompt, thread_id: threadId };
    if (mode === "team") {
      url = "/agent/team";
      body.force_stub = true;
    } else if (mode === "grok") {
      const routineId = routineEl.value;
      url = routineId ? "/grok/routine/" + encodeURIComponent(routineId) : "/grok/send";
      body = {
        prompt,
        thread_id: threadId,
        architect_first: true,
      };
    }
    const res = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body),
    });
    const data = await res.json();
    if (mode === "grok") {
      const tag = data.dry_run ? "[grok dry-run] " : data.ok ? "[grok] " : "[grok error] ";
      append("assistant", tag + (data.detail || JSON.stringify(data)));
    } else {
      const prefix =
        data.mode === "stub" ? "[stub] " : data.qa_passed === false ? "[qa fail] " : "";
      append("assistant", prefix + (data.response || data.detail || "(empty)"));
    }
  } catch (err) {
    append("assistant", "Network error: " + err.message);
  } finally {
    sendBtn.disabled = false;
  }
});
