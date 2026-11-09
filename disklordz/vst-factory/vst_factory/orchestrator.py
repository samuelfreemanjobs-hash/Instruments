from __future__ import annotations

from vst_factory import apc_bridge, build_broker, model_router, modules, references
from vst_factory.db import FactoryDB
from vst_factory.models import FactoryState, PluginClass


class Orchestrator:
    def __init__(self, db: FactoryDB) -> None:
        self.db = db

    def create_project(
        self,
        *,
        prompt: str,
        plugin_class: PluginClass,
        plugin_name: str | None,
    ) -> dict:
        root = apc_bridge.repo_root()
        name = plugin_name or apc_bridge.suggest_plugin_name(prompt)
        if self.db.get_project_by_name(name):
            raise ValueError(f"Project name already exists: {name}")
        spec = modules.build_spec_from_modules(
            plugin_name=name,
            plugin_class=plugin_class.value,
            prompt=prompt,
        )
        pid = self.db.insert_project(
            plugin_name=name,
            plugin_class=plugin_class.value,
            state=FactoryState.INTAKE.value,
            spec=spec,
        )
        self.db.update_project(pid, state=FactoryState.DREAM.value)
        references.update_liaison_greenfield_count(self.db.count_greenfield())
        return self.db.get_project(pid)  # type: ignore[return-value]

    def scaffold(self, project_id: int) -> dict:
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
            bundle = build_broker.find_vst3_bundle(name)
            if bundle:
                spec = proj["spec"]
                spec["artefact"] = str(bundle)
                self.db.update_project(project_id, state=FactoryState.BUILD.value, spec=spec)
            else:
                self.db.update_project(project_id, state=state)
        else:
            attempts = proj["repair_attempts"] + 1
            self.db.update_project(project_id, state=state, repair_attempts=attempts)
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
        result = self.db.get_project(project_id)
        assert result is not None
        result["last_ok"] = ok
        result["last_output_tail"] = output[-4000:]
        return result

    def export_cursor_prompt(self, project_id: int, role: str = "cpp") -> str:
        proj = self.db.get_project(project_id)
        if not proj:
            raise KeyError(project_id)
        if role == "cpp":
            return model_router.export_cursor_impl_prompt(proj["spec"])
        agent = model_router.load_agent_prompt(role)
        return f"{agent}\n\n---\n\n{model_router.export_cursor_impl_prompt(proj['spec'])}"
