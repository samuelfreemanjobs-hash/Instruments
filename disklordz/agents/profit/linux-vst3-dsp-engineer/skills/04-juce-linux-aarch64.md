# Skill 04 — JUCE Linux aarch64 (MPC Sample / Melodee)

## Goal

Produce:

`Plugin.vst3/Contents/aarch64-linux/Plugin.so`

## Build

```bash
git submodule update --init vst/JUCE
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu g++-13 \
  libstdc++-13-dev-arm64-cross
# Optional: multiarch :arm64 X11/alsa/freetype dev for full GUI link

./tools/build-vst-aarch64-linux.sh MyPlugin
./scripts/vst/verify-mpc-sample-vst3.sh \
  vst/MyPlugin/build-aarch64/.../MyPlugin.vst3
```

Toolchain file: `cmake/toolchains/linux-aarch64-gnu.cmake`.

## juceaide / host compiler trap [executed]

JUCE bootstraps **juceaide** with host `/usr/bin/c++`. If alternatives point `c++` → `clang++` without libstdc++, configure fails.

- Ensure `g++` links: `sudo update-alternatives --set c++ /usr/bin/g++`
- Build script fails early with a clear message if host C++ cannot link

## Link / glibc

- Cross link needs arm64 dev libraries (`libasound`, `freetype`, X11 for GUI plugins)
- Binary may require **glibc ≤ device**; very new Ubuntu cross toolchains can produce unusable `.so` on older firmware — rebuild in older LTS or match Melodee Docker image when load fails despite correct `file` output

## Install on device (Melodee)

Copy full `.vst3` bundle to **`Melodee/Plugins/`** on SD card (see [keremimo/melodee-sample README](https://github.com/keremimo/melodee-sample)).

## Anti-patterns

- Shipping `x86_64-linux` build to Melodee
- Copying only `.so` without bundle `Contents/Resources/moduleinfo.json`
- Building `all` targets on cross-build (runs tests that need host libs) — script targets `<Name>_VST3` only