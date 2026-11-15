from __future__ import annotations

import json
from typing import Any

from vst_factory import apc_bridge, build_broker, model_router, modules, references
from vst_factory.build_broker import find_release_artifact, find_vst3_bundle
from vst_factory.certification import evaluate_certification, load_recent_run_flags
from vst_factory.db import FactoryDB
from vst_factory.dsp_research import start_research
from vst_factory.models import FactoryState, PluginClass
from vst_factory.product_wo import build_product_wo
from vst_factory.slack_notify import notify_gate

# Canonical lifecycle order (matches modules/workflows/full-lifecycle.json).
PIPELINE_ORDER = [
    "intake",
    "dream",
    "plan",
    "design",
    "scaffold",
    "impl",
    "build",
    "test",
    "certify",
    "ship",
]

# FactoryState values that map onto pipeline stages (repair is a side-state).
_STATE_TO_PIPELINE = {
    FactoryState.INTAKE.value: "intake",
    FactoryState.DREAM.value: "dream",
    FactoryState.PLAN.value: "plan",
    FactoryState.DESIGN.value: "design",
    FactoryState.SCAFFOLD.value: "scaffold",
    FactoryState.IMPL.value: "impl",
    FactoryState.BUILD.value: "build",
    FactoryState.TEST.value: "test",
    FactoryState.SHIP.value: "ship",
    FactoryState.REPAIR.value: "build",  # treat repair as failed build/test progress
}


def _stage_index(stage: str) -> int:
    try:
        return PIPELINE_ORDER.index(stage)
    except ValueError:
        return -1


def _max_stage(a: str, b: str) -> str:
    return a if _stage_index(a) >= _stage_index(b) else b


class Orchestrator:
    def _notify_pending(self, project_id: int, gate: str, detail: str | None = None) -> None:
        proj = self.db.get_project(project_id)
        if not proj:
            return
        notify_gate(
            plugin_name=proj["plugin_name"],
            project_id=project_id,
            gate=gate,
            status="pending",
            detail=detail,
        )

    def _ensure_pending_gate(self, project_id: int, gate: str, detail: str | None = None) -> None:
        """Create gate once (any status) and Slack-notify only when newly pending."""
        for row in self.db.list_approvals(project_id):
            if row["gate"] == gate:
                return
        self.db.add_approval(project_id, gate=gate, status="pending", detail=detail)
        self._notify_pending(project_id, gate, detail=detail)

    def list_approvals(self, project_id: int) -> list[dict]:
        return self.db.list_approvals(project_id)

    def resolve_approval(self, project_id: int, approval_id: int, *, status: str = "approved") -> dict:
        rows = [a for a in self.db.list_approvals(project_id) if a["id"] == approval_id]
        if not rows:
            raise KeyError(approval_id)
        self.db.resolve_approval(approval_id, status=status)
        proj = self.db.get_project(project_id)
        name = proj["plugin_name"] if proj else "?"
        notify_gate(plugin_name=name, project_id=project_id, gate=rows[0]["gate"], status=status)
        return rows[0]

    def __init__(self, db: FactoryDB) -> None:
        self.db = db

    def create_project(
        self,
        *,
        prompt: str,
        plugin_class: PluginClass,
        plugin_name: str | None,
        product_category: str | None = None,
        autonomy_mode: str = "collaborative",
    ) -> dict:
        name = plugin_name or apc_bridge.suggest_plugin_name(prompt)
        if self.db.get_project_by_name(name):
            raise ValueError(f"Project name already exists: {name}")
        spec = modules.build_spec_from_modules(
            plugin_name=name,
            plugin_class=plugin_class.value,
            prompt=prompt,
            product_category=product_category,
        )
        spec["autonomy_mode"] = autonomy_mode
        refs = spec.setdefault("module_refs", {})
        refs["workflow_id"] = "workflow/full-lifecycle"
        pid = self.db.insert_project(
            plugin_name=name,
            plugin_class=plugin_class.value,
            state=FactoryState.INTAKE.value,
            spec=spec,
        )
        self.db.update_project(pid, state=FactoryState.DREAM.value)
        self.db.add_approval(pid, gate="approve_architecture", status="pending")
        self._notify_pending(pid, "approve_architecture")
        wo = build_product_wo(self.db.get_project(pid) or {})
        self.db.upsert_product_wo(pid, wo)
        references.update_liaison_greenfield_count(self.db.count_greenfield())
        return self.enrich_project(self.db.get_project(pid) or {})

    def _require_gate(self, project_id: int, gate: str) -> None:
        if not self.db.gate_approved(project_id, gate):
            pending = self.db.pending_gates(project_id)
            raise PermissionError(f"Gate '{gate}' not approved. Pending: {pending}")

    def enrich_project(self, proj: dict[str, Any]) -> dict[str, Any]:
        """Attach phase + validation for ProjectOut (non-destructive; no gate subprocess)."""
        name = proj["plugin_name"]
        status = apc_bridge.read_status(name)
        validation = dict((status or {}).get("validation") or {})
        apc_phase = None
        if status is not None:
            apc_phase = status.get("apc_phase") or status.get("current_phase")
        sync_meta = (proj.get("spec") or {}).get("apc_sync") or {}
        if sync_meta.get("validation"):
            validation = dict(sync_meta["validation"])
        if sync_meta.get("apc_phase"):
            apc_phase = sync_meta["apc_phase"]
        out = dict(proj)
        out["phase"] = _STATE_TO_PIPELINE.get(proj["state"], proj["state"])
        out["validation"] = validation or None
        out["apc_phase"] = apc_phase
        return out

    def _latest_run(self, project_id: int, kind: str) -> dict[str, Any] | None:
        for run in self.db.list_runs(project_id, limit=50):
            if run["kind"] == kind:
                return run
        return None

    def _run_ok(self, project_id: int, kind: str) -> bool:
        run = self._latest_run(project_id, kind)
        return bool(run and run["ok"])

    def _furthest_verified(self, proj: dict[str, Any]) -> str:
        """Compute furthest verified pipeline stage from APC + factory evidence."""
        name = proj["plugin_name"]
        furthest = "intake"
        # Project create always lands in dream with architecture gate pending.
        furthest = _max_stage(furthest, "dream")

        apc_phase, _validation = apc_bridge.infer_apc_phase(name)
        if apc_phase in PIPELINE_ORDER:
            furthest = _max_stage(furthest, apc_phase)

        if proj.get("vst_path") or (apc_bridge.vst_dir(apc_bridge.repo_root()) / name).is_dir():
            furthest = _max_stage(furthest, "scaffold")

        # Impl completion requires approve_gui — without it, cap below impl.
        if furthest == "impl" and not self.db.gate_approved(proj["id"], "approve_gui"):
            furthest = "design" if _stage_index(furthest) >= _stage_index("design") else furthest
            if proj.get("vst_path"):
                furthest = _max_stage(furthest, "scaffold")

        if self._run_ok(proj["id"], "build"):
            furthest = _max_stage(furthest, "build")
        if self._run_ok(proj["id"], "test"):
            furthest = _max_stage(furthest, "test")
        if self._run_ok(proj["id"], "certify"):
            furthest = _max_stage(furthest, "certify")
        ship_run = self._latest_run(proj["id"], "ship")
        if ship_run and ship_run["ok"] and "skipped:" not in (ship_run.get("output_tail") or ""):
            furthest = _max_stage(furthest, "ship")
        elif proj["state"] == FactoryState.SHIP.value:
            furthest = _max_stage(furthest, "ship")

        # Never regress below recorded factory state (except repair).
        if proj["state"] != FactoryState.REPAIR.value:
            mapped = _STATE_TO_PIPELINE.get(proj["state"])
            if mapped:
                if mapped == "impl" and not self.db.gate_approved(proj["id"], "approve_gui"):
                    pass
                else:
                    furthest = _max_stage(furthest, mapped)

        return furthest

    def sync_phase(self, project_id: int) -> dict[str, Any]:
        """Read APC status/gates and advance factory state to furthest verified phase."""
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)

        apc_phase, validation = apc_bridge.infer_apc_phase(proj["plugin_name"])

        # When design phase is verified, open approve_gui (once) + Slack notify.
        design_verified = apc_phase in ("design", "impl") or (
            bool(validation.get("architecture_defined"))
            and bool(validation.get("parameter_spec_exists"))
        )
        if design_verified:
            self._ensure_pending_gate(project_id, "approve_gui")

        furthest = self._furthest_verified(proj)

        # Cap impl on missing approve_gui.
        if furthest == "impl" and not self.db.gate_approved(project_id, "approve_gui"):
            furthest = "scaffold" if proj.get("vst_path") else "design"

        # Map pipeline stage → FactoryState (certify stays at test until ship).
        state_map = {
            "intake": FactoryState.INTAKE.value,
            "dream": FactoryState.DREAM.value,
            "plan": FactoryState.PLAN.value,
            "design": FactoryState.DESIGN.value,
            "scaffold": FactoryState.SCAFFOLD.value,
            "impl": FactoryState.IMPL.value,
            "build": FactoryState.BUILD.value,
            "test": FactoryState.TEST.value,
            "certify": FactoryState.TEST.value,
            "ship": FactoryState.SHIP.value,
        }
        new_state = state_map.get(furthest, proj["state"])
        if proj["state"] == FactoryState.REPAIR.value and furthest not in (
            "ship",
            "certify",
            "test",
            "build",
        ):
            new_state = FactoryState.REPAIR.value

        spec = dict(proj["spec"])
        spec["apc_sync"] = {
            "apc_phase": apc_phase,
            "validation": validation,
            "furthest_verified": furthest,
        }
        self.db.update_project(project_id, state=new_state, spec=spec)
        self.db.add_run(
            project_id,
            "sync",
            True,
            json.dumps({"apc_phase": apc_phase, "furthest": furthest, "validation": validation}),
        )
        self.db.upsert_product_wo(project_id, build_product_wo(self.db.get_project(project_id) or {}))
        out = self.enrich_project(self.db.get_project(project_id) or {})
        out["furthest_verified"] = furthest
        return out

    def scaffold(self, project_id: int) -> dict:
        self._require_gate(project_id, "approve_architecture")
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        root = apc_bridge.repo_root()
        name = proj["plugin_name"]
        pclass = proj["plugin_class"]
        spec = proj["spec"]

        existing_apc = apc_bridge.plugins_dir(root) / name
        existing_vst = apc_bridge.vst_dir(root) / name
        # Dogfood / resume: if both trees already exist, advance without rewriting.
        if existing_apc.is_dir() and existing_vst.is_dir():
            spec["vst_rel"] = f"vst/{name}"
            spec["plugins_rel"] = f"plugins/{name}"
            spec["scaffold_reused"] = True
            self.db.update_project(
                project_id,
                state=FactoryState.SCAFFOLD.value,
                spec=spec,
                apc_path=str(existing_apc.relative_to(root)),
                vst_path=str(existing_vst.relative_to(root)),
            )
            self.db.add_run(
                project_id,
                "scaffold",
                True,
                f"reused existing APC: {existing_apc}\nVST: {existing_vst}\n",
            )
            self.db.upsert_product_wo(
                project_id, build_product_wo(self.db.get_project(project_id) or {})
            )
            references.update_liaison_greenfield_count(self.db.count_greenfield())
            return self.enrich_project(self.db.get_project(project_id) or {})

        apc_path = apc_bridge.init_apc_workspace(root, name)
        brief = modules.materialize_creative_brief(pclass, name, spec.get("prompt", ""), spec)
        (apc_path / ".ideas" / "creative-brief.md").write_text(brief, encoding="utf-8")

        vst_path = apc_bridge.run_new_vst(root, name, pclass)
        spec["vst_rel"] = f"vst/{name}"
        spec["plugins_rel"] = f"plugins/{name}"

        self.db.update_project(
            project_id,
            state=FactoryState.SCAFFOLD.value,
            spec=spec,
            apc_path=str(apc_path.relative_to(root)),
            vst_path=str(vst_path.relative_to(root)),
        )
        self.db.add_run(project_id, "scaffold", True, f"APC: {apc_path}\nVST: {vst_path}\n")
        self.db.upsert_product_wo(project_id, build_product_wo(self.db.get_project(project_id) or {}))
        references.update_liaison_greenfield_count(self.db.count_greenfield())
        out = self.enrich_project(self.db.get_project(project_id) or {})
        return out

    def build(self, project_id: int) -> dict:
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        name = proj["plugin_name"]
        ok, output = build_broker.run_linux_build(name)
        self.db.add_run(project_id, "build", ok, output)
        state = FactoryState.BUILD.value if ok else FactoryState.REPAIR.value
        if ok:
            bundle = find_vst3_bundle(name)
            if bundle:
                spec = proj["spec"]
                spec["artefact"] = str(bundle)
                self.db.update_project(project_id, state=FactoryState.BUILD.value, spec=spec)
            else:
                self.db.update_project(project_id, state=state)
        else:
            attempts = proj["repair_attempts"] + 1
            self.db.update_project(project_id, state=state, repair_attempts=attempts)
        self.db.upsert_product_wo(project_id, build_product_wo(self.db.get_project(project_id) or {}))
        result = self.enrich_project(self.db.get_project(project_id) or {})
        result["last_ok"] = ok
        result["last_output_tail"] = output[-4000:]
        return result

    def test(self, project_id: int) -> dict:
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        name = proj["plugin_name"]
        ok, output = build_broker.run_factory_qa(name)
        self.db.add_run(project_id, "test", ok, output)
        state = FactoryState.TEST.value if ok else FactoryState.REPAIR.value
        if ok:
            self.db.update_project(project_id, state=FactoryState.TEST.value)
        else:
            attempts = proj["repair_attempts"] + 1
            self.db.update_project(project_id, state=state, repair_attempts=attempts)
        audio_ok, _ = build_broker.run_factory_audio_check(name)
        self.db.add_run(project_id, "test-audio", audio_ok, "factory-audio profile")
        self.db.upsert_product_wo(project_id, build_product_wo(self.db.get_project(project_id) or {}))
        result = self.enrich_project(self.db.get_project(project_id) or {})
        result["last_ok"] = ok and audio_ok
        result["last_output_tail"] = output[-4000:]
        return result

    def certify(self, project_id: int) -> dict[str, Any]:
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        name = proj["plugin_name"]
        build_ok, test_ok, audio_ok = load_recent_run_flags(project_id, self.db)
        artefact_ok = find_vst3_bundle(name) is not None
        report = evaluate_certification(
            build_ok=build_ok,
            artefact_ok=artefact_ok,
            pluginval_ok=test_ok,
            audio_ok=audio_ok,
        )
        self.db.add_run(project_id, "certify", report["passed"], json.dumps(report))
        if report["passed"]:
            spec = dict(proj["spec"])
            spec.setdefault("commercial", {})["release_ready"] = True
            detail = json.dumps(report)
            self._ensure_pending_gate(project_id, "approve_release", detail=detail)
            self.db.update_project(project_id, spec=spec)
        return report

    def ship(self, project_id: int) -> dict[str, Any]:
        """Package release zip. Requires certify passed + approve_release."""
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        if not self._run_ok(project_id, "certify"):
            raise PermissionError("Ship requires a passed certify run")
        self._require_gate(project_id, "approve_release")

        name = proj["plugin_name"]
        status, ok, output = build_broker.run_package_ship(name)
        self.db.add_run(project_id, "ship", ok, f"[{status}] {output}")

        artifact: str | None = None
        if status == "ok":
            found = find_release_artifact(name)
            artifact = str(found) if found else f"release/{name}/"
            spec = dict(proj["spec"])
            spec["release_artifact"] = artifact
            self.db.update_project(project_id, state=FactoryState.SHIP.value, spec=spec)
        elif status == "skipped":
            spec = dict(proj["spec"])
            spec["ship_skipped"] = True
            spec["ship_skip_reason"] = output.strip()
            self.db.update_project(project_id, spec=spec)
        self.db.upsert_product_wo(project_id, build_product_wo(self.db.get_project(project_id) or {}))
        result = self.enrich_project(self.db.get_project(project_id) or {})
        result["ship_status"] = status
        result["last_ok"] = ok
        result["last_output_tail"] = output[-4000:]
        result["artifact"] = artifact
        return result

    def pipeline(self, project_id: int) -> dict[str, Any]:
        """Data-driven pipeline status from full-lifecycle workflow."""
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)

        workflow = modules.load_full_lifecycle_workflow()
        stages_def = workflow.get("stages") or []
        furthest = self._furthest_verified(proj)
        furthest_i = _stage_index(furthest)

        pending_gates = self.db.pending_gates(project_id)
        phases: list[dict[str, Any]] = []
        blocking_gate: str | None = None
        next_command: str | None = None
        next_sop: str | None = None

        for stage in stages_def:
            sid = stage["id"]
            idx = _stage_index(sid)
            gate = stage.get("gate")
            status = "pending"
            run = self._latest_run(project_id, sid)

            if idx >= 0 and idx < furthest_i:
                status = "done"
            elif idx == furthest_i:
                if run and not run["ok"] and sid in ("build", "test", "certify", "ship"):
                    status = "failed"
                else:
                    status = "done"
            else:
                if run and not run["ok"] and sid in ("build", "test", "certify", "ship"):
                    status = "failed"
                else:
                    status = "pending"

            # Gate blocks only when the gate row exists (pending) or is explicitly required now.
            if sid == "scaffold" and not self.db.gate_approved(project_id, "approve_architecture"):
                if furthest_i < _stage_index("scaffold"):
                    status = "blocked"
                    gate = "approve_architecture"

            if sid == "impl" and "approve_gui" in pending_gates:
                if furthest_i >= _stage_index("design"):
                    status = "blocked"
                    gate = "approve_gui"

            if sid == "ship":
                if proj["state"] == FactoryState.SHIP.value:
                    status = "done"
                elif self._run_ok(project_id, "certify") and not self.db.gate_approved(
                    project_id, "approve_release"
                ):
                    status = "blocked"
                    gate = "approve_release"

            if status == "blocked" and gate and blocking_gate is None:
                blocking_gate = gate
            if status in ("pending", "blocked", "failed") and next_command is None:
                next_command = (stage.get("command") or "").replace("{id}", str(project_id))
                next_sop = stage.get("sop")

            phases.append(
                {
                    "id": sid,
                    "label": stage.get("label", sid),
                    "status": status,
                    "gate": gate,
                    "sop": stage.get("sop"),
                    "command": (stage.get("command") or "").replace("{id}", str(project_id)),
                    "verify": stage.get("verify"),
                }
            )

        if blocking_gate is None and pending_gates:
            for g in ("approve_architecture", "approve_gui", "approve_release"):
                if g in pending_gates:
                    blocking_gate = g
                    break
            if blocking_gate is None:
                blocking_gate = pending_gates[0]

        return {
            "project_id": project_id,
            "plugin_name": proj["plugin_name"],
            "workflow_id": workflow.get("id", "workflow/full-lifecycle"),
            "phases": phases,
            "blocking_gate": blocking_gate,
            "next_command": next_command,
            "next_sop": next_sop,
        }

    def dsp_research(self, project_id: int, goal: str) -> dict[str, Any]:
        result = start_research(project_id, goal, self.db)
        self._notify_pending(project_id, "approve_dsp_promotion", detail=json.dumps(result))
        return result

    def promote_dsp_verified(self, project_id: int, algorithm_id: str) -> dict[str, Any]:
        if not self.db.gate_approved(project_id, "approve_dsp_promotion"):
            self._require_gate(project_id, "approve_dsp_promotion")
        return {"promoted": algorithm_id, "target": f"knowledge/verified/dsp/{algorithm_id}.json"}

    def export_cursor_prompt(self, project_id: int, role: str = "cpp") -> str:
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        if role == "cpp":
            return model_router.export_cursor_impl_prompt(proj["spec"])
        agent = model_router.load_agent_prompt(role)
        return f"{agent}\n\n---\n\n{model_router.export_cursor_impl_prompt(proj['spec'])}"
