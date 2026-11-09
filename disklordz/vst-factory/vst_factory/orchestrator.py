from __future__ import annotations

import json
from typing import Any

from vst_factory import apc_bridge, build_broker, model_router, modules, references
from vst_factory.build_broker import find_vst3_bundle
from vst_factory.certification import evaluate_certification, load_recent_run_flags
from vst_factory.db import FactoryDB
from vst_factory.dsp_research import start_research
from vst_factory.models import FactoryState, PluginClass
from vst_factory.product_wo import build_product_wo
from vst_factory.slack_notify import notify_gate


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
        return self.db.get_project(pid)  # type: ignore[return-value]

    def _require_gate(self, project_id: int, gate: str) -> None:
        if not self.db.gate_approved(project_id, gate):
            pending = self.db.pending_gates(project_id)
            raise PermissionError(f"Gate '{gate}' not approved. Pending: {pending}")

    def scaffold(self, project_id: int) -> dict:
        self._require_gate(project_id, "approve_architecture")
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        root = apc_bridge.repo_root()
        name = proj["plugin_name"]
        pclass = proj["plugin_class"]
        spec = proj["spec"]

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
        out = self.db.get_project(project_id)
        assert out is not None
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
        result = self.db.get_project(project_id)
        assert result is not None
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
        result = self.db.get_project(project_id)
        assert result is not None
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
            self.db.add_approval(project_id, gate="approve_release", status="pending", detail=detail)
            self._notify_pending(project_id, "approve_release", detail=detail)
            self.db.update_project(project_id, spec=spec)
        return report

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
