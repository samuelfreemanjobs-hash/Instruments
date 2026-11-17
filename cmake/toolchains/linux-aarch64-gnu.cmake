# Cross-compile JUCE VST3 for Linux aarch64 (e.g. Akai MPC Sample / Melodee).
#
# Usage (from vst/<PluginName>):
#   cmake -B build-aarch64 -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchains/linux-aarch64-gnu.cmake \
#         -DCMAKE_BUILD_TYPE=Release
#
# Requires host packages (Debian/Ubuntu):
#   gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

if(NOT DEFINED MPC_CROSS_PREFIX)
  set(MPC_CROSS_PREFIX aarch64-linux-gnu)
endif()

set(CMAKE_C_COMPILER ${MPC_CROSS_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${MPC_CROSS_PREFIX}-g++)

# Skip host pkg-config when cross-compiling GUI deps (use static/minimal JUCE flags in plugin CMake).
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)