# linux-vst3-dsp-engineer — learnings (validated lessons)

Linux JUCE VST3, aarch64 cross-build, Melodee/MPC Sample. Format: [MEMORY.md](MEMORY.md).

### L-0001 · Melodee wants aarch64-linux VST3 path
- **date:** 2026-11-15
- **area:** mpc-sample, melodee, vst3
- **when:** targeting Akai MPC Sample with Melodee firmware
- **lesson:** Plugin bundles must place the `.so` under `Contents/aarch64-linux/`, installed under `Melodee/Plugins/` on SD; x86_64-linux and Windows/macOS bundles do not load.
- **do:** `./tools/build-vst-aarch64-linux.sh <Name>` then `./scripts/vst/verify-mpc-sample-vst3.sh <bundle>`.
- **evidence:** keremimo/melodee-sample README; cross-build session on Instruments branch
- **confidence:** inspected
- **status:** active

### L-0002 · juceaide needs a working host g++
- **date:** 2026-11-15
- **area:** cross-compile, juce, cmake
- **when:** CMAKE_TOOLCHAIN_FILE aarch64 cross-build on cloud image
- **lesson:** JUCE sub-cmake compiles juceaide with `/usr/bin/c++`; clang default without libstdc++ breaks before target link.
- **do:** `update-alternatives --set c++ /usr/bin/g++` or ensure `g++-13` can link; see build script `ensure_host_cxx_for_juceaide`.
- **evidence:** MeterTap build-aarch64 configure failure on VM
- **confidence:** executed
- **status:** active

### L-0003 · Open MPC vs JUCE lane
- **date:** 2026-11-15
- **area:** architecture, mpc-os, melodee
- **when:** user asks for "MPC plugin" without naming host
- **lesson:** Melodee on Sample loads **JUCE VST3 aarch64** and Open MPC folders; MPC Live/Force native community path is **Open MPC VST2 + skin**, not Instruments `vst/` JUCE unless also shipping desktop VST3.
- **do:** Ask host (Melodee vs MPC OS vs DAW); read `skills/07-open-mpc-porting.md`.
- **evidence:** open-mpc/mpc-vst-plugins README; mpc-vst-melodee-chip vst.json
- **confidence:** inspected
- **status:** active