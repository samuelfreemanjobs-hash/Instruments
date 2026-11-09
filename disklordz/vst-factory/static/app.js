let selectedClass = "midi";
let currentProject = null;

const CLASSES = [
  { id: "synth", label: "Synthesizer" },
  { id: "effect", label: "Effects" },
  { id: "midi", label: "MIDI" },
  { id: "sampler", label: "Sampler" },
];

function log(msg) {
  const el = document.getElementById("log");
  el.textContent = (el.textContent ? el.textContent + "\n" : "") + msg;
}

async function refreshApprovals() {
  const banner = document.getElementById("approval-banner");
  if (!currentProject) {
    banner.classList.add("hidden");
    banner.innerHTML = "";
    return;
  }
  try {
    const rows = await api(`/api/projects/${currentProject.id}/approvals`);
    const pending = rows.filter((a) => a.status === "pending");
    if (!pending.length) {
      banner.classList.add("hidden");
      banner.innerHTML = "";
      return;
    }
    banner.classList.remove("hidden");
    banner.innerHTML =
      "<strong>Approval required</strong><br/>" +
      pending.map((a) => `Gate <code>${a.gate}</code> (id ${a.id})`).join("<br/>");
    pending.forEach((a) => {
      const b = document.createElement("button");
      b.type = "button";
      b.textContent = `Approve ${a.gate}`;
      b.onclick = async () => {
        await api(`/api/projects/${currentProject.id}/approvals/${a.id}/resolve`, {
          method: "POST",
          body: JSON.stringify({ status: "approved" }),
        });
        log(`Approved ${a.gate}`);
        await refreshApprovals();
        await refreshPipeline();
      };
      banner.appendChild(b);
    });
  } catch (e) {
    log("Approvals: " + e.message);
  }
}

async function refreshPipeline() {
  const strip = document.getElementById("pipeline-strip");
  if (!currentProject) {
    strip.innerHTML = "";
    return;
  }
  try {
    const pipe = await api(`/api/projects/${currentProject.id}/pipeline`);
    const approvals = await api(`/api/projects/${currentProject.id}/approvals`);
    strip.innerHTML = "";
    (pipe.phases || []).forEach((stage) => {
      const el = document.createElement("div");
      el.className = "stage " + (stage.status || "pending");
      el.title = [stage.verify, stage.sop, stage.command].filter(Boolean).join(" · ");
      el.appendChild(document.createTextNode(stage.id));
      if (stage.status === "blocked" && stage.gate) {
        const pending = approvals.find(
          (a) => a.gate === stage.gate && a.status === "pending"
        );
        if (pending) {
          const b = document.createElement("button");
          b.type = "button";
          b.textContent = "Approve";
          b.onclick = async () => {
            await api(
              `/api/projects/${currentProject.id}/approvals/${pending.id}/resolve`,
              { method: "POST", body: JSON.stringify({ status: "approved" }) }
            );
            log(`Approved ${stage.gate}`);
            await refreshApprovals();
            await refreshPipeline();
          };
          el.appendChild(b);
        }
      }
      strip.appendChild(el);
    });
    if (pipe.blocking_gate) {
      log(`Pipeline blocking gate: ${pipe.blocking_gate}`);
    }
  } catch (e) {
    log("Pipeline: " + e.message);
  }
}

function setProject(proj) {
  currentProject = proj;
  const meta = document.getElementById("project-meta");
  meta.textContent = proj
    ? `#${proj.id} ${proj.plugin_name} · ${proj.state} · ${proj.plugin_class}`
    : "No project selected.";
  document.getElementById("btn-scaffold").disabled = !proj;
  document.getElementById("btn-sync").disabled = !proj;
  document.getElementById("btn-build").disabled = !proj || !proj.vst_path;
  document.getElementById("btn-test").disabled = !proj || !proj.vst_path;
  document.getElementById("btn-ship").disabled = !proj;
  document.getElementById("btn-prompt").disabled = !proj;
  refreshApprovals();
  refreshPipeline();
}

async function api(path, opts = {}) {
  const res = await fetch(path, {
    headers: { "Content-Type": "application/json", ...(opts.headers || {}) },
    ...opts,
  });
  const text = await res.text();
  let data;
  try {
    data = JSON.parse(text);
  } catch {
    data = { detail: text };
  }
  if (!res.ok) throw new Error(data.detail || res.statusText);
  return data;
}

function initChips() {
  const wrap = document.getElementById("class-chips");
  CLASSES.forEach((c) => {
    const b = document.createElement("button");
    b.type = "button";
    b.className = "chip" + (c.id === selectedClass ? " active" : "");
    b.textContent = c.label;
    b.onclick = () => {
      selectedClass = c.id;
      wrap.querySelectorAll(".chip").forEach((el) => el.classList.remove("active"));
      b.classList.add("active");
    };
    wrap.appendChild(b);
  });
}

async function loadModules() {
  const blocks = await api("/api/modules/dsp");
  const grid = document.getElementById("modules-dsp");
  blocks.forEach((b) => {
    const s = document.createElement("span");
    s.textContent = b.id;
    s.title = b.name;
    grid.appendChild(s);
  });
  const chord = await api("/api/references/chord-engine");
  document.getElementById("chord-ref").textContent = JSON.stringify(chord, null, 2);
}

document.getElementById("btn-create").onclick = async () => {
  const prompt = document.getElementById("prompt").value.trim();
  if (prompt.length < 8) {
    log("Prompt too short (min 8 chars).");
    return;
  }
  const name = document.getElementById("plugin-name").value.trim() || null;
  try {
    const body = {
      prompt,
      plugin_class: selectedClass,
      product_category: name === "MeterTap" ? "fx.metering" : undefined,
    };
    if (name) body.plugin_name = name;
    const proj = await api("/api/projects", { method: "POST", body: JSON.stringify(body) });
    setProject(proj);
    log(`Created project ${proj.plugin_name}`);
  } catch (e) {
    log("Create failed: " + e.message);
  }
};

document.getElementById("btn-scaffold").onclick = async () => {
  if (!currentProject) return;
  try {
    const proj = await api(`/api/projects/${currentProject.id}/scaffold`, { method: "POST" });
    setProject(proj);
    log(`Scaffolded ${proj.apc_path} + ${proj.vst_path}`);
  } catch (e) {
    log("Scaffold failed: " + e.message);
  }
};

document.getElementById("btn-sync").onclick = async () => {
  if (!currentProject) return;
  try {
    const proj = await api(`/api/projects/${currentProject.id}/sync`, { method: "POST" });
    setProject(proj);
    log(`Synced phase=${proj.phase} apc=${proj.apc_phase || "-"}`);
  } catch (e) {
    log("Sync failed: " + e.message);
  }
};

document.getElementById("btn-ship").onclick = async () => {
  if (!currentProject) return;
  try {
    const res = await api(`/api/projects/${currentProject.id}/ship`, { method: "POST" });
    setProject(res);
    log(`Ship ${res.ship_status || ""}: ${(res.last_output_tail || "").slice(-2000)}`);
  } catch (e) {
    log("Ship failed: " + e.message);
  }
};

document.getElementById("btn-build").onclick = async () => {
  if (!currentProject) return;
  try {
    const res = await api(`/api/projects/${currentProject.id}/build`, { method: "POST" });
    setProject(res);
    log((res.last_output_tail || "").slice(-2000));
  } catch (e) {
    log("Build failed: " + e.message);
  }
};

document.getElementById("btn-test").onclick = async () => {
  if (!currentProject) return;
  try {
    const res = await api(`/api/projects/${currentProject.id}/test`, { method: "POST" });
    setProject(res);
    log((res.last_output_tail || "").slice(-2000));
  } catch (e) {
    log("Test failed: " + e.message);
  }
};

document.getElementById("btn-prompt").onclick = async () => {
  if (!currentProject) return;
  try {
    const res = await api(`/api/projects/${currentProject.id}/cursor-prompt?role=cpp`);
    await navigator.clipboard.writeText(res.prompt_text);
    log("Cursor prompt copied to clipboard.");
  } catch (e) {
    log("Prompt export failed: " + e.message);
  }
};

initChips();
loadModules().catch((e) => log("Init: " + e.message));
