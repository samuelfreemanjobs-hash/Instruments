"""Monorepo QA pipeline — mirrors CI and optional product smoke tests."""

from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from dataclasses import asdict, dataclass, field
from enum import Enum
from pathlib import Path
from typing import Callable

OPS_ROOT = Path(__file__).resolve().parent
REPO_ROOT = OPS_ROOT.parent
REPORTS_DIR = OPS_ROOT / "reports"
ERROR_LOG = OPS_ROOT / "error_log.txt"
BUILD_LOG = OPS_ROOT / "last_build.log"
REPORT_JSON = OPS_ROOT / "last_run_report.json"
ORCHESTRATOR = REPO_ROOT / "scripts" / "vst" / "run_pluginval.py"


class StageId(str, Enum):
    CONFIGURE = "configure"
    BUILD = "build"
    ARTEFACTS = "artefacts"
    DETERMINISM = "determinism"
    GOLDEN = "golden"
    WAVE909_TESTS = "wave909_tests"
    PLUGINVAL = "pluginval"
    DISKLORDZ_WEB = "disklordz_web"


@dataclass
class StageResult:
    stage: StageId
    ok: bool
    duration_s: float
    output: str = ""

    def to_dict(self) -> dict:
        d = asdict(self)
        d["stage"] = self.stage.value
        return d


@dataclass
class PipelineConfig:
    configure: bool = True
    build_all: bool = True
    check_artefacts: bool = True
    determinism: bool = True
    golden: bool = True
    wave909_tests: bool = True
    pluginval: bool = True
    disklordz_web: bool = False
    build_jobs: int | None = None


@dataclass
class PipelineRun:
    profile: str
    started_at: float
    finished_at: float = 0.0
    stages: list[StageResult] = field(default_factory=list)

    @property
    def ok(self) -> bool:
        return all(s.ok for s in self.stages)

    def to_dict(self) -> dict:
        return {
            "profile": self.profile,
            "ok": self.ok,
            "started_at": self.started_at,
            "finished_at": self.finished_at,
            "duration_s": self.finished_at - self.started_at,
            "stages": [s.to_dict() for s in self.stages],
        }


def _run(cmd: list[str], *, cwd: Path | None = None, env: dict | None = None) -> tuple[int, str]:
    proc = subprocess.run(
        cmd,
        cwd=str(cwd or REPO_ROOT),
        capture_output=True,
        text=True,
        env=env,
        check=False,
    )
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, out


def stage_configure() -> StageResult:
    t0 = time.time()
    build = REPO_ROOT / "build"
    if (build / "CMakeCache.txt").is_file():
        return StageResult(StageId.CONFIGURE, True, time.time() - t0, "CMake cache present.\n")
    cmd = [
        "cmake",
        "-B",
        "build",
        "-DCMAKE_BUILD_TYPE=Release",
    ]
    if sys.platform != "win32":
        cmd.extend(["-DCMAKE_CXX_COMPILER=g++-12", "-DCMAKE_C_COMPILER=gcc-12"])
    code, out = _run(cmd)
    return StageResult(StageId.CONFIGURE, code == 0, time.time() - t0, out)


def stage_build(jobs: int | None) -> StageResult:
    t0 = time.time()
    cmd = ["cmake", "--build", "build"]
    if jobs:
        cmd.extend(["-j", str(jobs)])
    else:
        cmd.append("-j")
    if sys.platform == "win32":
        cmd.extend(["--config", "Release"])
    code, out = _run(cmd)
    BUILD_LOG.write_text(out, encoding="utf-8")
    return StageResult(StageId.BUILD, code == 0, time.time() - t0, out)


def stage_artefacts() -> StageResult:
    t0 = time.time()
    checks = [
        REPO_ROOT / "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3",
        REPO_ROOT / "build/JDUpgraded_artefacts/Release/CLAP/JD Upgraded.clap",
        REPO_ROOT / "build/JDUpgraded_artefacts/Release/Standalone/JD Upgraded",
    ]
    lines: list[str] = []
    ok = True
    for path in checks:
        exists = path.is_dir() or (path.is_file() and os.access(path, os.X_OK))
        mark = "OK" if exists else "MISSING"
        if not exists:
            ok = False
        lines.append(f"[{mark}] {path.relative_to(REPO_ROOT)}")
    return StageResult(StageId.ARTEFACTS, ok, time.time() - t0, "\n".join(lines) + "\n")


def stage_determinism() -> StageResult:
    t0 = time.time()
    render = REPO_ROOT / "build/OfflineRender"
    diff = REPO_ROOT / "build/SpectralDiff"
    if not render.is_file() or not diff.is_file():
        return StageResult(
            StageId.DETERMINISM,
            False,
            time.time() - t0,
            "OfflineRender or SpectralDiff missing — run full build first.\n",
        )
    a, b = "/tmp/jdupg-ops-a.wav", "/tmp/jdupg-ops-b.wav"
    code1, o1 = _run([str(render), a, "0", "60", "100", "0.5", "44100"])
    code2, o2 = _run([str(render), b, "0", "60", "100", "0.5", "44100"])
    code3, o3 = _run([str(diff), a, b, "--max-rms-db", "-80", "--max-spectral-db", "0.25"])
    out = o1 + o2 + o3
    ok = code1 == code2 == code3 == 0
    return StageResult(StageId.DETERMINISM, ok, time.time() - t0, out)


def stage_golden() -> StageResult:
    t0 = time.time()
    script = REPO_ROOT / "tests/golden/verify_golden.sh"
    if not script.is_file():
        return StageResult(StageId.GOLDEN, False, time.time() - t0, "verify_golden.sh not found\n")
    code, out = _run(["bash", str(script)])
    return StageResult(StageId.GOLDEN, code == 0, time.time() - t0, out)


def stage_wave909_tests() -> StageResult:
    t0 = time.time()
    tests_bin = REPO_ROOT / "build/Wave909Tests"
    if tests_bin.is_file() and os.access(tests_bin, os.X_OK):
        code, out = _run([str(tests_bin)])
        return StageResult(StageId.WAVE909_TESTS, code == 0, time.time() - t0, out)
    code, out = _run(["ctest", "--test-dir", "build", "-R", "Wave909", "--output-on-failure"])
    if code != 0 and "No tests were found" in out:
        return StageResult(
            StageId.WAVE909_TESTS,
            True,
            time.time() - t0,
            "Wave909Tests not built — skipped (enable wave909_tests after Wave909 target build).\n",
        )
    return StageResult(StageId.WAVE909_TESTS, code == 0, time.time() - t0, out)


def stage_pluginval() -> StageResult:
    t0 = time.time()
    if not ORCHESTRATOR.is_file():
        return StageResult(StageId.PLUGINVAL, False, time.time() - t0, "Missing run_pluginval.py\n")
    env = os.environ.copy()
    bin_pv = OPS_ROOT / "bin" / "pluginval"
    if bin_pv.is_file():
        env["PLUGINVAL_BIN"] = str(bin_pv)
    code, out = _run([sys.executable, str(ORCHESTRATOR), "--default-artefacts"], env=env)
    return StageResult(StageId.PLUGINVAL, code == 0, time.time() - t0, out)


def stage_disklordz_web() -> StageResult:
    t0 = time.time()
    website = REPO_ROOT / "disklordz/website"
    inbox = REPO_ROOT / "disklordz/daw-inbox"
    if not (website / "package.json").is_file():
        return StageResult(StageId.DISKLORDZ_WEB, False, time.time() - t0, "disklordz/website missing\n")
    chunks: list[str] = []
    ok = True
    for label, cwd, cmd in [
        ("website npm ci", website, ["npm", "ci"]),
        ("website npm run build", website, ["npm", "run", "build"]),
        ("daw-inbox npm ci", inbox, ["npm", "ci"]),
        ("daw-inbox syntax", inbox, ["node", "--check", "bin/watch.mjs"]),
    ]:
        if label.startswith("daw-inbox") and not (inbox / "package.json").is_file():
            chunks.append(f"[skip] {label}\n")
            continue
        code, out = _run(cmd, cwd=cwd)
        chunks.append(f"=== {label} (exit {code}) ===\n{out[-4000:]}\n")
        if code != 0:
            ok = False
            break
    return StageResult(StageId.DISKLORDZ_WEB, ok, time.time() - t0, "".join(chunks))


STAGE_RUNNERS: dict[StageId, Callable[[], StageResult]] = {
    StageId.CONFIGURE: stage_configure,
    StageId.ARTEFACTS: stage_artefacts,
    StageId.DETERMINISM: stage_determinism,
    StageId.GOLDEN: stage_golden,
    StageId.WAVE909_TESTS: stage_wave909_tests,
    StageId.PLUGINVAL: stage_pluginval,
    StageId.DISKLORDZ_WEB: stage_disklordz_web,
}


def _build_stage(jobs: int | None) -> Callable[[], StageResult]:
    return lambda: stage_build(jobs)


PROFILES: dict[str, PipelineConfig] = {
    "ci": PipelineConfig(disklordz_web=False),
    "full": PipelineConfig(disklordz_web=True),
    "plugin-quick": PipelineConfig(
        configure=False,
        build_all=False,
        check_artefacts=False,
        determinism=False,
        golden=False,
        wave909_tests=False,
        pluginval=True,
    ),
    "dsp-only": PipelineConfig(
        configure=False,
        build_all=False,
        check_artefacts=False,
        determinism=True,
        golden=True,
        wave909_tests=False,
        pluginval=False,
        disklordz_web=False,
    ),
}


def resolve_stages(config: PipelineConfig) -> list[tuple[StageId, Callable[[], StageResult]]]:
    stages: list[tuple[StageId, Callable[[], StageResult]]] = []
    if config.configure:
        stages.append((StageId.CONFIGURE, STAGE_RUNNERS[StageId.CONFIGURE]))
    if config.build_all:
        stages.append((StageId.BUILD, _build_stage(config.build_jobs)))
    if config.check_artefacts:
        stages.append((StageId.ARTEFACTS, STAGE_RUNNERS[StageId.ARTEFACTS]))
    if config.determinism:
        stages.append((StageId.DETERMINISM, STAGE_RUNNERS[StageId.DETERMINISM]))
    if config.golden:
        stages.append((StageId.GOLDEN, STAGE_RUNNERS[StageId.GOLDEN]))
    if config.wave909_tests:
        stages.append((StageId.WAVE909_TESTS, STAGE_RUNNERS[StageId.WAVE909_TESTS]))
    if config.pluginval:
        stages.append((StageId.PLUGINVAL, STAGE_RUNNERS[StageId.PLUGINVAL]))
    if config.disklordz_web:
        stages.append((StageId.DISKLORDZ_WEB, STAGE_RUNNERS[StageId.DISKLORDZ_WEB]))
    return stages


def run_pipeline(
    profile: str = "ci",
    config: PipelineConfig | None = None,
    *,
    stop_on_fail: bool = True,
    progress_cb: Callable[[StageResult], None] | None = None,
) -> PipelineRun:
    cfg = config or PROFILES.get(profile, PROFILES["ci"])
    run = PipelineRun(profile=profile, started_at=time.time())
    REPORTS_DIR.mkdir(parents=True, exist_ok=True)

    for stage_id, runner in resolve_stages(cfg):
        result = runner()
        run.stages.append(result)
        if progress_cb:
            progress_cb(result)
        if not result.ok:
            tail = result.output[-20000:]
            ERROR_LOG.write_text(
                f"Stage failed: {stage_id.value}\n\n{tail}",
                encoding="utf-8",
            )
            if stop_on_fail:
                break

    run.finished_at = time.time()
    if run.ok:
        ERROR_LOG.unlink(missing_ok=True)

    REPORT_JSON.write_text(json.dumps(run.to_dict(), indent=2), encoding="utf-8")
    stamp = time.strftime("%Y%m%d-%H%M%S")
    (REPORTS_DIR / f"run-{stamp}.json").write_text(json.dumps(run.to_dict(), indent=2), encoding="utf-8")
    return run


def fleet_status() -> dict[str, bool]:
    """Quick health of key build outputs."""
    paths = {
        "JD Upgraded VST3": REPO_ROOT / "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3",
        "JD Upgraded CLAP": REPO_ROOT / "build/JDUpgraded_artefacts/Release/CLAP/JD Upgraded.clap",
        "OfflineRender": REPO_ROOT / "build/OfflineRender",
        "Wave909 VST3": REPO_ROOT / "build/Wave909_artefacts/Release/VST3/Wave909.vst3",
        "pluginval cached": OPS_ROOT / "bin/pluginval",
    }
    return {name: p.is_dir() or p.is_file() for name, p in paths.items()}
