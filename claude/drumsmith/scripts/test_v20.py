"""v2 consistency tests, no JUCE required. Native C++ tests run separately via CTest."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def _cxx_compiler() -> str | None:
    for cand in (os.environ.get("CXX"), "g++-12", "g++", "c++"):
        if cand and shutil.which(cand):
            return cand
    return None


class V20Tests(unittest.TestCase):
    def test_native_project_files(self):
        for path in [
            "native/CMakeLists.txt",
            "native/include/DrumEngine.h",
            "native/include/WavWriter.h",
            "native/juce/PluginProcessor.cpp",
            "native/juce/PluginEditor.cpp",
            "native/tests/test_engine.cpp",
        ]:
            self.assertTrue((ROOT / path).is_file(), path)

    def test_native_build_and_run_if_compiler_available(self):
        if not shutil.which("cmake"):
            self.skipTest("CMake unavailable")
        cxx = _cxx_compiler()
        if not cxx:
            self.skipTest("C++ compiler unavailable")
        with tempfile.TemporaryDirectory() as td:
            cmake = [
                "cmake",
                "-S",
                str(ROOT / "native"),
                "-B",
                td,
                f"-DCMAKE_CXX_COMPILER={cxx}",
                "-DCMAKE_BUILD_TYPE=Release",
            ]
            subprocess.run(cmake, check=True, capture_output=True)
            subprocess.run(["cmake", "--build", td, "-j"], check=True, capture_output=True)
            subprocess.run(
                ["ctest", "--test-dir", td, "--output-on-failure"],
                check=True,
                capture_output=True,
            )


if __name__ == "__main__":
    unittest.main()
